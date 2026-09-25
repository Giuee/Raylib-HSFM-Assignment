#pragma once

#include "AI_State.h"
#include "raymath.h"
#include <math.h>

//wander state class

class AI_Wander_State : public AI_State
{

private:
	//the three variables that were in the main but are now owned by the state :) 
	bool wanderWalking = false;
	float wanderTime = 0.0f;
	Vector2 wanderDir = { 0.0f, 0.0f }; //direction the enemy is currently moving in with the length of 1 being a unit vector 

public:
	//tells the base class this state's ID and name
	AI_Wander_State() : AI_State(e_AI_StateID::Wander, "Wander") {}

	void OnEnter(body& agent) override
	{
		wanderWalking = false;
		wanderTime = 0.0f; //0 makes it pick a new action on the first frame
		wanderDir = Vector2{ 0.0f, 0.0f };
	}

	//runs evenry frame when making this, this ends up being my block where i had my enemy wander function
	e_AI_StateID OnUpdate(body& agent, body& target, float dt) override
	{
		wanderTime -= dt;
		if (wanderTime <= 0.0f) //timer runs out switch what is being done 
		{
			wanderWalking = !wanderWalking; //flips true to false and vice versa

			if (wanderWalking)
			{
				float angle = GetRandomValue(0, 360) * DEG2RAD; //picks a random angle in degrees and converts to radians god i hate math cosf sinf like really thank the lord i did mathB even then i barely remember crap
				wanderDir = Vector2{ cosf(angle), sinf(angle) }; //cos give s x and sin gives y 
				wanderTime = GetRandomValue(100, 300) / 100.0f; // random time between 1 and 3 seconds
			}
			else
			{
				wanderDir = Vector2{ 0.0f, 0.0f }; //no direction = no movement
				wanderTime = GetRandomValue(100, 200) / 100.0f; // stand still for 1 to 2 seconds
			}
		}

		if (wanderWalking)
		{
			//in theory this is the same idea as the wasd for player movement but coming in the wander direction instead of using a key to input
			agent.velocity.x += wanderDir.x * agent.acceleration * dt;
			agent.velocity.y += wanderDir.y * agent.acceleration * dt;
		}

		return e_AI_StateID::Wander; //stays in wander state
	}

	//runs when leaving the state once and stops anything pushing the enemy

	void OnExit(body& agent) override
	{
		wanderWalking = false;
		wanderDir = Vector2{ 0.0f, 0.0f };
	}


};