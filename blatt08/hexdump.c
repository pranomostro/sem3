#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

void hexdump(int sd, char* buffer, int length)
{
	printf("Buffer: %sMit Länge: %d\n", buffer, strlen(buffer));
	size_t i, o;
	int index = 0;
	char buf[1024];

	for(i=0; i<=length; i+=16)
	{
		sprintf(&buf[index], "%06lx : ", i);
		index += 9;
		printf("After first: %s\nLength: %d\n", buf, index);
		for(o=0; o<16 && !(buffer[i+o-1]=='\0' && (i>0 || o>0)); o++) {
			sprintf(&buf[index], "%02x ", (unsigned char)buffer[i+o]);
			index += 3;
		}
		printf("Added second line: %s\nLength: %d\n", buf, index);
		for(; o<16; o++) {
			sprintf(&buf[index], "   ");
			index += 3;
		}
		sprintf(&buf[index], "   ");
		index += 3;
		printf("Added third line: %s\nLength: %d\n", buf, index);
		for(o=0; o<16 && !(buffer[i+o-1]== '\0' && (i>0 || o>0)); o++) {
			sprintf(&buf[index++], (buffer[i+o]<32||buffer[i+o]>126) ? '.' : buffer[i+o]);
			printf("Added in line: %s\nLength: %d\n", buf, index);
		}
		printf("Added fourth line: %s\nLength: %d\n", buf, index);
		sprintf(&buf[index++], '\n');
	}
	
	// Sending the hexdump back to the Client

	if (write(sd, buf, sizeof(buf)) == -1) {
		perror("Error while sending Hexdump\n");
		close(sd);
		exit(-1);
	}

}
