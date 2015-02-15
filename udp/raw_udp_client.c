#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <netinet/ip.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <arpa/inet.h>


#define PORT 9999
#define IP_ADDR "127.0.0.1"

void* send_thr(void*);
void* recv_thr(void*);
void* recv_thr(void*);
void process_packet(char* buf, int size);
void process_udp_packet(char* buf, int size);

static int sock_raw;
static struct sockaddr_in servaddr;


int main(int argc, char**argv)
{
    //sock_raw = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
    sock_raw = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);

    bzero(&servaddr,sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(IP_ADDR);
    servaddr.sin_port = htons(PORT);
    pthread_t pid_recv = 0;
    pthread_t pid_send = 0;;
    pthread_create(&pid_recv, NULL, recv_thr, NULL);
    pthread_join(pid_recv, NULL);

    return 0;
}

void* send_thr(void* p) {
    while(1) {
        sleep(1);
    }
    return NULL;
}

void* recv_thr(void* p) {
    printf("thread start\n");
    struct sockaddr saddr;
    //saddr.sin_family = AF_INET;
    //saddr.sin_addr.s_addr = inet_addr(IP_ADDR);
    //saddr.sin_port = htons(PORT);
    socklen_t saddr_size = sizeof(saddr);

    char buf[1024] = {0};
    int data_size = 0;
    while(1) {
        memset(buf, 0x00, sizeof buf);
        data_size = recvfrom(sock_raw , buf , 1024, 0 , &saddr , &saddr_size);
        if (data_size <= 0) {
            continue;
        }
        process_packet(buf, data_size);
    }
    return NULL;
}

void process_packet(char* buf, int size) {
    struct iphdr *iph = (struct iphdr*)buf;
    switch(iph->protocol) {
        case IPPROTO_UDP:
            process_udp_packet(buf, size);
            break;
        default:
            printf("other protocol\n");
            break;
    }
    return;
}

void process_udp_packet(char* buf, int size)
{
    struct iphdr *iph = (struct iphdr*)buf;
    int iphdrlen = iph->ihl*4;
    struct udphdr* updh = (struct udphdr*)(buf + iphdrlen);
    if (PORT == htons(updh->source))
    {
        printf("get a udp packet\n");
        char* data = (char*)(buf + iphdrlen + sizeof(struct udphdr));
        printf("raw recv [%s]\n", data);
    }
}
