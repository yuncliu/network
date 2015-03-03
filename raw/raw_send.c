#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

#define DEST "127.0.0.1"

int main(void)
{

    int sock_raw;
    struct sockaddr_in daddr;
    char buf[50];
    /* point the iphdr to the beginning of the packet */

    if ((sock_raw = socket(AF_INET, SOCK_RAW, IPPROTO_RAW)) < 0) {
        perror("error:");
        exit(EXIT_FAILURE);
    }

    daddr.sin_family = AF_INET;
    daddr.sin_port = 0; /* not needed in SOCK_RAW */
    inet_pton(AF_INET, DEST, (struct in_addr *)&daddr.sin_addr.s_addr);
    memset(daddr.sin_zero, 0, sizeof(daddr.sin_zero));
#ifdef __linux 
    struct iphdr *ip = (struct iphdr *)buf;  
    ip->ihl = 5;
    ip->version = 4;
    ip->tos = 0;
    ip->tot_len = htons(40);    /* 16 byte value */
    ip->frag_off = 0;       /* no fragment */
    ip->ttl = 64;           /* default value */
    ip->protocol = IPPROTO_RAW; /* protocol at L4 */
    ip->check = 0;          /* not needed in iphdr */
    ip->saddr = daddr.sin_addr.s_addr;
    ip->daddr = daddr.sin_addr.s_addr;
    int iplen = ip->ihl * 4;
#else
    struct ip *ip = (struct ip *)buf;  
    ip->ip_hl = 5;
    ip->ip_v = 4;
    ip->ip_tos = 0;
    ip->ip_len = htons(20);    /* 16 byte value */
    ip->ip_off = 0;       /* no fragment */
    ip->ip_ttl = 64;           /* default value */
    ip->ip_p = IPPROTO_RAW; /* protocol at L4 */
    ip->ip_sum = 0;          /* not needed in iphdr */
    ip->ip_src = (struct in_addr)daddr.sin_addr;
    ip->ip_dst = (struct in_addr)daddr.sin_addr;
    int iplen = ip->ip_hl * 4;
#endif

    //snprintf(buf + iplen, sizeof(buf) - iplen, "%s", "ip protocol directly");
    //printf("ip hdr len  = %d\n", iplen);
    snprintf(buf, sizeof(buf), "%s", "ip protocol directly");


    while(1) {
        sleep(1);
        ssize_t send_len = sendto(sock_raw, (char *)buf, sizeof(buf), 0, 
                (struct sockaddr *)&daddr, (socklen_t)sizeof(daddr));
        if (send_len <= 0) {
            printf("error [%s]\n", strerror(errno));
            break;
        }
    }
    return 0;
}
