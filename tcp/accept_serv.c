#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <arpa/inet.h>

int main(int argc, char* argv[]) {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr("127.0.0.3");
    addr.sin_port = htons(9999);
    socklen_t sock_len = sizeof(addr);
    bind(fd, (struct sockaddr*)&addr, sock_len);
    listen(fd, 10);

    struct sockaddr_in client_addr;


    int client = accept(fd, (struct sockaddr*)&client, &sock_len);

    return 0;
}
