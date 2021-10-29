#include <stdio.h>
#include <stdlib.h>

#include "tester.h"
#include "state_machine.h"

enum ETestMachineStates
{
    START_STATE,
    STATE_A,
    STATE_B,
    STATE_C,
    NUM_STATES
};

/* test state machine */
int StartStateHandler(StateMachineType* , StateMachineData* );
int State_AtoB_Handler(StateMachineType* , StateMachineData* );
int State_BtoC_Handler(StateMachineType* , StateMachineData* );
int State_CtoEnd_Handler(StateMachineType* , StateMachineData* );


/* tester function - # actual tests */
void TestCreateMachine(StatusType *status);
void TestDestroyMachine(StatusType *status);
void TestPackData(StatusType *status);
void TestAddState(StatusType *status);
void TestStateMachineRun(StatusType *status);

TestFunction tests[] = {&TestCreateMachine, &TestDestroyMachine, &TestPackData, &TestAddState, &TestStateMachineRun};

int main()
{
    StatusType s ={0, 0, 0};

    RunTests(tests, &s, sizeof(tests) / sizeof(TestFunction));
    PrintSummary(&s);

    return 0;
}

/*********************
 *    TEST MACHINE   * 
 ********************/

int StartStateHandler(StateMachineType *m, StateMachineData *data)
{
    

    return 0;
}

int State_AtoB_Handler(StateMachineType* m, StateMachineData *data)
{
    return 0;
}

int State_BtoC_Handler(StateMachineType *m, StateMachineData *data)
{
    return 0;
}

int State_CtoEnd_Handler(StateMachineType *m, StateMachineData *data)
{
    return 0;
}

/*********************
 *     ALL TESTS     * 
 ********************/

void TestCreateMachine(StatusType *status)
{
    StateMachineType *m = NULL;

    PrintTestSubject("CREATE MACHINE");

    PrepareForTest("Cerate a machine start state bigger that num states", status);
    m = CreateStateMachine(10, NUM_STATES);
    CheckResult(NULL == m, __LINE__, status);

    PrepareForTest("Create a machine when num states is 0", status);
    m = CreateStateMachine(START_STATE, START_STATE);
    CheckResult(NULL == m, __LINE__, status);

    PrepareForTest("Create a machine when num states is smaller than start state", status);
    m = CreateStateMachine(STATE_C, STATE_A);
    CheckResult(NULL == m, __LINE__, status);

    PrepareForTest("Create a machine successfully, check not NULL", status);
    m = CreateStateMachine(START_STATE, NUM_STATES);
    CheckResult(NULL != m && NUM_STATES == m->num_states && START_STATE == m->current_state, __LINE__, status);

    PrepareForTest("Verify num states", status);
    CheckResult(NUM_STATES == m->num_states, __LINE__, status);
    
    PrepareForTest("Verify current state at creation", status);
    CheckResult(START_STATE == m->current_state, __LINE__, status);

    PrepareForTest("Verify states array is not NULL", status);
    CheckResult(NULL != m->states, __LINE__, status);
    
    DestroyStateMachine(m);
}

void TestDestroyMachine(StatusType *status)
{
    StateMachineType *m = NULL;

    PrintTestSubject("DESTSROY MACHINE");
    PrepareForTest("Create and destroy machine Verify states array is NULL", status);
    m = CreateStateMachine(START_STATE, NUM_STATES);
    
    if(m && m->states)
    {
        DestroyStateMachine(m);
        CheckResult(NULL == m->states, __LINE__, status);
    }
    else
    {
        CheckResult(TEST_FAIL, __LINE__, status);
    }
}

void TestPackData(StatusType *status)
{
    StateMachineData* d = NULL;
    char val = '{';
    int params = 6;
    float ret_val = 1.5f;

    PrintTestSubject("DATA PACKAGE");

    PrepareForTest("Create a package", status);
    d = CreateStateMachineData(&val, &params, &ret_val);
    CheckResult(NULL != d, __LINE__, status);

    PrepareForTest("Verify package data", status);
    CheckResult(*(char*)d->val == val && *(int*)d->params == params &&
                 *(float*)d->return_val == ret_val, __LINE__, status);

    PrepareForTest("Destroy package and verify NULL", status);
    DestroyStateMachineData(d);
    CheckResult(NULL == d->val, __LINE__, status);
}

void TestAddState(StatusType *status)
{
    StateMachineType *m = NULL;

    PrintTestSubject("ADD STATES");
    PrepareForTest("Add valid state and check return val", status);
    m = CreateStateMachine(START_STATE, NUM_STATES);

    if(m)
    {
         /*AddStateMachine(m, START_STATE, );*/
    }
    else
    {
        CheckResult(TEST_FAIL, __LINE__, status);
    }
    
}

void TestStateMachineRun(StatusType *status)
{

}