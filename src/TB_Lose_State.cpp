#include "TB_Lose_State.h"
#include <iostream>

TB_Lose_State::TB_Lose_State() : TB_State(e_TB_StateID::Lose, "Lose")
{
}

void TB_Lose_State::OnEnter(body& agent)
{
	std::cout << "Combat lost!" << std::endl;
}

e_TB_StateID TB_Lose_State::OnUpdate(body& agent, body& target, float dt)
{
	return GetStateID(); //just sits here, main.cpp watches for this and leaves combat//
}

void TB_Lose_State::OnExit(body& agent)
{
	std::cout << "Exiting Lose State." << std::endl;
}