/* Copy */

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h> 
#include <pthread.h>
#include "can.h"

#define MAX_MSG_LEN 20

struct transmitParams{
    int sockfd;
    int usbfd;
};

bool isValidPacket (char firstChar){
    switch (firstChar){
        case 't': case 'T': case 'r': case 'R': return true;
        default: return false;
    }
}

void parsePacket (CANMsg *msg, char *msgstr)
{
    char buf[17];
    strncpy(buf, msgstr + 1, 3);
    buf[3] = '\0';
    msg->stdId = 0;
    msg->rtr = (msgstr[0] == 'r') ? 1 : 0;
    /* strtol: start, end, base */
    msg->extId = (uint32_t)strtol(buf, NULL, 16);
    msg->dlc = *((uint8_t *)msgstr[4]);
    int i;
    buf[2] = '\0';
    for (i = 0; i < msg->dlc; i++){
        strncpy (buf, msgstr + 5 + (2 * i), 2);
        uint8_t math = (uint8_t)strtol(buf, NULL, 16);
        //strncpy(msg->data, msgstr + 5, msg->dlc * 2);
        msg->data[i] = math;
    }
    printf("Id is %d, dlc is %d\n", msg->extId, msg->dlc);
    for (i = 0; i < msg->dlc; i++){
        printf ("Data[%i] is %i\n", i, msg->data[i]);
    }
}

void readPackets (CANMsg *msg, int usbfd)
{
    char buf[MAX_MSG_LEN];
    buf[0] = '\0';
    int i = 0;
    while (i < MAX_MSG_LEN && buf[i] != '\r'){
        int bytes = read(usbfd, buf, 1);
        if (bytes == -1){
            /* Fail */
            return;
        }
        /* If it was well formed, parse and reset. */
        if (isValidPacket(buf[0])){
            parsePacket (msg, buf);
            sendPacket (sockfd, msg);
            buf[0] = '\0';
            i = 0;
        }   /* Only parse it if it was well formed. */         
    }
}

void *transmitCAN(void *data)
{
    struct transmitParams params = *((struct transmitParams *) data);
    int sockfd = params.sockfd;
    int usbfd = params.usbfd;
    uint64_t counter = 0;
    CanMsg msg;
    msg.stdId = 0;
    msg.extId = 0x0FFF;
    msg.ide = 1;
    msg.rtr = 0;
    msg.dlc = 8;
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
        usleep(1000);

    }
}   
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

    int usbfd;  /* Open usb port */
    struct transmitParams params;
    params.sockfd = sockfd;
    params.usbfd = usbfd;
    
    
    pthread_t txThread;
    if(pthread_create(&txThread, NULL, transmitCAN, &params))
        return -1;

 

    while(1)
    {
        CanMsg rcvMsg;
        int size = recv(sockfd, &rcvMsg, sizeof(rcvMsg), 0);	
        if(size == sizeof(rcvMsg))
        {
            printf("stdId: 0x%.4x, extId: 0x%.8x, rtr: %i, dlc: %i, data: 0x%.16lx\n",rcvMsg.stdId, rcvMsg.extId, rcvMsg.rtr, rcvMsg.dlc, *((uint64_t *)rcvMsg.data)); 
        }
    }

    pthread_exit(NULL);

    return 0;
}
