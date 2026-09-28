/*
Raylib example file.
This is an example main file for a simple raylib project.
Use this as a starting point or replace it with your code.

by Jeffery Myers is marked with CC0 1.0. To view a copy of this license, visit https://creativecommons.org/publicdomain/zero/1.0/

*/

#include "raylib.h"
#include "raymath.h"
#include "math.h" //used for cosf and sinf 
#include "AI_HFSM_Field.h" //pulls in the AI state machine and states
#include "resource_dir.h"	// utility header for SearchAndSetResourceDir

typedef enum GameState
{
	GAMESTATE_MENU,
	GAMESTATE_PLAY,
	GAMESTATE_COMBAT,
	GAMESTATE_GAMEOVER
} GameState;

//constraints//
int screen_Width = 1280;
int screen_Height = 720;

const float GROUND_Y = 300.0f;
const float ACCELERATION = 1650.0f;
const float DRAG = 5.0f;

//initialising the body with position size etc//
void InitBody(body* b, Vector2 pos, Vector2 bounds, float mass)
{
	b->position = pos;
	b->size = bounds;
	b->velocity = Vector2{ 0.0f, 0.0f };
	b->acceleration = ACCELERATION; //calls to constraint
	b->drag = DRAG;

	b->mass = mass;
	b->invMass = 0.0f;

	if (mass > 0.0f)
	{
		b->invMass = 1.0f / mass;
	}
}

//AABB Collision which turns true when A box overlaps with B box//
bool AABB(body bodyA, body bodyB)
{
	bool overlapX = false;
	bool overlapY = false;

	overlapX = (bodyA.position.x < bodyB.position.x + bodyB.size.x) && (bodyA.position.x + bodyA.size.x > bodyB.position.x);
	overlapY = (bodyA.position.y < bodyB.position.y + bodyB.size.y) && (bodyA.position.y + bodyA.size.y > bodyB.position.y);

	return overlapX && overlapY;
}

//Penetration Resolution so the box doesn't teleport above the platform etc//

void ResolveCollision(body* a, body* b)
{
	float overlapLeft = (a->position.x + a->size.x) - b->position.x;
	float overlapRight = (b->position.x + b->size.x) - a->position.x;
	float overlapTop = (a->position.y + a->size.y) - b->position.y;
	float overlapBottom = (b->position.y + b->size.y) - a->position.y;

	float minOverlap = overlapLeft;

	float totalInvMass = a->invMass + b->invMass;

	if (totalInvMass == 0.0f) return; //if both are static, stops execution as theres nothing to resolve

	float ratioA = a->invMass / totalInvMass;
	float ratioB = b->invMass / totalInvMass;

	int pushDirection = 0; // 0 = Left, 1 = Right, 2 = Top, 3 = Bottom

	if (overlapRight < minOverlap)
	{
		minOverlap = overlapRight;
		pushDirection = 1;
	}
	if (overlapTop < minOverlap)
	{
		minOverlap = overlapTop;
		pushDirection = 2;
	}
	if (overlapBottom < minOverlap)
	{
		minOverlap = overlapBottom;
		pushDirection = 3;
	}
	if (pushDirection == 0)
	{
		a->position.x -= minOverlap * ratioA;
		b->position.x += minOverlap * ratioB;
		if (a->invMass == 0.0f)
		{
			b->velocity.x = 0.0f;
		}
		else if (b->invMass == 0.0f)
		{
			a->velocity.x = 0.0f;
		}
		else
		{
			float totalMass = a->mass + b->mass;
			float combined = (a->velocity.x * a->mass + b->velocity.x * b->mass) / totalMass;

			a->velocity.x = combined;
			b->velocity.x = combined;
		}
	}
	else if (pushDirection == 1)
	{
		a->position.x += minOverlap * ratioA;
		b->position.x -= minOverlap * ratioB;
		if (a->invMass == 0.0f)
		{
			b->velocity.x = 0.0f;
		}
		else if (b->invMass == 0.0f)
		{
			a->velocity.x = 0.0f;
		}
		else
		{
			float totalMass = a->mass + b->mass;
			float combined = (a->velocity.x * a->mass + b->velocity.x * b->mass) / totalMass;

			a->velocity.x = combined;
			b->velocity.x = combined;
		}
	}
	else if (pushDirection == 2)
	{
		a->position.y -= minOverlap * ratioA;
		b->position.y += minOverlap * ratioB;
		a->velocity.y = 0.0f;
	}
	else if (pushDirection == 3)
	{
		a->position.y += minOverlap * ratioA;
		b->position.y -= minOverlap * ratioB;
		a->velocity.y = 0.0f;
	}
}
//step the physics for a body//
void StepPhysics(body* b, float dt)
{
	if (b->invMass == 0.0f)
		return;

	// Apply velocity to position
	b->position.x += b->velocity.x * dt;
	b->position.y += b->velocity.y * dt;

	// Apply drag
	b->velocity.x -= b->velocity.x * b->drag * dt;
	b->velocity.y -= b->velocity.y * b->drag * dt;
}

