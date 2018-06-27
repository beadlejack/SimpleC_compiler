int foo(void);
int bar(int a, int b);
void car(void);

int main(void) {

	int x, y, z;
	int w[20];
	int *foo;
	char *a, b, c;
	void r[420];			/* line 11: 'r' has type void */
	void *m, *n, o, **p[2]; /* line 12: 'o' has type void */

	foo = &x;

	y = 100;
	y = c;

	w[20] = c;


	z = m; /* line 22: invalid operands to binary = */
	z = *m;	/* line 23: invalid operand to unary * */

	p[1] = n;
	p[0] = &n;
 
 	b = far();
 	b = car();	/* line 29: invalid operands to binary = */
 	b = bar(a, b); /* line 30: invalid arguments to called function */
 	b = bar(b, c);
 	c = bar(y, x);
 	x = bar(y, z);

	return 1;
}