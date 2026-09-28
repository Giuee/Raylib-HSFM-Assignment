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

		const float detectRange = 300.0f; //the range at which the enemy will detect the player and switch to chase state


	public:

		AI_Wander_State();
		~AI_Wander_State() override = default;

		void OnEnter(body& agent) override; // Called when entering the state
		e_AI_StateID OnUpdate(body& agent, body& target, float dt) override; // Called every frame while in the state
		void OnExit(body& agent) override; // Called when exiting the state

};