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

int main()
{
	InitWindow(screen_Width, screen_Height, "Test");
	SearchAndSetResourceDir("resources");
	SetTargetFPS(60);

	//moving body//
	body player;
	InitBody(&player, Vector2 { screen_Width / 80.0f, 350.0f }, Vector2 { 50.0f, 50.0f }, 1.0f);

	//Enemy Body//
	body enemy;
	InitBody(&enemy, Vector2 { 1000.0f, 350.0f }, Vector2 { 50.0f, 50.0f }, 1.0f);
	enemy.acceleration = 600.0f;

	//Enemy state//
	AI_HFSM_Field enemyFSM; //Enemy state machine
	enemyFSM.InitializeStates(enemy);   //creates both states and starts in Wander, this calls OnEnter for us


	//Main Game Loop//
	while (!WindowShouldClose())
	{
		float dt = GetFrameTime();

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

		//enemy wander//
		enemyFSM.Update(enemy, player); //HFSM runs whichever state is current and switches when needed
		

		//physics//
		StepPhysics(&player, dt);
		StepPhysics(&enemy, dt);


		//Render Graphics//
		BeginDrawing();

		ClearBackground(RAYWHITE);

		DrawRectangleV(player.position, player.size, GOLD); //draws character
		DrawRectangleV(enemy.position, enemy.size, RED); //draws enemy

		EndDrawing();
	}
	// destroy the window and cleanup the OpenGL context
	CloseWindow();
	return 0;
}