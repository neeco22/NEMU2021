#include "FLOAT.h"

FLOAT F_mul_F(FLOAT a, FLOAT b) {
	return ((long long)a * b) / val;
}

FLOAT F_div_F(FLOAT a, FLOAT b) {
	long long dividend = (long long)a * val;
	int quotient,remainder;
	asm volatile("idivl %2"
		: "=a"(quotient), "=d"(remainder)
		: "r"(b),
		"a"((int)dividend), "d"((int)(dividend >> 32)));
	return quotient;
}

FLOAT f2F(float a) {
	unsigned int bits=*(unsigned int *)&a;
	int sign=bits >> 31;
	int exp=(bits >> 23) & 0xff;
	int frac=bits & 0x7fffff | 0x800000;
	int result;
	if(exp==0){
		return 0;
	}
	if(exp-134>=0){
		result=frac<<(exp-134);
	}
	else{
		result=frac>>(134-exp);
	}
	return sign? -result:result;
}

FLOAT Fabs(FLOAT a) {
	return a < 0 ? -a : a;
}


/* Functions below are already implemented */

FLOAT sqrt(FLOAT x) {
	FLOAT dt, t = int2F(2);

	do {
		dt = F_div_int((F_div_F(x, t) - t), 2);
		t += dt;
	} while(Fabs(dt) > f2F(1e-4));

	return t;
}

FLOAT pow(FLOAT x, FLOAT y) {
	/* we only compute x^0.333 */
	FLOAT t2, dt, t = int2F(2);

	do {
		t2 = F_mul_F(t, t);
		dt = (F_div_F(x, t2) - t) / 3;
		t += dt;
	} while(Fabs(dt) > f2F(1e-4));

	return t;
}

