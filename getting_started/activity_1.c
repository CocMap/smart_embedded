#include <stdio.h>

#define MAXLENGTH 1000

#define QUESTION 03

int LineIn(char s[]);
void swapxy(int x, int y);
void swapxy_fixed(int *x, int *y);

void main(void) {

#if QUESTION == 01
	int no_char = 0;
	char s[MAXLENGTH];
	printf("Please key in a string of character:\n");
	no_char = LineIn(s);
	printf("The input string is:\n%s\nWith %d characters", s, no_char);
#endif

#if QUESTION == 02

	char name[MAXLENGTH], dob[MAXLENGTH], addr[MAXLENGTH];

	printf("What's your name?\n");
	LineIn(name);
	printf("What's your date of birth?\n");
	LineIn(dob);
	printf("What's your address?\n");
	LineIn(addr);

	printf("Your provided information:\n");
	printf("Name: %s\nDOB: %s\nAddress: %s\n", name, dob, addr);
#endif

#if QUESTION == 03
	printf("Please input your strings:\n");
	
	int char_no = 0;
	char s[MAXLENGTH];
	char_no = LineIn(s);

	printf("The input string is:\n");

	for (int j = char_no-1; j >= 0; j--) {
		putchar(s[j]);
	}
#endif


#if QUESTION == 041
	int x, y;
	printf("Value of x is:\n");
	scanf("%d", &x);
	printf("Value of y is:\n");
	scanf("%d", &y);

	printf("You specified x = %d and y = %d\n", x, y);
	swapxy(x, y);
	printf("Value of x and y after swap: x = %d and y = %d\n", x, y);
#endif

#if QUESTION == 042
	int x, y;
	printf("Value of x is:\n");
	scanf("%d", &x);
	printf("Value of y is:\n");
	scanf("%d", &y);

	printf("You specified x = %d and y = %d\n", x, y);

	int *x_pt = &x;
	int *y_pt = &y;
	swapxy_fixed(x_pt, y_pt);
	//or just swapxy_fixed(&x, &y);
	printf("Value of x and y after swap: x = %d and y = %d\n", x, y);
#endif

#if QUESTION == 05
	struct points {
		double x;
		double y;
		double z;
	} p = {0,0,0};

	struct points * A = &p;
	A->x = 1.2;
	A->y = 2.3;
	A->z = -2.5;

	struct points B;
	B.x = 2.0;
	B.y = -4.9;
	B.z = 10.1;

	printf("Vector AB is: (%.2f, %.2f, %.2f)\n", (B.x - (*A).x), (B.y - (*A).y), (B.z - (*A).z));
	printf("Point p is : (%.2f, %.2f, %.2f)\n", p.x, p.y, p.z);
#endif // QUESTION == 05

	while(1);
}

int LineIn(char s[]) {
	int i = 0;
	int c;
	while ((c = getchar()) != EOF && c != '\n') {
		s[i++] = c;
	}
	if (c == '\n')
		s[i] = '\0';

	return i;
}

void swapxy(int x, int y) {
	int tmp;
	tmp = x;
	x = y;
	y = tmp;
}

void swapxy_fixed(int *x, int *y) {
	int tmp;
	tmp = *x;
	*x = *y;
	*y = tmp;
}
