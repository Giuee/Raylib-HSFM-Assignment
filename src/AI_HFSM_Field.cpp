#include "AI_HFSM_Field.h"
#include "AI_Seek_State.h"
#include "AI_Wander_State.h"
#include <iostream>
AI_HFSM_Field::AI_HFSM_Field()
{
    // Initialize the AI state machine to the idle state
}

void AI_HFSM_Field::InitializeStates(body& _agent)
{
    // Initialize the states for the AI state machine using polymorphic pointers
    // Seek State
    seekState = std::make_unique<AI_Seek_State>();
    // Wander State
    wanderState = std::make_unique<AI_Wander_State>();

    // Add new states to the state machine as needed

    // Set the initial state to seek
    currentAIStatePtr = wanderState.get();  // Set the current state pointer to the seek state
    currentAIStatePtr->OnEnter(_agent);  // Call the OnEnter method of the initial state
    std::cout << "Initial state: " << currentAIStatePtr->GetStateName() << std::endl;
}

void AI_HFSM_Field::Update(body& _agent, body& _target)
{
    // Update logic based on the current state
    if (currentAIStatePtr == nullptr)
    {
        return; // If no current state, do nothing
    }
    // The update will return the next state ID based on the current state logic
	float dt = GetFrameTime();
    e_AI_StateID nextStateID = currentAIStatePtr->OnUpdate(_agent, _target, dt);  // Call OnUpdate for the current state

    if (nextStateID != e_AI_StateID::Idle)
    {
        TransitionToState(_agent, nextStateID);
    }
}

void AI_HFSM_Field::TransitionToState(body& _agent, e_AI_StateID _nextStateID)
{
    if (currentAIStatePtr != nullptr && _nextStateID == currentAIStatePtr->GetStateID())
    {
        std::cout << "Already in the desired state, no transition needed." << std::endl;
        return; // No transition needed if already in the desired state
    }

    if (currentAIStatePtr != nullptr) 
    {
        currentAIStatePtr->OnExit(_agent);  // Call OnExit for the current state
    }

    AI_State* newStatePtr = nullptr;
    switch (_nextStateID) 
    {
    case e_AI_StateID::Idle:
        // No state change, do nothing
        break;
    case e_AI_StateID::Seek:
        newStatePtr = seekState.get();
        break;
    case e_AI_StateID::Wander:
        newStatePtr = wanderState.get();
        break;
        // Add more cases for additional states as needed
    default:
        break; // Handle other states as needed
    }

    // Change to the new state
    currentAIStatePtr = newStatePtr;
    std::cout << "Transitioning to: " << currentAIStatePtr->GetStateName() << std::endl;
    currentAIStatePtr->OnEnter(_agent);  // Call the OnEnter method of the new state


}