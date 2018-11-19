#include <stdio.h>

unsigned int factorial(unsigned int n)
{
	if(n==1)
		return 1;
	return n*factorial(n-1);
}

int main(void)
{
	printf("%d\n", factorial(10));
	return 0;
}
