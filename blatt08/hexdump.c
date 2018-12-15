#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

void hexdump(int sd, char* buffer, int length)
{
	size_t i, o;
	char buf[1024];

	for(i=0; i<=length; i+=16)
	{
		sprintf(buf, "%06lx : ", i);
		for(o=0; o<16 && !(buffer[i+o-1]=='\0' && (i>0 || o>0)); o++)
			sprintf(buf, "%02x ", (unsigned char)buffer[i+o]);
		for(; o<16; o++)
			fputs("   ", buf);
		fputs("  ", buf);
		for(o=0; o<16 && !(buffer[i+o-1] == '\0' && (i>0 || o>0)); o++)
			fputc((buffer[i+o]<32||buffer[i+o]>126) ? '.' : buffer[i+o], buf);
		fputc('\n', buf);
	}
	
	// Sending the hexdump back to the Client

	if (write(sd, buf, sizeof(buf) == -1)) {
		perror("Error while sending Hexdump\n");
		exit(-1);
	}

}
