#pragma once
#include "Body.h"

//FSM//
enum class e_TB_StateID
{
	PlayerTurn,
	EnemyTurn,
	Win,
	Lose
};

//TB_State is the template every TB state follows//
class TB_State
{
public:
	TB_State(e_TB_StateID id, const char* name) : stateName(name), stateID(id) {}
	virtual ~TB_State() = default;

	virtual void OnEnter(body& agent) = 0;
	virtual e_TB_StateID OnUpdate(body& agent, body& target, float dt) = 0;
	virtual void OnExit(body& agent) = 0;

	const char* GetStateName() const { return stateName; }
	e_TB_StateID GetStateID() const { return stateID; }

protected:
	const char* stateName;
	const e_TB_StateID stateID;
};