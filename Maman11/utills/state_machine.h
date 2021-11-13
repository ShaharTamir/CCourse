#ifndef __STATE_MACHINE_H__
#define __STATE_MACHINE_H__

#define STATE_MACHINE_END_STATE -1
#define STATE_MACHINE_FAIL 0
#define STATE_MACHINE_OK 1

typedef struct 
{
    void *val;
    void *params;
    void *return_val;
}StateMachineData;

/*  ** StateMachineData **
*   val - data for handler to work with.
*   params - state machine data / system data / what ever to follow
*   return_val - is the value to return after the state machine finish running.
*   
*   return value - StateHandler function should return the next state.
*/
typedef int (*StateHandler)(StateMachineData*);

typedef struct
{
    int num_states;
    int current_state;
    StateHandler *states;
} StateMachineType;

StateMachineType *CreateStateMachine(int starting_state, int num_states);

void DestroyStateMachine(StateMachineType *machine);

int AddStateMachine(StateMachineType *machine, int state_index, StateHandler handlerFunction);

int RunStateMachine(StateMachineType *machine, StateMachineData* data);

#endif /* __STATE_MACHINE_H__ */