#pragma once
#include <memory>
#include "TB_State.h"

class TB_HFSM_Combat
{
public:
    TB_HFSM_Combat();
    ~TB_HFSM_Combat() = default;
    void InitializeStates(body& _agent);
    void Update(body& _agent, body& _target);  //Update the state machine

    const TB_State* GetCurrentState() const { return currentTBStatePtr; }  //Get the current state pointer
private:
    TB_State* currentTBStatePtr;  //Pointer to the current TB state object

    // Unique pointers to TB states
    std::unique_ptr<TB_State> playerTurn;  //Pointer to the Player Turn state
	std::unique_ptr<TB_State> enemyTurn;   //Pointer to the Enemy Turn state
	std::unique_ptr<TB_State> winState;     //Pointer to the Win state
	std::unique_ptr<TB_State> loseState;    //Pointer to the Lose state

    //Add more states as needed

    //Transition to a new state
    //needs protecting as transitions handled in the FSM update function
    void TransitionToState(body& _agent, e_TB_StateID _nextStateID);  //Change to a new state
};