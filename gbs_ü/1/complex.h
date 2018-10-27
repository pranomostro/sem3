#ifndef UNTITLED4_COMPLEX_H
#define UNTITLET4_COMPLEX_H

struct Complex_number
{
	double real;
	double imaginary;
};

typedef struct Complex_number Complex;

Complex add(Complex a, Complex b);
Complex multiply(Complex a, Complex b);

#endif
