#pragma once
#include "Body.h"

//FSM//
enum class e_AI_StateID
{
	Idle,
	Wander,
	Seek,
	Attack
};

//AI_State is the template every state follows//
class AI_State
{
public:
	//every state gives its ID and name when it's made//
	AI_State(e_AI_StateID id, const char* name) : stateName(name), stateID(id) {} //the part after the colon sets stateName and stateID before the constructor body runs
	//virtual destructor, so deleting a state through an AI_State pointer cleans up the real state, default means use the compiler's normal version, as such I do not need to write our own
	virtual ~AI_State() = default;

	//body& everywhere so these match WanderState exactly also just kind a copied from josh's AI_State.h lol 
	virtual void OnEnter(body& agent) = 0; // Called when entering the state
	virtual e_AI_StateID OnUpdate(body& agent, body& target, float dt) = 0; // Called every frame while in the state
	virtual void OnExit(body& agent) = 0; // Called when exiting the state

	// Getters/Setters
	const char* GetStateName() const { return stateName; } // Get the state name for debugging
	e_AI_StateID GetStateID() const { return stateID; }// Get the state ID

protected:
	const char* stateName; // Name of the state for debugging
	const e_AI_StateID stateID;// Unique ID for the state
};