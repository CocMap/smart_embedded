#include <stdio.h>
#include <time.h>

void Invalid_DateOfBirth() {
    printf("Invalid Date of Birth\n");
}

void Valid_DateOfBirth(){
    printf("Valid Date of Birth\n");
}

int main() {
    int dd, mm, yyyy;
    printf("Insert your date of birth\n");
    scanf("%d %d %d", &dd, &mm, &yyyy);

    //get local time
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    tm = *localtime(&t);

    int current_day = tm.tm_mday;
    int current_mon = tm.tm_mon +1;
    int current_year = tm.tm_year+1900;
    printf("The current date: %d-%d-%d \n", current_day, current_mon, current_year);

    //case 1: over year
    if (yyyy > current_year){
        Invalid_DateOfBirth();
    }

    //case 2: current year (yyyy = current_year)
    if (mm > current_mon || mm > 12 || mm == 0){
        Invalid_DateOfBirth();
    } else {
        if (dd > current_day || dd > 31){
            Invalid_DateOfBirth();
        }
        switch (dd){
            case 31:
                if (mm==2 || mm==4 || mm==6|| mm==9|| mm==11){
                    Invalid_DateOfBirth();
                } break;
            case 30:
                if (mm==2){
                    Invalid_DateOfBirth();
                } break;
            case 29:
                if(mm==2 && yyyy%4!=0){
                    Invalid_DateOfBirth();
                } break;
            default: break;
        }
    }








        //char s[num of strings][string length]
//        char month[12][10] = {"January", "February", "March", "April", "May", "June", "July", "August", "September",
//                              "October", "November", "February"};
//    char month31[7][10];
//    char month30[4][10];
//
//    for (int m = 0; m < 12; m++) {
//        for
//
//    }
//
//    char output_month[10];
//
//    if (dd <= 31 && mm =) {
//        output_month
//    }
//
//
//
//
//
//
//    = { "Jan", "March", "May", "July", "August", "October", "February" };
//    char month30[4][10] = {"April", "June", "September", "November"};
//    char month_Feb[10] = "February";
//

}


