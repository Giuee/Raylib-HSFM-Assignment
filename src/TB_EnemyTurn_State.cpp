#include "TB_EnemyTurn_State.h"
#include <iostream>

extern float enemyLungeTimer;

TB_EnemyTurn_State::TB_EnemyTurn_State() : TB_State(e_TB_StateID::EnemyTurn, "Enemy Turn")
{
}
//enemy's turn starts and sets the delay timer so the player has a moment before the enemy attacks//
void TB_EnemyTurn_State::OnEnter(body& agent)
{
	turnTimer = 1.5f; //wait 1.5 second before the enemy swings, gives the player a moment to see what's happening
	std::cout << "Enemy Turn" << std::endl;
}
//enemy turn waits for the timer to finish then attacks the player halving the damage if they defended and switches to the next state based on the player's hp//
e_TB_StateID TB_EnemyTurn_State::OnUpdate(body& agent, body& target, float dt)
{
	//agent is always player, target is always enemy
	turnTimer -= dt;
	if (turnTimer > 0.0f)
	{
		return GetStateID(); //still waiting
	}
	enemyLungeTimer = 0.4f;
	int damage = target.attack; //target = enemy, enemy is attacking
	
	if (agent.defending)
	{
		damage /= 2; //if the player is defending, halve the damage
		agent.defending = false;//defence only lasts 1 hit
		std::cout << "player defended" << damage << std::endl;
	}

	agent.hp -= damage;//agent = player, player takes the hit
	std::cout << "Enemy attacked, player hp: " << agent.hp << std::endl;

	if (agent.hp <= 0)
	{
		agent.hp = 0;
		return e_TB_StateID::Lose;
	}

	return e_TB_StateID::PlayerTurn;
}
//enemy turn ends//
void TB_EnemyTurn_State::OnExit(body& agent)
{
}