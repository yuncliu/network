#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>

#define PORT 9999
#define IP_ADDR "127.0.0.1"

int main(int argc, char**argv)
{
   int sockfd = 0;
   struct sockaddr_in servaddr,cliaddr;
   char sendbuf[1000];
   char recvbuf[1000];


   sockfd=socket(AF_INET,SOCK_DGRAM,0);

   bzero(&servaddr,sizeof(servaddr));
   servaddr.sin_family = AF_INET;
   servaddr.sin_addr.s_addr=inet_addr(IP_ADDR);
   servaddr.sin_port=htons(9999);

    snprintf(sendbuf, sizeof(sendbuf), "%s", "hellow");
   while (1) {
      sendto(sockfd,sendbuf,strlen(sendbuf),0,
             (struct sockaddr *)&servaddr,sizeof(servaddr));
      int recvlen=recvfrom(sockfd,recvbuf,10000,0,NULL,NULL);
      recvbuf[recvlen]=0;
      printf("recv[%s]\n", recvbuf);
      sleep(1);
   }
   return 0;
}
