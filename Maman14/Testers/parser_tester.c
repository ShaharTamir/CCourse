#include <stdio.h>
#include "../parser.h"

int main()
{
    char valid_name[] = {"m1\0"};
    char invalid_name[] = {"r1_\0"};
    char invalid_name2[] = {"mov\0"};
    char invalid_name3[] = {"mov:\0"};

    printf("valid name res: %d\n", ParserValidateName(valid_name));
    printf("invalid name res: %d\n", ParserValidateName(invalid_name));
    printf("invalid name res: %d\n", ParserValidateName(invalid_name2));
    printf("invalid name res: %d\n", ParserValidateName(invalid_name3));

    return 0;
}