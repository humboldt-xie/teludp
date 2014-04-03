/*
 * =====================================================================================
 *
 *       Filename:  main.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/03/2014 11:30:37 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Humboldt(), 
 *        Company:  
 *
 * =====================================================================================
 */
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <arpa/inet.h>
#define FD_STDIN 0

int loop(int sockfd,struct sockaddr_in addr)
{
	fd_set readset;
	fd_set writeset;
	fd_set exceptset;
	FD_ZERO(&readset);
	FD_ZERO(&writeset);
	FD_ZERO(&exceptset);
	int maxfd=sockfd;
	char buf[255];
	while(1){
		FD_SET(0,&readset);
		FD_SET(sockfd,&readset);
		struct timeval timeout;
		timeout.tv_sec=3;
		timeout.tv_usec=0;
		int ret=select(maxfd + 1,&readset,&writeset,&exceptset, &timeout);
		switch(ret)
		{
			case -1: 
				perror("select error");
				exit(-1);
				break; //select错误，退出程序
			case 0:
				/*timeout*/
				continue; 
			default:
				break;
		}
		if(FD_ISSET(0,&readset)){
			int len=read(0,buf,255);
			buf[len]=0;
			sendto(sockfd,buf,len,0,(const struct sockaddr*)&addr,sizeof(addr));
		}
		if(FD_ISSET(sockfd,&readset)){
			int len=sizeof(addr);
			int l=recvfrom(sockfd,buf,255,0,(struct sockaddr *)&addr,&len);
			buf[l]=0;
			printf(buf);
		}
	}
}
int server(int port)
{
	int sockfd;
	struct sockaddr_in servaddr,cliaddr;
	sockfd = socket(AF_INET,SOCK_DGRAM,0); /* create a socket */
	/* init servaddr */
	bzero(&servaddr,sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(port);
	/* bind address and port to socket */
	if(bind(sockfd,(struct sockaddr *)&servaddr,sizeof(servaddr)) == -1)
	{
		perror("bind error");
		exit(1);
	}
	return sockfd;
}
int client(char*ip,int port,struct sockaddr_in *paddress)
{
	int sock = socket(AF_INET,SOCK_DGRAM,0);
	paddress->sin_family=AF_INET;
	paddress->sin_port=htons(port);
	paddress->sin_addr.s_addr=inet_addr(ip);
	if(connect(sock,(struct sockaddr *)paddress,sizeof(struct sockaddr_in )) == -1)
	{
		perror("connect error");
		exit(1);
	}
	return sock;
}


int main(int argc, char * argv[])
{
	
	int sock=-1;
    struct sockaddr_in address;

	if(argc==2){
		sock=server(atoi(argv[1]));
	}else if(argc==3){
		sock=client(argv[1],atoi(argv[2]),&address);
	}else{
		perror("usage:%s [server_addr] port ");
		exit(1);
	}
	if(sock<0){
		perror("error");
		exit(1);
	}
	loop(sock,address);
	return 0;
}
