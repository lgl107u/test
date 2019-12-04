#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/sendfile.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>

char paginaweb[] = 
"HTTP/1.1 200 Ok\r\n"
"Content-Type: text/html; charset=UTF-8\r\n\r\n"
"<!DOCTYPE html>\r\n"
"<html><head><title>server web</title>\r\n"
"<style>body { background-color: #A9D0F5 }</style></head>\r\n"
"<body><center><h1>Welcome to web server</h1><br>\r\n"
"<img src=\"icon.jpg\" alt=\"picture\" width=\"500\" ></center>\r\n"
"<form action=\"\" method=\"POST\"><center><input type=\"file\" name=\"upload\"><p></center>\r\n"
"<center><input type=\"submit\" value=\"submit\"></center></form>\r\n"
"</body></html>\r\n";
int xx;

char *subti(char *fname)
{
	char *finish,*ptr,num[2];
	num[0]=xx+'0';
	ptr = fname;
	finish = (char*) malloc(1000);
	
	while(*ptr!='.' && *ptr!='\0') *ptr++;	
	*ptr--;
	int test = *ptr;
	if( test>=49 && test<=57)	
	{
		*ptr = xx+'0';
		strcpy(finish,fname);
		return finish;
	}
	*ptr++;
	if(*ptr=='\0')
	{
		num[1]='\0';
		strcat(fname,num);
		strcpy(finish,fname);
	}

	else 
	{
		int j=0;
		ptr = fname;
		while(*ptr!='.')
		{
			finish[j]=*ptr;
			j++;
			*ptr++;
		}
		strcat(finish,num);
		strcat(finish,ptr);
	}
	return finish;
}

	

int main(int argc, char *argv[]) 
	{
	struct sockaddr_in server_addr, client_addr; 
	socklen_t sin_len =sizeof(client_addr);
	int srvsock, fd_client;
	char buf[2048];
	int fdimg;
	int i=0, Bind;
	struct stat stbuf;
	int on = 1;
	long int sz= 0;

	srvsock = socket(AF_INET, SOCK_STREAM, 0); //create socket
        
	if (srvsock < 0) {
		perror("socket");
		exit(1);	
	}
        
	setsockopt(srvsock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(int));

	server_addr.sin_family = AF_INET; //IPv4
	server_addr.sin_addr.s_addr = INADDR_ANY; //host to network long int 32 bits
	server_addr.sin_port = htons(8080); //port
	Bind = bind(srvsock, (struct sockaddr *) &server_addr, sizeof(server_addr));
	if (Bind < 0)
	{
		perror("bind"); //port to in error
		close(srvsock);
		exit(1);
	}
	int Listen = listen(srvsock,10);
	if ( Listen== -1)
	{
		perror("listen"); //person can connect to the server < 0 , error
		close(srvsock);
		exit(1);
	}	
	
	while(1) {	
		fd_client = accept(srvsock, (struct sockaddr *) &client_addr, &sin_len);
		if (fd_client < 0)
		{
			perror("Accept errror...\n");
			continue;
		}

		printf("Establish connection.\n");
		if (!fork()) {
			
			memset(buf, 0, 2048);
			read(fd_client, buf, 2047);
			printf("buf %s\n",buf);	

			if(!strncmp(buf, "GET /favicon.ico", 16))
			{
				fdimg = open("favicon.jpg", O_RDONLY);
				int sent = sendfile(fd_client, fdimg, NULL, 10000);
				//printf("sent favicon:%d\n",sent);
				close(fdimg);
			}
			if(strstr(buf, "POST /"))
			{
				int nw;
				FILE *test;
				char *temp=strstr(buf,"upload=");
				int i=8;
				int flag=0;
				while(--i>0) *temp++;

				fdimg = open(temp, O_RDONLY); 
				xx=1;
				while(0<1)
				{
					test = fopen(temp,"r");					
					if(test)
					{
						char *finish = subti(temp);
						char store[1000];
						strcpy(store,finish);
						temp = store;
						xx++;	
					}	
					else break;
				}
				
				printf("fname baru %s\n",temp);
				nw = open(temp, O_CREAT | O_WRONLY | O_TRUNC, 0600);
				sendfile(nw, fdimg, NULL, 10000000000);
				close(fdimg);
				
			}
			if(!strncmp(buf, "GET /icon.jpg", 13))
			{
				fdimg = open("icon.jpg", O_RDONLY);
				int sent = sendfile(fd_client, fdimg, NULL, 100000);
				//printf("sent icon`:%d\n",sent);
				close(fdimg);
			}


			else write(fd_client, paginaweb, sizeof(paginaweb)-1);

			close(fd_client);
			printf("Disconnecting\n");
			exit(0);
			
		}
		if(fork() >0 ) sleep (100); 
		close(fd_client);
	}	


	return 0;
}
