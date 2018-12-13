#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <ctype.h>
#include <arpa/inet.h>

#define MYPORT 44444

int main(int argc, char** argv)
{
	char buffer[0x10000];
	struct sockaddr_in sin;
	struct sockaddr_in sender;
	int sockfd;
	int len, i;
	socklen_t sender_len;

	if(argc!=1)
	{
		fprintf(stderr, "Usage: %s\n", argv[0]);
		exit(0);
	}

	sockfd=socket(AF_INET, SOCK_DGRAM, 0);

	if(sockfd==-1)
	{
		perror("Socket could not be created");
		exit(1);
	}

	sin.sin_family=AF_INET;
	sin.sin_port=htons(MYPORT);
	sin.sin_addr.s_addr=INADDR_ANY;

	if(bind(sockfd, (struct sockaddr *) &sin, sizeof(sin))==-1)
	{
		perror("Could not bind socket");
		exit(1);
	}

	while(1)
	{
		sender_len=sizeof(sender);
		len=recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *)&sender, &sender_len);
		if(len==-1)
		{
			perror("Receiving failed");
			exit(1);
		}
		printf("Received packet from %s:%d, with size of %d\n", inet_ntoa(sender.sin_addr), ntohs(sender.sin_port), len);
		if(len>0)
			for(i=0; i<len; i++)
			{
				if(isalpha(buffer[i]))
					buffer[i]=buffer[i]^0x20;
				if(isdigit(buffer[i]))
					buffer[i]=(9-(buffer[i]-'0'))+'0';
			}
		sendto(sockfd, buffer, len, 0, (struct sockaddr *) &sender, sender_len);
	}

	return 0;
}
