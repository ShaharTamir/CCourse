#include <stdio.h>  /* printf */
#include "../colors.h"
#include "tester.h"

void RunTests(TestFunction *tests, TestStatusType* status, int num_of_tests)
{
    int i = 0;

    for(i = 0; i < num_of_tests; ++i)
    {
        tests[i](status);
    }
}

void PrintTestSubject(char *subject)
{
    printf("*******************************************\n");
    printf("            %s\n", subject);
    printf("*******************************************\n\n");
}

void PrepareForTest(char *desc, TestStatusType* status)
{
    printf("test number %d:\n", ++status->test_num);
    printf("    %s  \n", desc);
}

void CheckResult(int condition, int line_of_failure, TestStatusType* status)
{
    if(condition)
    {
        printf("Test %s%s PASS!! %s\n\n", CLR_BOLD, CLR_GRN, CLR_RESET);
        ++status->num_pass;
    }
    else
    {
        printf("Test %s%s FAIL %s at %d :(\n\n", CLR_BOLD, CLR_RED, CLR_RESET, line_of_failure);
        ++status->num_fail;
    }
}

void PrintSummary(TestStatusType* status)
{
    printf("\n\n   %s%s SUMMARY: %s\n\n", CLR_BOLD, CLR_YEL, CLR_RESET);
    printf("Num of tests pass: %s%s %d\\%d %s\n", CLR_BOLD, CLR_GRN, status->num_pass, status->test_num, CLR_RESET);
    printf("Num of tests fail: %s%s %d\\%d %s\n", CLR_BOLD, CLR_RED, status->num_fail, status->test_num, CLR_RESET);
    printf("fin.\n");
}

