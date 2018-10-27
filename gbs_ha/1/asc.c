#include <stdio.h>

int main(int argc, char** argv)
{
	printf("Oct\tDec\tHex\tChar\n");
	for(int c=0; c<128; c++)
		printf("%03o\t%d\t%02x\t%C\n", c, c, c, c);
	return 0;
}
