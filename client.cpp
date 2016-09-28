#include <sys/types.h>    //here can find defined families of domens      
#include <sys/socket.h>
#include <cstdlib>
#include <arpa/inet.h>
#include <stdio.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <unistd.h>
#include <string.h>

#define BUF_SIZE 256
#define PORT 1100

using namespace std;

int main(int argc, char** argv) {
    int cfd;
    struct sockaddr_in addr;  //handle ip addr of socket
    char buf[BUF_SIZE];
    
    
    cfd = socket(AF_INET, SOCK_STREAM, 0); //AF_INET - IPv4, AF_LOCAL - local communications
    if(cfd == -1){                            //recieve a socket descriptor
        perror("socket error");
        exit(1);
    }
    
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    //addr.sin_addr.s_addr = inet_addr("192.168.1.51");
    addr.sin_port = htons(PORT);
    
     if(connect(cfd, (struct sockaddr *)&addr, sizeof(addr)) < 0) // установка соединения с сервером
    {
        perror("connect error");
        exit(1);
    }
    strcpy(buf, "Hello, server");
    while(1){
    
        if(write(cfd, buf, strlen(buf))==0){
            perror("write");
            break;
        }
    }
     
    return 0;
}

