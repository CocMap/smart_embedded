#include <stdio.h>

/*choose one of these PARTS below:
- PART 1: Hello World, printf(), puts()
- PART 2: datatype, operator
- PART 3: input from user using scanf()
- PART 4: ternary
- PART 5: switch()
- PART 6: while() loop
- PART 7: function
- PART 8: array
- PART 9: passing array to a function
- PART 10: pointers
*/

#define PART 1

/*
CALL function: void function_name(datatype1 arg1, datatype2 arg2,...)
DECLARE function: void function_name(datatype1 arg1, datatype2 arg2,...) {
//code
}
*/
int Function1(int a, int b) {
	int c = 20;
	int total = a*b + c;
	return total;
}

int Function2(int MyArray[], int ArraySize) {
	int sum = 0;
	for (int i = 0; i < ArraySize; i++) {
		sum += MyArray[i];		//sum = sum + MyArray[i]
	}

	return sum;
}

//main function
int main()
{
	if (PART == 1) {
		puts("*****PART 1 - Hello World, printf(), puts() \n\n");

		//printf()
		printf("Hello World Coc Map: \n");

		printf("%d \n", 32);			//integer
		printf("%ld \n", 1234678);		//large integer
		printf("%f \n", 32.12);			//float
		printf("%lf \n", 32.12132);		//large float
		printf("%c \n", 'c');			//character
		printf("%s \n", "Hello World!");//string
		printf("%x \n", 16);			//Hex

										//puts()
		puts("This is my c program");		//puts() function = print out screeen
	}

	if (PART == 2) {
		puts("*****PART 2 - datatype + operator \n\n");

		int a = 16, b = 2, c = 7;
		float pi = 3.14;					//know more in part 1
		char str1 = "Coc Map", str2 = "c";	//can be string or character

											//operator: (+), (-), (*), (/ division), (% modulus - take remainder)
		int sum_AB = a + b;
		float sum_Api = a + pi;

		printf("a = %d and b = %d \n", a, b);
		printf("Sum a + b = %d \n", sum_AB);
		printf("Sum a + pi = %f \n", sum_Api);

		printf("Sum a + b + pi = %f \n", a + b + pi);

		//different between (/) and (%) operator
		printf("Division a_b: %d \n", a / b);
		printf("Modulus a_b: %d \nModulus a_c: %d \n", a%b, a%c);		//NOTE: datatype of DIVISION and MODULUS MUST BE %d
	}

	if (PART == 3) {
		puts("*****PART 3 - input from user using scanf() \n\n");

		int age;
		printf("Please enter your age \n");
		scanf("%d", &age);						//MUST go with printf, it scan the input from printf

		if (age > 18) {
			printf("You are allowed to drink Alcohol \n");
		}
		else if (age == 18) {
			printf("You may be allowed to drink Alcohol \n");
		}
		else {
			printf("You are NOT allowed to drink Alcohol \n");
		}
	}




	if (PART == 4) {
		puts("*****PART 4 - The ternary (conditional) operator in C \n\n");


		int a = 21, b = 20;
		int c;

		// c = (logical expression) ? (true) : (false)
		c = (a > b) ? b : a;
		printf("Ans = %d", c);			//ans = 20
	}


	if (PART == 5) {
		puts("*****PART 5 - switch() \n\n");

		int score;
		printf("Input your score in Number (from 50 or 60 or 70 or 80) \n");
		scanf("%d", &score);

		switch (score) {
		case 50:
			printf("PA - pass");
			break;							//if not break here it will run the left (case 60, 70, 80)
		case 60:
			printf("CR - credit");
			break;
		case 70:
			printf("DI - distinction");
			break;
		case 80:
			printf("HD - high distinction");
			break;
		default:
			printf("You fail the course N/N");
		}
	}


	if (PART == 6) {
		puts("*****PART 6 - while() loop \n\n");		//a loop run until reach the condition


		int i = 0;
		while (i <= 5) {
			printf("Value of i = %d\n", i);
			i++;
		}
	}

	if (PART == 7) {
		puts("*****PART 7 - function in C \n\n");			//Function1(int i)


		int prod;
		prod = Function1(2, 4);
		printf("The value of prod is: %d", prod);
	}



	if (PART == 8) {
		puts("*****PART 8 - array \n\n");

		//ARRAY - datatype name[size]
		int myNumberArray[10] = { 1,3,5,2,1 };				//the rest has 0 value
		char myStringArray[10] = { 'a', 'b', 'c' };			//the rest has \0 value

		for (int i = 0; i < 10; i++) {
			printf("Number array with element [%d] is %d \n", i, myNumberArray[i]);
			printf("String array with element [%d] is %c \n", i, myStringArray[i]);

		}

		//MULTI-DIMENTIONAL ARRAY - datatype name[row][col]
		int myMultiArray[2][3] = {
			{ 1,2,4 },	//row 0
			{ 5,8,9 }		//row 1
		};

		//read carefully how to call element in array
		printf("Multi array row 2 col 1 is: %d \n", myMultiArray[1][0]);		//ans = 5
		for (int row = 0; row < 2; row++) {
			for (int col = 0; col < 3; col++) {
				printf("The array wiht the postion [%d]{%d] is %d \n", row + 1, col + 1, myMultiArray[row][col]);
			}
		}
	}

	if (PART == 9) {
		puts("*****PART 9 - passing array using function \n\n");

		int numArray[4] = { 1,1,1,1 };
		int sum_of_array = Function2(numArray, 4);

		printf("Array sum is %d", sum_of_array);
	}


	if (PART == 10) {
		puts("*****PART 10 - pointers in C \n\n");

		//pointer = address of the variable (hex value)
		//pointer is used to access to the address of the variable

		int val = 30;
		int *pointer_p;
		pointer_p = &val;

		print("pointer_p is %x \n", pointer_p);
		printf("Address of the val = %x", &val);

	}


	//getchar() will wait for keyboard input from closing console window too fast - used for debug
	getchar(); //press Ctrl + C to exit 
	return 0;
}

