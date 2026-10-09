#include "TB_HFSM_Combat.h"
#include "TB_PlayerTurn_State.h"
#include "TB_EnemyTurn_State.h"
#include "TB_Win_State.h"
#include "TB_Lose_State.h"
#include <iostream>
TB_HFSM_Combat::TB_HFSM_Combat() //constructor for the combat state machine
{
	currentTBStatePtr = nullptr; //Initialize the current state pointer to nullptr
}
//creates all four combat states//
void TB_HFSM_Combat::InitializeStates(body& _agent)
{
    //Initialize the states for the TB state machine using polymorphic pointers
    //Player Turn State
    playerTurn = std::make_unique<TB_PlayerTurn_State>();
	enemyTurn = std::make_unique<TB_EnemyTurn_State>();
	winState = std::make_unique<TB_Win_State>();
	loseState = std::make_unique<TB_Lose_State>();
    //Add more states as needed


    //Add new states to the state machine as needed

    //Set the initial state 
    currentTBStatePtr = playerTurn.get();  //Set the current state pointer 
    currentTBStatePtr->OnEnter(_agent);  //Call the OnEnter method of the initial state
    std::cout << "Initial state: " << currentTBStatePtr->GetStateName() << std::endl;
}
//updates the current state and transitions to the next state if needed//
void TB_HFSM_Combat::Update(body& _agent, body& _target)
{
    //Update logic based on the current state
    if (currentTBStatePtr == nullptr)
    {
        return; //If no current state, do nothing
    }
    //The update will return the next state ID based on the current state logic
    float dt = GetFrameTime();
    e_TB_StateID nextStateID = currentTBStatePtr->OnUpdate(_agent, _target, dt);  //Call OnUpdate for the current state

    if (nextStateID != currentTBStatePtr->GetStateID()) //only switch state when asked for a different one
    {
        TransitionToState(_agent, nextStateID);
    }
}
//exits the current state, finds the requested state and enters it//
void TB_HFSM_Combat::TransitionToState(body& _agent, e_TB_StateID _nextStateID)
{
    if (currentTBStatePtr != nullptr && _nextStateID == currentTBStatePtr->GetStateID())
    {
        std::cout << "Already in the desired state, no transition needed." << std::endl;
        return; //No transition needed if already in the desired state
    }

    if (currentTBStatePtr != nullptr)
    {
        currentTBStatePtr->OnExit(_agent);  //Call OnExit for the current state
    }

    TB_State* newStatePtr = nullptr; //picks new state first so any unhandled ID cannot null 
    switch (_nextStateID)
    {
    case e_TB_StateID::PlayerTurn:
        newStatePtr = playerTurn.get();
        break;
    case e_TB_StateID::EnemyTurn:
        newStatePtr = enemyTurn.get();
		break;
    case e_TB_StateID::Win:
        newStatePtr = winState.get();
		break;
    case e_TB_StateID::Lose:
		newStatePtr = loseState.get();
		break;
    //Add more cases for additional states as needed
    
        //Add more cases for additional states as needed
    default:
        return; // Handle other states as needed
    }

    if (currentTBStatePtr != nullptr)
    {
        currentTBStatePtr->OnExit(_agent);  //Cleans up the old state
    }

    // Change to the new state
    currentTBStatePtr = newStatePtr;
    std::cout << "Transitioning to: " << currentTBStatePtr->GetStateName() << std::endl;
    currentTBStatePtr->OnEnter(_agent);  //Call the OnEnter method of the new state


}