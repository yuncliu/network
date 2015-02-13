#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netinet/ip_icmp.h>   //Provides declarations for icmp header
#include <netinet/udp.h>   //Provides declarations for udp header
#include <netinet/ip.h>    //Provides declarations for ip header
#include <arpa/inet.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define SERV_PORT  9999

/* 
 *     96 bit (12 bytes) pseudo header needed for tcp header checksum calculation 
 */
struct pseudo_header
{
    u_int32_t source_address;
    u_int32_t dest_address;
    u_int8_t placeholder;
    u_int8_t protocol;
    u_int16_t tcp_length;
};

static int func();
static void* recv_thread();
static int raw_socket;
/*
   Generic checksum calculation function
   */
static unsigned short csum(unsigned short *ptr,int nbytes);
static unsigned short tcpchecksum(struct tcphdr* hdr,struct sockaddr_in* dest, 
        char* data, size_t data_len);
    

int main() {
    pthread_t pid_recv = 0;
    pthread_create(&pid_recv, NULL, recv_thread, NULL);
    func();
    return 0;
}

int func() {
    struct sockaddr_in dest;
    struct sockaddr_in src;
    char buf[1024] = {0};
    memset(&src, 0x00, sizeof(struct sockaddr_in));
    src.sin_port = htons(9998);

    dest.sin_family = AF_INET;
    dest.sin_port = htons(SERV_PORT);
    if (inet_pton(AF_INET, "127.0.0.1", &(dest.sin_addr)) != 1) {
        printf("Bad Address!\n");
        return(1);
    }

    if ((raw_socket = socket(AF_INET, SOCK_RAW, IPPROTO_TCP)) < 0) {
        printf("socket() failed!\n");
        return(1);
    }

    bind(raw_socket, (struct sockaddr*)&src, sizeof(src));
    printf("%s\n", strerror(errno));

    struct tcphdr* tcpHdr = (struct tcphdr*)buf;
    struct pseudo_header psh;
    tcpHdr->source = htons(900);
    tcpHdr->dest = htons(SERV_PORT);
    tcpHdr->seq = 0x0000FFFF; //32 bit sequence number, initially set to zero
    tcpHdr->ack_seq = 0x0; //32 bit ack sequence number, depends whether ACK is set or not
    tcpHdr->doff = 5; //4 bits: 5 x 32-bit words on tcp header
    tcpHdr->res1 = 0; //4 bits: Not used
    //tcpHdr->cwr = 0; //Congestion control mechanism
    //tcpHdr->ece = 0; //Congestion control mechanism
    tcpHdr->urg = 0; //Urgent flag
    tcpHdr->ack = 0; //Acknownledge
    tcpHdr->psh = 0; //Push data immediately
    tcpHdr->rst = 0; //RST flag
    tcpHdr->syn = 1; //SYN flag
    tcpHdr->fin = 0; //Terminates the connection
    tcpHdr->window = htons(155);//0xFFFF; //16 bit max number of databytes 
    tcpHdr->check = 0; //16 bit check sum. Can't calculate at this point
    tcpHdr->urg_ptr = 0; //16 bit indicate the urgent data. Only if URG flag is set

    /*
    u_int32_t source_address;
    u_int32_t dest_address;
    u_int8_t placeholder;
    u_int8_t protocol;
    u_int16_t tcp_length;
    */
    psh.source_address = inet_addr("127.0.0.1");
    psh.dest_address = dest.sin_addr.s_addr;
    psh.placeholder = 0;
    psh.protocol = IPPROTO_TCP;
    psh.tcp_length = htons(sizeof(struct tcphdr));

    int psize = sizeof(struct pseudo_header) + sizeof(struct tcphdr);
    char* pseudogram = (char*)malloc(psize);
    memcpy(pseudogram , (char*) &psh , sizeof (struct pseudo_header));
    memcpy(pseudogram + sizeof(struct pseudo_header) , tcpHdr , sizeof(struct tcphdr));
    tcpHdr->check = csum( (unsigned short*) pseudogram , psize);

    printf("%ld\n", sizeof(tcpHdr));

    ssize_t send_len = sendto(raw_socket, buf, sizeof(struct tcphdr), 0, (struct sockaddr*) &dest, sizeof(struct sockaddr));
    printf("send:%ld\n", send_len);

    if (send_len < 0)  {
        printf("sendto() failed!\n");
        printf("%s\n", strerror(errno));
        return(1);
    }

    char buff[1024] = {0};
    socklen_t socklen = sizeof(struct sockaddr);
    ssize_t recvlen = recvfrom(raw_socket, buff, 1024, 0, (struct sockaddr*) &dest, &socklen);
    //ssize_t recvlen = recv(sd, buff, sizeof(struct tcphdr), 1024);
    printf("recv:%ld\n", recvlen);
    struct tcphdr* recvhdr = (struct tcphdr*)(buff+(sizeof(struct tcphdr)));
    recvhdr->seq+=2;
    recvhdr->check = tcpchecksum(recvhdr,&dest, NULL, 0);
    printf("seq:%u\n", recvhdr->seq);
    printf("ack:%u\n", recvhdr->ack);

    if (sendto(raw_socket, buff, sizeof(struct tcphdr), 0, (struct sockaddr*) &dest, sizeof(struct sockaddr)) < 0)  {
        printf("sendto() failed!\n");
        printf("%s\n", strerror(errno));
        return(1);
    }
    sleep(10);


    return(0);
}

