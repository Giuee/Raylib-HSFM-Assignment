#pragma once
#include "TB_State.h"

class TB_PlayerTurn_State : public TB_State
{
public:
	TB_PlayerTurn_State();

	void OnEnter(body& agent) override;
	e_TB_StateID OnUpdate(body& agent, body& target, float dt) override;
	void OnExit(body& agent) override;
};