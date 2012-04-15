#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

/* port we're listening on */
#define PORT 22222

int main(int argc, char *argv[])
{
    fd_set master;
    fd_set read_fds;
    struct sockaddr_in serveraddr;
    struct sockaddr_in clientaddr;
    int fdmax;
    int listener;
    int newfd;
    char buf[1024];
    int nbytes;
    int yes = 1;
    int addrlen;
    int i, j;
    FD_ZERO(&master);
    FD_ZERO(&read_fds);

    if((listener = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        exit(1);
    }

    if(setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
    {
        exit(1);
    }

    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = INADDR_ANY;
    serveraddr.sin_port = htons(PORT);
    memset(&(serveraddr.sin_zero), '\0', 8);

    if(bind(listener, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) == -1)
    {
        exit(1);
    }

    if(listen(listener, 10) == -1)
    {
        exit(1);
    }

    /* add the listener to the master set */
    FD_SET(listener, &master);
    /* keep track of the biggest file descriptor */
    fdmax = listener; 

    for(;;)
    {
        read_fds = master;

        if(select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1)
        {
            exit(1);
        }

        for(i = 0; i <= fdmax; i++)
        {
            if(FD_ISSET(i, &read_fds))
            { 
                if(i == listener)
                {
                    // handle new connections 
                    addrlen = sizeof(clientaddr);
                    if((newfd = accept(listener, (struct sockaddr *)&clientaddr, &addrlen)) == -1)
                    {
                        perror("Error Accepting Request");
                    }
                    else
                    {
                        FD_SET(newfd, &master); 
                        if(newfd > fdmax)
                        { 
                            // keep track of the maximum 
                            fdmax = newfd;
                        }
                        printf("%s: New connection from %s on socket %d\n", argv[0], inet_ntoa(clientaddr.sin_addr), newfd);
                    }
                }
                else
                {
                    /* handle data from a client */
                    if((nbytes = recv(i, buf, sizeof(buf), 0)) <= 0)
                    {
                        if(nbytes == 0)
                            printf("%s: socket %d hung up\n", argv[0], i);
                        else
                            perror("recv() error");

                        close(i);
                        FD_CLR(i, &master);
                    }
                    else
                    {
                        /* we got some data from a client*/
                        for(j = 0; j <= fdmax; j++)
                        {
                            /* send to everyone! */
                            if(FD_ISSET(j, &master))
                            {
                                /* except the listener and ourselves */
                                if(j != listener && j != i)
                                {
                                    if(send(j, buf, nbytes, 0) == -1)
                                        perror("send() error");
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return 0;
}

