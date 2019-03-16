#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

void hexdump(int sd, char* buffer, int length)
{
	size_t i, o;
	int index = 0;
	char buf[10240]; // Should be enough for hexdump since input is max 1024 there are ~4 chars in hexdump + some constant stuff

	for(i=0; i<=length; i+=16)
	{
		sprintf(&buf[index], "%06lx : ", i);
		index += 9;
		
		for(o=0; o<16 && !(i + o >= length && (i>0 || o>0)); o++) {
			sprintf(&buf[index], "%02x ", (unsigned char)buffer[i+o]);
			index += 3;
		}
		
		for(; o<16; o++) {
			sprintf(&buf[index], "   ");
			index += 3;
		}

		sprintf(&buf[index], "  ");
		index += 2;

		for(o=0; o<16 && !(i + o >= length && (i>0 || o>0)); o++) {
			buf[index++] = (buffer[i+o]<32||buffer[i+o]>126) ? '.' : buffer[i+o];
		}
		sprintf(&buf[index++], "\n");
	}
	
	// Sending the hexdump back to the Client

	if (write(sd, buf, index) < 0) {
		perror("Error while sending Hexdump\n");
		close(sd);
		exit(-1);
	}

}
