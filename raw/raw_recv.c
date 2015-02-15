#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

static void process_packet(char* buf, int size);

int main(void)
{
    int sock_raw = 0;
    struct sockaddr_in saddr;
    char buf[1024];

    if ((sock_raw = socket(AF_INET, SOCK_RAW, IPPROTO_RAW)) < 0) {
        perror("error:");
        exit(EXIT_FAILURE);
    }

    int fromlen = sizeof(saddr);

    while(1) {
        memset(buf, 0, sizeof(buf));
        ssize_t recvlen = recvfrom(sock_raw, buf, sizeof(buf), 0, (struct sockaddr *)&saddr, &fromlen);
        if (recvlen < 0) {
            printf("error [%s]\n", strerror(errno));
            continue;
        }
        process_packet(buf, recvlen);
    }
    return 0;
}

static void process_packet(char* buf, int size) {
    struct iphdr *iph = (struct iphdr*)buf;
    int iphlen = iph->ihl*4;
    char* data = (char*)(buf + iphlen);
    printf("data [%s]\n", data);
    return;
}
