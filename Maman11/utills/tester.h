#ifndef __TESTER_H__
#define __TESTER_H__

typedef enum
{
    TEST_FAIL,
    TEST_PASS
} ETestStatus;

typedef struct
{
    int test_num;
    int num_pass;
    int num_fail;
} TestStatusType;

typedef void (*TestFunction)(TestStatusType*);

void RunTests(TestFunction *tests, TestStatusType* status, int num_of_tests);

void PrintTestSubject(char *test_subject);

void PrepareForTest(char *test_description, TestStatusType* status);

void CheckResult(int condition, int line_of_failure, TestStatusType* status);

void PrintSummary(TestStatusType* status);

#endif /* __TESTER_H__ */

