#include <stdlib.h> /* malloc, free */
#include <string.h> /* memset */
#include "state_machine.h"

StateMachineType *CreateStateMachine(int starting_state, int num_states)
{
    StateMachineType *m = NULL;

    if(starting_state < 0 || num_states <= 0 || starting_state >= num_states)
    {
        return m;
    }

    m = (StateMachineType*)malloc(sizeof(StateMachineType));
    
    if(m)
    {
        m->current_state = starting_state;
        m->num_states = num_states;
        m->states = (StateHandler*)malloc(sizeof(StateHandler) * num_states);

        if(!m->states)
        {
            DestroyStateMachine(m);
            m = NULL;
        }
    }

    return m;
}

void DestroyStateMachine(StateMachineType *machine)
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

int AddStateMachine(StateMachineType *machine, int state_index, StateHandler handlerFunction)
{
    int retVal = STATE_MACHINE_FAIL; /* assume fail */

    if(machine && handlerFunction)
    {
        if(state_index < machine->num_states && state_index >= 0)
        {
            machine->states[state_index] = handlerFunction;
            retVal = STATE_MACHINE_OK;
        }
    }

    return retVal;
}

int RunStateMachine(StateMachineType *machine, StateMachineData* data)
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