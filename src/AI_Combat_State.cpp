#include "AI_Combat_State.h"

AI_Combat_State::AI_Combat_State() : AI_State(e_AI_StateID::Combat, "Combat")
{
}
//runs when enemy catches the player and transitions to combat screen
void AI_Combat_State::OnEnter(body& agent)
{
	agent.velocity = Vector2{ 0.0f, 0.0f }; //stop the enemy dead when it catches the player//
}
//stay in the combat state
e_AI_StateID AI_Combat_State::OnUpdate(body& agent, body& target, float dt)
{
	return e_AI_StateID::Combat; //stays here, main sees this and switches screens//
}

void AI_Combat_State::OnExit(body& agent)
{
}