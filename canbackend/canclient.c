#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h> 
#include "can.h"

int main(int argc, char *argv[])
{
    int sockfd = 0, n = 0;
    char recvBuff[1024];
    struct sockaddr_in serv_addr; 

    if(argc != 2)
    {
        printf("\n Usage: %s <ip of server> \n",argv[0]);
        return 1;
    } 

    memset(recvBuff, '0',sizeof(recvBuff));
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Error : Could not create socket \n");
        return 1;
    } 

    memset(&serv_addr, '0', sizeof(serv_addr)); 

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(22222); 

    if(inet_pton(AF_INET, argv[1], &serv_addr.sin_addr)<=0)
    {
        printf("\n inet_pton error occured\n");
        return 1;
    } 

    if( connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
       printf("\n Error : Connect Failed \n");
       return 1;
    } 

    CanMsg msg;
    //memcpy(msg.sync, "STANFORD16", 10);
    msg.stdId = 0;
    msg.extId = 0x0FFF;
    msg.ide = 1;
    msg.rtr = 0;
    msg.dlc = 8;
    uint64_t counter=0;
    while(1)
    {
        struct timeval tv;
        gettimeofday(&tv,NULL);
        uint64_t timestamp = 1000000*tv.tv_sec+tv.tv_usec;
        if((timestamp-1000) > msg.timestamp)
        {
            msg.timestamp = timestamp;
            msg.data[0] = ((uint8_t*)(&counter))[0];
            msg.data[1] = ((uint8_t*)(&counter))[1];
            msg.data[2] = ((uint8_t*)(&counter))[2];
            msg.data[3] = ((uint8_t*)(&counter))[3];
            msg.data[4] = ((uint8_t*)(&counter))[4];
            msg.data[5] = ((uint8_t*)(&counter))[5];
            msg.data[6] = ((uint8_t*)(&counter))[6];
            msg.data[7] = ((uint8_t*)(&counter))[7];
             
            counter++;
            if(send(sockfd, &msg, sizeof(msg), 0) == -1)
            {
                perror("send() error");
            }
        }
        CanMsg rcvMsg;
        int size = recv(sockfd, &rcvMsg, sizeof(rcvMsg), MSG_DONTWAIT);
        if(size == sizeof(rcvMsg))
        {
            printf("stdId: 0x%.4x, extId: 0x%.8x, rtr: %i, dlc: %i, data: 0x%.16lx\n",rcvMsg.stdId, rcvMsg.extId, rcvMsg.rtr, rcvMsg.dlc, *((uint64_t *)rcvMsg.data)); 
        }
        usleep(10);
    }

    return 0;
}