unsigned short csum(unsigned short *ptr,int nbytes) 
{
    register long sum;
    unsigned short oddbyte;
    register short answer;

    sum=0;
    while(nbytes>1) {
        sum+=*ptr++;
        nbytes-=2;
    }
    if(nbytes==1) {
        oddbyte=0;
        *((u_char*)&oddbyte)=*(u_char*)ptr;
        sum+=oddbyte;
    }

    sum = (sum>>16)+(sum & 0xffff);
    sum = sum + (sum>>16);
    answer=(short)~sum;

    return(answer);
}

unsigned short tcpchecksum(struct tcphdr* hdr,struct sockaddr_in* dest, 
        char* data, size_t data_len)
{
    /*
    u_int32_t source_address;
    u_int32_t dest_address;
    u_int8_t placeholder;
    u_int8_t protocol;
    u_int16_t tcp_length;
    */
    struct pseudo_header psh;
    psh.source_address = inet_addr("127.0.0.1");
    psh.dest_address = dest->sin_addr.s_addr;
    psh.placeholder = 0;
    psh.protocol = IPPROTO_TCP;
    psh.tcp_length = htons(sizeof(struct tcphdr));

    int psize = sizeof(struct pseudo_header) + sizeof(struct tcphdr) + data_len;
    char* pseudogram = (char*)malloc(psize);
    memcpy(pseudogram , (char*) &psh , sizeof (struct pseudo_header));
    memcpy(pseudogram + sizeof(struct pseudo_header) , hdr , sizeof(struct tcphdr));
    memcpy(pseudogram + sizeof(struct pseudo_header) + sizeof(struct tcphdr), data, data_len);
    unsigned short checksum = csum( (unsigned short*) pseudogram , psize);
    free(pseudogram);
    return checksum;
}

void* recv_thread()
{
    int saddr_size , data_size;
    struct sockaddr saddr;
    struct in_addr in;
    unsigned char buffer[1024] = {0};
    while(1)
    {
        saddr_size = sizeof(saddr);
        memset(buffer, 0x00, sizeof(buffer));
        data_size = recvfrom(raw_socket , buffer , 65536 , 0 , &saddr , &saddr_size);
        if (data_size < 0 )
        {
            printf("error [%s]\n", strerror(errno));
            return NULL;
        }
        struct iphdr *iph = (struct iphdr*)buffer;
        unsigned short iphdrlen = iph->ihl*4;
        struct tcphdr *tcph=(struct tcphdr*)(buffer + iphdrlen);
        if (IPPROTO_TCP == iph->protocol)
        {
            if (SERV_PORT == ntohs(tcph->source))
            {
                printf("get response from server\n");
                printf("ack = [%u]\n",ntohl(tcph->ack_seq));
            }
        }
    }
    return NULL;
}
