#pragma once
#include "TB_State.h"

class TB_Lose_State : public TB_State
{
public:
	TB_Lose_State();
	~TB_Lose_State() override = default;

	void OnEnter(body& agent) override;
	e_TB_StateID OnUpdate(body& agent, body& target, float dt) override;
	void OnExit(body& agent) override;
};