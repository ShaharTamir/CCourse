#ifndef __TESTER_H__
#define __TESTER_H__

typedef enum
{
    TEST_FAIL,
    TEST_PASS
} EStatus;

typedef struct
{
    int test_num;
    int num_pass;
    int num_fail;
} StatusType;

typedef void (*TestFunction)(StatusType*);

void RunTests(TestFunction *tests, StatusType* status, int num_of_tests);

void PrintTestSubject(char *test_subject);

void PrepareForTest(char *test_description, StatusType* status);

void CheckResult(int condition, int line_of_failure, StatusType* status);

void PrintSummary(StatusType* status);

#endif /* __TESTER_H__ */