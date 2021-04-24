struct complejo
{
	float re;
	float im;
};

#define PI 3.14159265358979323846

complejo *compl(float real, float imag);
complejo operator +(complejo s1,complejo s2);
complejo operator -(complejo s2,complejo s1);
complejo operator *(complejo f1,complejo f2);
complejo operator /(complejo d2,complejo d1);
complejo expj(float arg);
