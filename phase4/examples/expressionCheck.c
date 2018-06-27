void func1(void);

int main(void){
	int x;
	char y;
	void *z;
	char *b;
	int *i;
	int k;
	char a[10];
	void *a2[10];

	x||y;/*no error*/
	y||z;/*no error*/
	i||func1&&k;/*error, func1 not predicate type, && should report out*/
	a||a2;/*no error*/
	
	a&&a2;/*no error*/
	i&&func1;/*error*/

	x==z;/*error*/
	y!=x;
	x==func1;/*error*/
	z!=func1;/*error*/	

	x>=y;
	a>=x;/*error*/
	a>=a2;/*error*/
	i<=x;/*error*/
	
	x+y;
	y-x;
	i+x;
	x+i;
	i-b;/*error*/
	z+i;/*error*/
	i+z;/*error*/
	b-a;

	k*x;
	x/y;
	x*z;/*error*/

	*z;	
	*x;/*error*/
	&x;
	&4;/*error*/
	!x;
	!z;
	!func1;/*error*/
	-x;
	-y;/*error*/	

	sizeof x;
	sizeof y;
	sizeof func1;/*error*/

	x[10]/*error*/;
	i[10];
	z[9];/*error*/
}
