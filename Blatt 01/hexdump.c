#include <stdio.h>

void hexdump(FILE* output, char* buffer, int length)
{
	size_t i, o;

	for(i=0; i<=length; i+=16)
	{
		fprintf(output, "%06x : ", i);
		for(o=0; o<16&&!(buffer[i+o-1]=='\0'&&(i>0||o>0)); o++)
			fprintf(output, "%02x ", buffer[i+o]);
		for(; o<16; o++)
			fputs("   ", output);
		for(o=0; o<16&&!(buffer[i+o-1]=='\0'&&(i>0||o>0)); o++)
			fputc((buffer[i+o]<32||buffer[i+o]>126)?'.':buffer[i+o], output);
		fputc('\n', output);
	}
}
