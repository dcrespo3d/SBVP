#include "complejo.h"
#include <math.h>

complejo *compl(float real, float imag)
{
	static complejo aux;
	*(long *)(&aux.re)=*(long *)(&real);
	*(long *)(&aux.im)=*(long *)(&imag);
	return &aux;
}

complejo operator +(complejo s1,complejo s2)
{
	return *compl(s1.re + s2.re, s1.im + s2.im);
}

complejo operator -(complejo s2,complejo s1)
{
	return *compl(s2.re - s1.re, s2.im - s1.im);
}

complejo operator *(complejo f1,complejo f2)
{
	return *compl((f1.re * f2.re) - (f1.im * f2.im),
				(f1.re * f2.im) + (f2.re * f1.im));
}

complejo operator /(complejo d2,complejo d1)
{
	return *compl(((d2.re*d1.re)+(d2.im*d2.im))/((d1.re*d1.re)+(d1.im*d1.im)),
				((d1.re*d2.im)-(d2.re*d1.im))/((d1.re*d1.re)+(d1.im*d1.im)));
}

complejo expj(float arg)
{
	return *compl(cos(arg),sin(arg));
}
