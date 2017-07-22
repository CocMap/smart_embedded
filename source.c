#include <stdio.h>
#include <string.h>
#include <time.h>

//int CharIn(char s[], int n);
void Calculator(float x, float y, char op);     //calculator function


void main() {
    int question;
    printf("Please choose your question:\n");
    scanf("%d", &question);

    //function - Calculator
    if (question == 1) {
        float x, y;
        char op;

        printf("Please input your equation following these formats: <element 1> <enter> <operator> <enter> <element 2>\n");
        scanf("%f %c %f", &x, &op, &y);
        Calculator(x,y,op);
    }

    if (question == 0){
        time_t t = time(NULL);
        struct tm tm = *localtime(&t);

        printf("now: %d-%d-%d %d:%d:%d\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
    }

    //function -
    if (question == 2){
        struct StudentInfo {
            char name[50];
            int dd, mm, yy;
            int id[50];

        };

        char test_name;

        printf("What is your student's name?\n");
        scanf("%s", )

}

    if (question == 0){
        char month31[7][10] = {"Jan", "March", "May", "July", "August", "October", "February"};
        printf("%s", month31[1]);
    }





}

//function - Calculator
void Calculator(float x, float y, char op){
    float output = 0;
    switch (op) {
        case '+': output = x + y; break;
        case '-': output = x - y; break;
        case '*': output = x * y; break;
        case '/': output = x / y; break;
        default: printf("Your equation is invalid \n"); break;
    }

    printf("The answer is: %f \n", output);
}


void DateOfBirth(int dd, int mm, int yyyy){
    //get local time
    time_t local_time = time(NULL);
    struct tm;
    tm = *localtime(&local_time);






    //condition of dd
    if (dd > 31){
        printf("Invalid date\n");
    }

    //condition of mm
    if (mm > 12 || mm = 0){
        printf("Invalid month\n");
    }

    //condition of yyyy
    if (yyyy > )
    //char s[num of strings][string length]
    char month[12][10] = {"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November","February"};
    char month31[7][10];
    char month30[4][10];

    for (int m = 0; m < 12; m++){
        for

    }

    char output_month[10];

    if (dd <= 31 && mm =  ){
        output_month
    }






     = {"Jan", "March", "May", "July", "August", "October", "February"};
    char month30[4][10] = {"April", "June", "September", "November"};
    char month_Feb[10] = "February";


//change mm to month






    switch(dd) {
        case 1: output_month = 'January';
        case 2: output_month = 'February';
        case 3: output_month = 'March';
        case : output_month =
        case 2: output_month =
        case 2: output_month =
        case 2: output_month =
        case 2: output_month =
        case 2: output_month =
        case 2: output_month =
        case 2: output_month =
        case 2: output_month =
        case 2: output_month =

    }

    for (int i = 0; i <= 31; i++){
        if (dd <= 30) {
            for (int j = 0; j < 7; j++){
                switch()
            }

        }
    }




}


