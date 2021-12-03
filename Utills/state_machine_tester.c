#include <stdio.h>
#include <stdlib.h>

#include "tester.h"
#include "state_machine.h"

#define TEST_MACHINE_NUM_CYCLES 3

enum ETestMachineStates
{
    START_STATE,
    STATE_A,
    STATE_B,
    STATE_C,
    NUM_STATES
};

struct _StateMachineType
{
    int num_states;
    int current_state;
    StateHandler *states;
};

/* test state machine */
int StartStateHandler(StateMachineData*);
int State_A_Handler(StateMachineData*);
int State_B_Handler(StateMachineData*);
int State_C_Handler(StateMachineData*);

/* tester function - # actual tests */
void TestCreateMachine(TestStatusType *status);
void TestDestroyMachine(TestStatusType *status);
void TestAddState(TestStatusType *status);
void TestStateMachineRun(TestStatusType *status);

TestFunction tests[] = {TestCreateMachine, TestDestroyMachine, TestAddState, TestStateMachineRun};

int main()
{
    TestStatusType s ={0, 0, 0};

    RunTests(tests, &s, sizeof(tests) / sizeof(TestFunction));
    PrintSummary(&s);

    return 0;
}

/*********************
 *    TEST MACHINE   * 
 ********************/

int StartStateHandler(StateMachineData *data)
{
    if(*(char*)data->val == '}')
    {
        *(ETestStatus*)data->return_val = TEST_PASS;
        return STATE_B;
    }
    if(*(char*)data->val == '{')
    {
        return STATE_A;
    }

    *(ETestStatus*)data->return_val = TEST_FAIL;
    return STATE_MACHINE_END_STATE;
}

int State_A_Handler(StateMachineData *data)
{
    *(char*)data->val = 'A';

    printf("Currently in state A\n");
    ++*(int *)data->params;

    return STATE_B;
}

int State_B_Handler(StateMachineData *data)
{
    printf("Currently in state B\n");
    if(*(char *)data->val == 'A')
    {
        return STATE_C;
    }
    else if(*(char *)data->val == 'C')
    {
        return STATE_A;
    }

    return STATE_MACHINE_END_STATE;
}

int State_C_Handler(StateMachineData *data)
{
    *(char *)data->val = 'C';
    printf("Currently in state C\n");
    if(*(int*)data->params == TEST_MACHINE_NUM_CYCLES)
    {
        *(ETestStatus*)data->return_val = TEST_PASS;
        return STATE_MACHINE_END_STATE;
    }

    return STATE_B;
}

/*********************
 *     ALL TESTS     * 
 ********************/

void TestCreateMachine(TestStatusType *status)
{
    StateMachineType *m = NULL;

    PrintTestSubject("CREATE MACHINE");

    PrepareForTest("Cerate a machine start state bigger that num states", status);
    m = StateMachineCreate(10, NUM_STATES);
    CheckResult(NULL == m, __LINE__, status);

    PrepareForTest("Create a machine when num states is 0", status);
    m = StateMachineCreate(START_STATE, START_STATE);
    CheckResult(NULL == m, __LINE__, status);

    PrepareForTest("Create a machine when num states is smaller than start state", status);
    m = StateMachineCreate(STATE_C, STATE_A);
    CheckResult(NULL == m, __LINE__, status);

    PrepareForTest("Create a machine successfully, check not NULL", status);
    m = StateMachineCreate(START_STATE, NUM_STATES);
    CheckResult(NULL != m && NUM_STATES == m->num_states && START_STATE == m->current_state, __LINE__, status);

    PrepareForTest("Verify num states", status);
    CheckResult(NUM_STATES == m->num_states, __LINE__, status);
    
    PrepareForTest("Verify current state at creation", status);
    CheckResult(START_STATE == m->current_state, __LINE__, status);

    PrepareForTest("Verify states array is not NULL", status);
    CheckResult(NULL != m->states, __LINE__, status);
    
    StateMachineDestroy(m);
}

void TestDestroyMachine(TestStatusType *status)
{
    StateMachineType *m = NULL;

    PrintTestSubject("DESTSROY MACHINE");
    PrepareForTest("Create and destroy machine Verify states array is NULL", status);
    m = StateMachineCreate(START_STATE, NUM_STATES);
    
    if(m && m->states)
    {
        StateMachineDestroy(m);
        CheckResult(NULL == m->states, __LINE__, status); /* this will raise a valgrind err */
    }
    else
    {
        CheckResult(TEST_FAIL, __LINE__, status);
    }
}

void TestAddState(TestStatusType *status)
{
    StateMachineType *m = NULL;
    StateMachineData d = {NULL, NULL, NULL};
    char val = '}';
    ETestStatus ret_val = TEST_FAIL;

    PrintTestSubject("ADD STATES");
    
    PrepareForTest("Add valid state and check return val", status);
    m = StateMachineCreate(START_STATE, NUM_STATES);
    
    if(m)
    {
        CheckResult(STATE_MACHINE_OK == StateMachineAddState(m, START_STATE, StartStateHandler), __LINE__, status);
    }
    else
    {
        CheckResult(TEST_FAIL, __LINE__, status);
    }

    PrepareForTest("Run the state handler and check state returned", status);
    d.val = &val;
    d.return_val = &ret_val;
    CheckResult(STATE_B == m->states[START_STATE](&d), __LINE__, status);
    
    PrepareForTest("Verify handler data return value", status);
    CheckResult(TEST_PASS == ret_val, __LINE__, status);

    PrepareForTest("Change val and verify handler returns state end", status);
    val = '0';
    CheckResult(STATE_MACHINE_END_STATE == m->states[START_STATE](&d), __LINE__, status);

    PrepareForTest("Verify handler data return value", status);
    CheckResult(TEST_FAIL == ret_val, __LINE__, status);
    
    StateMachineDestroy(m);
}

void TestStateMachineRun(TestStatusType *status)
{
    StateMachineType *m = NULL;
    StateMachineData d = {NULL, NULL, NULL};
    char val = '{';
    int cycles = 0;
    ETestStatus ret_val = TEST_FAIL;

    PrintTestSubject("FULL MACHINE RUN");
    PrepareForTest("Test full machine run status is success", status);
    m = StateMachineCreate(START_STATE, NUM_STATES);
    d.val = &val;
    d.params = &cycles;
    d.return_val = &ret_val;

    StateMachineAddState(m, START_STATE, StartStateHandler);
    StateMachineAddState(m, STATE_A, State_A_Handler);
    StateMachineAddState(m, STATE_B, State_B_Handler);
    StateMachineAddState(m, STATE_C, State_C_Handler);

    CheckResult(STATE_MACHINE_OK == StateMachineRun(m, &d), __LINE__, status);

    PrepareForTest("Verify handler data return value", status);
    CheckResult(TEST_PASS == ret_val, __LINE__, status);

    PrepareForTest("Verify handler data params", status);
    CheckResult(TEST_MACHINE_NUM_CYCLES == cycles, __LINE__, status);
    
    StateMachineDestroy(m);
}
