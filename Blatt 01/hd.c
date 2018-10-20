#include <stdio.h>
#include <string.h>

int main(int argc, char** argv)
{
	char c;
	size_t s;
	FILE* f=fopen("log", "w");

	for(s=1; s<argc; s++)
	{
		hexdump(f, argv[s], strlen(argv[s]));
		fputc('\n', f);
	}
	fclose(f);

	f=fopen("log", "r");

	while((c=fgetc(f))!=EOF)
		putchar(c);

	fclose(f);

	remove("log");

	return 0;
}
