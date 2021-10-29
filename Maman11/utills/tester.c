#include <stdio.h>  /* printf */
#include "tester.h"

void RunTests(TestFunction *tests, StatusType* status, int num_of_tests)
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

void PrepareForTest(char *desc, StatusType* status)
{
    printf("test number %d:\n", ++status->test_num);
    printf("    %s  \n", desc);
}

void CheckResult(int condition, int line_of_failure, StatusType* status)
{
    if(condition)
    {
        printf("Test PASS!!\n\n");
        ++status->num_pass;
    }
    else
    {
        printf("Test FAIL at %d :(\n\n", line_of_failure);
        ++status->num_fail;
    }
}

void PrintSummary(StatusType* status)
{
    printf("\n\n    SUMMARY:\n\n");
    printf("Num of tests pass: %d\\%d\n", status->num_pass, status->test_num);
    printf("Num of tests fail: %d\\%d\n", status->num_fail, status->test_num);
    printf("fin.\n");
}
