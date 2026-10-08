#pragma once
#include "TB_State.h"

class TB_Win_State : public TB_State
{
public:
	TB_Win_State();
	~TB_Win_State() override = default;

	void OnEnter(body& agent) override;
	e_TB_StateID OnUpdate(body& agent, body& target, float dt) override;
	void OnExit(body& agent) override;
};