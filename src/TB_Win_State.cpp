#include "TB_Win_State.h"
#include <iostream>

TB_Win_State::TB_Win_State() : TB_State(e_TB_StateID::Win, "Win")
{
}

void TB_Win_State::OnEnter(body& agent)
{
	std::cout << "Combat won!" << std::endl;
}

e_TB_StateID TB_Win_State::OnUpdate(body& agent, body& target, float dt)
{
	return GetStateID(); //just sits here, main.cpp watches for this and leaves combat//
}

void TB_Win_State::OnExit(body& agent)
{
}