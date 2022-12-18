#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <time.h>
#include <errno.h>

ssize_t FullWrite(int fd, const void *buf, size_t count)
{
	size_t nleft;
	ssize_t nwritten;
	nleft = count;
	while (nleft > 0) {

	/* repeat until no left */
	if ( (nwritten = write(fd, buf, nleft)) < 0) {
		if (errno == EINTR) { /* if interrupted by system call */
		continue;
		/* repeat the loop */
		} else {
		exit(nwritten); /* otherwise exit with error */
		}
	}

	nleft -= nwritten;
	/* set left to write */
	buf +=nwritten;
	/* set pointer */
	}

	return (nleft);
}

ssize_t FullRead(int fd, void *buf, size_t count)
{
    size_t nleft;
    ssize_t nread;
    nleft = count;
    while (nleft > 0)
    {
	    if( (nread=read(fd, buf, nleft))<0)
      {
        //se c'è stato errore
		    if(errno=EINTR){ continue; }
		    else{exit(nread);}
	    }else if(nread==0){ break;}//chiuso il canale

	    nleft-=nread;
	    buf+=nread;
    }
      buf=0;
      return (nleft);
 }


int main()
{
    int socketfd, listenfd, i;
    char buffer[4096];
    char ricezione[4096];


    if ((socketfd = socket(AF_INET,SOCK_STREAM,0)) < 0){
        perror("socket");
        exit(1);
    }
    struct sockaddr_in server;
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_family = AF_INET;
    server.sin_port = htons(52000);

    if ((bind(socketfd,(struct sockaddr*)&server,sizeof(server))) < 0){
        perror("bind");
        exit(1);
    }



    if (listen(socketfd,5) < 0){
        perror("listen");
        exit(1);
    }

    int array[32]={0};
    char *stringa = "IL SERVER E' STATO CONTATTATO DA TE!";

    fd_set readset, writeset;
    FD_ZERO(&readset);
    FD_ZERO(&writeset);
    int fd_open[32]={0}, fd, maxfd,fd_connected;
    fd_open[socketfd] = 1;
    maxfd = socketfd;
    while(1){

        for (int i=listenfd; i<=maxfd; i++)
            if(fd_open[i])
                FD_SET(i,&readset);


        fd = select(maxfd+1,&readset,NULL,NULL,NULL);


        sleep(1);

        ssize_t length;


        if(FD_ISSET(socketfd,&readset)){

            puts("Nuovo client connesso!");
            fd_connected = accept(socketfd,NULL,NULL);
            fd_open[fd_connected] = 1;
            if (maxfd < fd_connected)
                maxfd = fd_connected;
            fd--;

            printf("Client disponibile! maxfd = %d\n",maxfd);
            }







        i = socketfd;



        while(fd > 0 && i < 32){
            i++;
            if(fd_open[i] == 0)
                continue;

            if(FD_ISSET(i,&readset)){
                fd--;
                ssize_t bytes = read(i,buffer,sizeof(buffer));
                if(bytes < 0){

                    puts("read error");
                    exit(0);
                }
                else if(bytes == 0){
                    printf("\nClient col fd %d: DISCONNESSO\n",i);
                    fd_open[i] = 0;
                    FD_CLR(i,&readset);
                    close(i);
                    if (maxfd == i)
                        while (fd_open[--i] == 0){
                            maxfd = i;
                            break;
                }
                }
                else{
                printf("\nMessaggio arrivato dal client con il fd %d:\n",i);
                write(1,buffer,bytes);
                memset(buffer,0,sizeof(buffer));
                }
            }



        }

    }


    return 0;
}