//wraps a body around the screen edge and comes back in from the opposite edge//
void WrapBodyPosition(body* b)
{
	if (b->position.x < 0.0f)
	{
		b->position.x = screen_Width - b->size.x;
	}
	else if (b->position.x + b->size.x > screen_Width)
	{
		b->position.x = 0.0f;
	}
	if (b->position.y < 0.0f)
	{
		b->position.y = screen_Height - b->size.y;
	}
	else if (b->position.y + b->size.y > screen_Height)
	{
		b->position.y = 0.0f;
	}
}

int main()
{
	InitWindow(screen_Width, screen_Height, "RPGMaker MVCPP");
	SearchAndSetResourceDir("resources");
	SetTargetFPS(60);

	//which screen the game is on//
	GameState currentState = GameState::GAMESTATE_MENU;

	//bodies + enemy state machine made then reset when player presses play//
	body player;
	body enemy;
	AI_HFSM_Field enemyFSM;

	//Main Game Loop//
	while (!WindowShouldClose())

	{
		float dt = GetFrameTime();

		switch (currentState)
		{
		case GAMESTATE_MENU:
		{
			if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE))
			{
				InitBody(&player, Vector2{ screen_Width / 80.0f, 350.0f }, Vector2{ 50.0f, 50.0f }, 1.0f);
				InitBody(&enemy, Vector2{ 1000.0f, 350.0f }, Vector2{ 50.0f, 50.0f }, 1.0f);
				enemy.acceleration = 600.0f;
				enemyFSM.InitializeStates(enemy);   //creates both states and starts in Wander, this calls OnEnter

				currentState = GAMESTATE_PLAY;
			}
		} break;

		case GAMESTATE_PLAY:
		{

			//movement//
			if (IsKeyDown(KEY_D))
			{
				player.velocity.x += player.acceleration * dt;
			}

			if (IsKeyDown(KEY_A))
			{
				player.velocity.x -= player.acceleration * dt;
			}

			if (IsKeyDown(KEY_S))
			{
				player.velocity.y += player.acceleration * dt;
			}

			if (IsKeyDown(KEY_W))
			{
				player.velocity.y -= player.acceleration * dt;
			}

			//enemy HFSM//
			enemyFSM.Update(enemy, player); //HFSM runs whichever state is current and switches when needed

			if (enemyFSM.GetCurrentState()->GetStateID() == e_AI_StateID::Combat) //if the enemy has caught the player, switch the screen to combat state in this case the turn based combat screen
			{
				currentState = GAMESTATE_COMBAT; //switch to combat screen when the enemy catches the player
			}

			//physics//
			StepPhysics(&player, dt);
			StepPhysics(&enemy, dt);

			//collision resolution
			if (AABB(player, enemy))
			{
				ResolveCollision(&player, &enemy);
				currentState = GAMESTATE_COMBAT; //switch to combat screen when the player and enemy collide
			}

			//wraps player and enemy around the screen
			WrapBodyPosition(&player);
			WrapBodyPosition(&enemy);
		} break; //end of play case 

		case GAMESTATE_COMBAT:
		{
			//no logic placehodler to see if works can press a key to go back to field
			if (IsKeyPressed(KEY_SPACE))
			{
				// put the bodies well apart so they don't instantly collide again
				InitBody(&player, Vector2{ 200.0f, 350.0f }, Vector2{ 50.0f, 50.0f }, 1.0f);
				InitBody(&enemy, Vector2{ 1000.0f, 350.0f }, Vector2{ 50.0f, 50.0f }, 1.0f);
				enemy.acceleration = 600.0f;

				enemyFSM.InitializeStates(enemy); // recreates the states and starts back in Wander

				currentState = GAMESTATE_PLAY;
			}
		}break;

		default: break;
		} //end of update switch 

			//Render Graphics//
			BeginDrawing();
			ClearBackground(RAYWHITE);

			switch (currentState)
			{
			case GAMESTATE_MENU:
			{
				DrawText("Not an RPG Maker Game", 400, 200, 40, DARKPURPLE);
				DrawText("Press Enter or Space to Start", 475, 300, 20, DARKPURPLE);
			} break;

			case GAMESTATE_PLAY:
			{
				DrawRectangleV(player.position, player.size, GOLD); //draws character//
				DrawRectangleV(enemy.position, enemy.size, RED);    //draws enemy//
			} break;

			case GAMESTATE_COMBAT:
			{
				Vector2 combatSize = { 100.0f, 100.0f };
				Vector2 playerSpot = { 150.0f, screen_Height - 250.0f };
				Vector2 enemySpot = { screen_Width - 250.0f, 150.0f };

				DrawRectangleV(playerSpot, combatSize, GOLD);
				DrawRectangleV(enemySpot, combatSize, RED);
			} break;

			default: break;
			}

			EndDrawing();
	}
		// destroy the window and cleanup the OpenGL context
		CloseWindow();
		return 0;
}
	
