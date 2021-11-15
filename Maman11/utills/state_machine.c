#include <stdlib.h> /* malloc, free */
#include <string.h> /* memset */
#include "state_machine.h"

struct _StateMachineType
{
    int num_states;
    int current_state;
    StateHandler *states;
};

StateMachineType *StateMachineCreate(int starting_state, int num_states)
{
    StateMachineType *m = NULL;

    if(starting_state >= 0 && num_states > 0 && starting_state < num_states)
    {
        m = (StateMachineType*)malloc(sizeof(StateMachineType));
        
        if(m)
        {
            m->current_state = starting_state;
            m->num_states = num_states;
            m->states = (StateHandler*)malloc(sizeof(StateHandler) * num_states);

            if(!m->states)
            {
                StateMachineDestroy(m);
                m = NULL;
            }
        }
    }
    
    return m;
}

void StateMachineDestroy(StateMachineType *machine)
{
    if(machine)
    {
        if(machine->states)
        {
            memset(machine->states, 0, sizeof(StateHandler) * machine->num_states);
            free(machine->states);
            machine->states = NULL;
        }

        free(machine);
        machine = NULL;
    }
}

int StateMachineAddState(StateMachineType *machine, int state_index, StateHandler handler_function)
{
    int retVal = STATE_MACHINE_FAIL; /* assume fail */

    if(machine && handler_function)
    {
        if(state_index < machine->num_states && state_index >= 0)
        {
            machine->states[state_index] = handler_function;
            retVal = STATE_MACHINE_OK;
        }
    }

    return retVal;
}

int StateMachineRun(StateMachineType *machine, StateMachineData* data)
{
    /* data doesn't have to be transferred */
    if(machine)
    {
        while(STATE_MACHINE_END_STATE != machine->current_state)
        {
            machine->current_state = machine->states[machine->current_state](data);
        }

        return STATE_MACHINE_OK;
    }

    return STATE_MACHINE_FAIL;
}