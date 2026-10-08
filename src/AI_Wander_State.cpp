#include "AI_Wander_State.h"
#include "raylib.h"

//tells the base class this state's ID and name
AI_Wander_State::AI_Wander_State() : AI_State(e_AI_StateID::Wander, "Wander")
{
}

void AI_Wander_State::OnEnter(body& agent)
{
	wanderWalking = false;
	wanderTime = 0.0f; //0 makes it pick a new action on the first frame
	wanderDir = Vector2{ 0.0f, 0.0f };
}

//runs every frame, this ends up being my block where i had my enemy wander function
e_AI_StateID AI_Wander_State::OnUpdate(body& agent, body& target, float dt)
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

	//position is the top-left corner, so measure between the centres of the boxes
	Vector2 agentCentre = { agent.position.x + agent.size.x * 0.5f, agent.position.y + agent.size.y * 0.5f };
	Vector2 targetCentre = { target.position.x + target.size.x * 0.5f, target.position.y + target.size.y * 0.5f };

	if (Vector2Distance(agentCentre, targetCentre) < detectRange)
	{
		return e_AI_StateID::Seek; //switch to chase state
	}

	return e_AI_StateID::Wander; //stays in wander state
}

//runs when leaving the state once and stops anything pushing the enemy

void AI_Wander_State::OnExit(body& agent)
{
	wanderWalking = false;
	wanderDir = Vector2{ 0.0f, 0.0f };
}
