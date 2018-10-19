#include <stdio.h>

int main(int argc, char** argv)
{
	printf("Oct\tDec\tHex\tChar\n");
	for(char c=0; c<127; c++)
		printf("%o\t%d\t%02x\t%c\n", c, c, c, c);
	return 0;
}
