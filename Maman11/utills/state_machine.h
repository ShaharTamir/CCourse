#ifndef __STATE_MACHINE_H__
#define __STATE_MACHINE_H__

#define STATE_MACHINE_END_STATE -1
#define STATE_MACHINE_FAIL 0
#define STATE_MACHINE_OK 1

/*  ** StateMachineData **
*   The StateMachineData purpose is for the user to pass parametes, values and return values
*   through the state machine. All the parameters are void* so that the user can pass on what ever he likes.
*   One can also pass NULL if there is no data to pass to handlers or to extract from the state machine.
*
*   val - data for handler to work with.
*   params - state machine data / system data / what ever to follow.
*   return_val - is the value to return after the state machine finish running.
*   
*   return value - StateHandler function should return the next state.
*/
typedef struct 
{
    void *val;
    void *params;
    void *return_val;
}StateMachineData;

/**
 * StateHandler is a function type to handle all states.
 * 
 * return value should be the next state to go to.
 */
typedef int (*StateHandler)(StateMachineData*);

/* encapsulated StateMachineType */
typedef struct _StateMachineType StateMachineType;

/** 
 * StateMachineCreate
 * 
 * starting_state:  must be a non-negative number.
 * num_states:      must be a possitive number. 0 or negative will return NULL.
 * 
 * return: if parameters are valid and allocation didn't fail - return pointer to
 *  a new allocated state machine without any states or handlers.
*/
StateMachineType *StateMachineCreate(int starting_state, int num_states);

/** 
 * StateMachineDestroy
 * 
 * free all data allocated for the state machine.
 * 
 * return: NONE.
 */ 
void StateMachineDestroy(StateMachineType *machine);

/** 
 * StateMachineAddState
 * 
 * machine:          the machine to add state to.
 * state_index:      must be a non-valid index. state value to assign the handler to.
 * handler_function: function to handle state as described above.
 * 
 * return: if added successfully - return STATE_MACHINE_OK, else return STATE_MACHINE_FAIL.
 */ 
int StateMachineAddState(StateMachineType *machine, int state_index, StateHandler handler_function);

/**
 * StateMachineRun
 * 
 * machine: the state machine to run
 * data:    user data to pass along the handlers and update as he wish. Not a must -
 *          the state machine can also run without data (data == NULL).
 *          Use at your own risk.
 * 
 * return: if the state machine ended properly - STATE_MACHINE_OK,
 *         else will return STATE_MACHINE_FAIL or stuck in endless loop.
 *         Endless loops are for the user to debug and handle.
 */
int StateMachineRun(StateMachineType *machine, StateMachineData* data);

#endif /* __STATE_MACHINE_H__ */