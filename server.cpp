#include <sys/types.h>    //here can find defined families of domens      
#include <sys/socket.h>
#include <cstdlib>
#include <stdio.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <unistd.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <string.h>

#define BUF_SIZE 1024
#define LISTEN_BACKLOG 50
#define PORT 1100
#define NUM_MAX_FD 100000
#define FD_BUF_SIZE 100

using namespace std;

int main(int argc, char** argv) {

    int sfd, cfd;
    struct sockaddr_in servAddr, clientAddr;  //handle ip addr of socket

    sfd = socket(AF_INET, SOCK_STREAM|SOCK_NONBLOCK, 0); //AF_INET - IPv4, AF_LOCAL - local communications
    if(sfd == -1){                            //recieve a socket descriptor
        perror("socket error");
        exit(1);
    }
    
    servAddr.sin_family = AF_INET;
    //addr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAddr.sin_addr.s_addr=inet_addr("127.0.0.1");
    servAddr.sin_port = htons(PORT);
    if ( bind(sfd, (struct sockaddr*)&servAddr, sizeof(servAddr)) == -1){
        perror("bind error");
        exit(1);
    }
    
    if ( listen(sfd, LISTEN_BACKLOG) == -1) { 
        close(sfd);
        perror( "listen error\n");
        exit(1);
    }
    
    fd_set rfds;
    struct timeval tv, tvStart, tvEnd;
    tv.tv_sec = 1;
    int retval;
    FD_ZERO(&rfds);
    char buf[BUF_SIZE];
    char errBuf[BUF_SIZE];
    strcpy(errBuf, "Too many clients; can't take one more");
    int fdBuf[FD_BUF_SIZE] = {0};
    int i = 0, count = 0;
    
    
    while(1){
        cfd = accept(sfd, NULL, NULL); //create new fd, that not in listen state
         /*if( cfd == -1){
            perror( "accept error\n");
            exit(1);
        }*/
        
        if(i<FD_BUF_SIZE && cfd != -1){
            FD_SET(cfd, &rfds);
            fdBuf[i] = cfd; 
            printf("%d was pull in %d \n", cfd, i);
            i++;
        }
        else    
            write(cfd, errBuf, strlen(errBuf));
        
        retval = select(NUM_MAX_FD, &rfds, NULL, NULL, &tv);
        if(retval != 0){
            for(int j=0; j<FD_BUF_SIZE; j++){
                if(fdBuf[j] == 0)
                    break;
                    if(FD_ISSET(fdBuf[j], &rfds) !=  0){
                        count = read(fdBuf[j], buf, BUF_SIZE);
                        sleep(1);
                        printf("%d your speed is %d bytes\n", fdBuf[j], count);
                        fflush(stdout);
                    }
            }
        }
    }
    return 0;
}

