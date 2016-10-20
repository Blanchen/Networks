#include <sys/types.h>    //here can find defined families of domens      
#include <sys/socket.h>
#include <cstdlib>
#include <stdio.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <unistd.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <poll.h>


#include <string.h>

#define BUF_SIZE 256
#define LISTEN_BACKLOG 50
#define PORT 1100
#define NUM_MAX_FD 100000
#define FD_BUF_SIZE 100

using namespace std;

double momentSpeed[FD_BUF_SIZE] = {0};
double timeBuf[FD_BUF_SIZE] = {0};
double dataBuf[FD_BUF_SIZE] = {0};
    
void printSpeed(int count){
        for(int i = 0; i<count; i++)
            fprintf(stderr,"%d moment speed is %.2f bytes, average speed is %.2f kbytes/sec \n", i, momentSpeed[i], dataBuf[i]/timeBuf[i]*1000/1024);

    }  


int main(int argc, char** argv) {

    int sfd, cfd;
    struct sockaddr_in servAddr, clientAddr;  //handle ip addr of socket

    sfd = socket(AF_INET, SOCK_STREAM|SOCK_NONBLOCK, 0); //AF_INET - IPv4, AF_LOCAL - local communications
    if(sfd == -1){                            //recieve a socket descriptor
        perror("socket error");
        exit(1);
    }
    //|SOCK_NONBLOCK

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
    
    fd_set rfds, srfds;
    FD_ZERO(&rfds);
    char buf[BUF_SIZE];
    char errBuf[BUF_SIZE];
    strcpy(errBuf, "Too many clients; can't take one more");
    int i = 0, count = 0, time = 0;
    //printf("before client");


    struct pollfd fds[FD_BUF_SIZE];
    int timeout = (1000);
    struct timeval stSec, endSec;
    struct timeval clSt, clEnd;
    int retval;
    
    gettimeofday(&stSec, NULL);          
    while(1){

            cfd = accept(sfd, NULL, NULL); //create new fd, that not in listen state
            if(i<FD_BUF_SIZE && cfd != -1){
                FD_SET(cfd, &rfds);
                fds[i].fd = cfd;
                fds[i].events = POLLIN; 
                //printf("%d was pull in %d \n", cfd, i);
                i++;
            }
            else    
                write(cfd, errBuf, strlen(errBuf));
        
            retval = poll(fds, i, timeout);
            //printSpeed(i);
            if(retval > 0){
                //printf("client was\n");
                for(int j=0; j<i; j++){
                    gettimeofday(&endSec, NULL);
                    if(endSec.tv_sec - stSec.tv_sec){
                        gettimeofday(&stSec, NULL);
                        printSpeed(i);
                    }    
                    if(fds[j].revents == POLLIN){
                            gettimeofday(&clSt, NULL);
                            count = read(fds[j].fd, buf, BUF_SIZE);
                        //printf("%d \n", count);
                            gettimeofday(&clEnd, NULL);
                            momentSpeed[j] = count;    
                            time = (double)((clEnd.tv_sec - clSt.tv_sec)*1000000 + (clEnd.tv_usec - clSt.tv_usec)/1000);
                            timeBuf[j]+=time;
                            dataBuf[j]+=count; 
                    }
                }
            }
    }
    return 0;
}

