
#pragma once
#include "TB_State.h"

class TB_EnemyTurn_State : public TB_State
{
public:
	TB_EnemyTurn_State();
	~TB_EnemyTurn_State() override = default;

	void OnEnter(body& agent) override;
	e_TB_StateID OnUpdate(body& agent, body& target, float dt) override;
	void OnExit(body& agent) override;

private:
	float turnTimer = 0.0f; //short delay so the enemy's attack doesn't happen instantly
};