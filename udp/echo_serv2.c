#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>

#define PORT 9999

int main(int argc, char**argv)
{
   int sockfd,n;
   struct sockaddr_in servaddr,cliaddr;

   socklen_t len;
   sockfd=socket(AF_INET,SOCK_DGRAM,0);

   bzero(&servaddr,sizeof(servaddr));
   servaddr.sin_family = AF_INET;
   servaddr.sin_addr.s_addr=htonl(INADDR_ANY);
   servaddr.sin_port=htons(9999);

   bind(sockfd,(struct sockaddr *)&servaddr,sizeof(servaddr));

   char buffer[548];
   struct sockaddr_storage src_addr;
   struct iovec iov[1];
   iov[0].iov_base=buffer;
   iov[0].iov_len=sizeof(buffer);

   struct msghdr message;
   message.msg_name=&src_addr;
   message.msg_namelen=sizeof(src_addr);
   message.msg_iov=iov;
   message.msg_iovlen=1;
   message.msg_control=0;
   message.msg_controllen=0;

   for (;;)
   {
      len = sizeof(struct sockaddr_in);
      n = recvmsg(sockfd, &message, 0);
      struct sockaddr_in *src = message.msg_name;
      uint16_t port = ntohs(src->sin_port);
      printf("recv from [%s]:[%u]  [%s]\n", inet_ntoa(src->sin_addr), port, buffer);
      sendto(sockfd, buffer, strlen(buffer) ,0,(struct sockaddr *)src, len);
   }
   return 0;
}
