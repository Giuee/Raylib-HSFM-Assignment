#pragma once
#include <memory>
#include "AI_State.h"

class AI_HFSM_Field {
public:
    AI_HFSM_Field();
    ~AI_HFSM_Field() = default;

    void InitializeStates(body& _agent);
    void Update(body& _agent, body& _target);  // Update the state machine

    const AI_State* GetCurrentState() const { return currentAIStatePtr; }  // Get the current state pointer

private:
    AI_State* currentAIStatePtr;  // Pointer to the current AI state object

    // Unique pointers to AI states
    std::unique_ptr<AI_State> seekState;  // Pointer to the seek state
    std::unique_ptr<AI_State> wanderState;  // Pointer to the wander state
    // Add more states as needed

    // Transition to a new state
    // needs protecting as transitions handled in the FSM update function
    void TransitionToState(body& _agent, e_AI_StateID _nextStateID);  // Change to a new state
};