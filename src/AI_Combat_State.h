#pragma once
#include "AI_State.h"

//combat trigger state, the enemy has caught the player, main watches for this and switches to the combat screen//
class AI_Combat_State : public AI_State
{
public:
	AI_Combat_State();
	~AI_Combat_State() override = default;

	void OnEnter(body& agent) override;
	e_AI_StateID OnUpdate(body& agent, body& target, float dt) override;
	void OnExit(body& agent) override;
};