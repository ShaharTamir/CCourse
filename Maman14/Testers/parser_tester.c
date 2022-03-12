#include <stdio.h>
#include "../parser.h"

static void TestValidName();
static void TestIsLabel();

int main()
{
    TestValidName();
    TestIsLabel();
    return 0;
}


void TestValidName()
{
    char valid_name[] = {"m1\0"};
    char invalid_name[] = {"r1_\0"};
    char invalid_name2[] = {"mov\0"};
    char invalid_name3[] = {"mov:\0"};

    printf("valid name res: %d\n", ParserValidateName(valid_name));
    printf("invalid name res: %d\n", ParserValidateName(invalid_name));
    printf("invalid name res: %d\n", ParserValidateName(invalid_name2));
    printf("invalid name res: %d\n", ParserValidateName(invalid_name3));

}

void TestIsLabel()
{
    char label[] = {"X:\0"};
    char label2[] = {"somthing123:\0"};
    char not_label[] = {"mov\0"};
    char not_label2[] = {"r16\0"};

    printf("valid lbl res: %d\n", ParserIsNewLabel(label));
    printf("valid lbl res: %d\n", ParserIsNewLabel(label2));
    printf("invalid lbl res: %d\n", ParserIsNewLabel(not_label));
    printf("invalid lbl res: %d\n", ParserIsNewLabel(not_label2));
}