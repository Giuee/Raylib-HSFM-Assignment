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
#include "TB_HFSM_Combat.h"
#include "resource_dir.h"	// utility header for SearchAndSetResourceDir
#include <iostream>

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#define EASINGS_STATIC_INLINE
#include "reasings.h"

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

//TB constraints//
bool attackButtonPressed = false;
bool defendButtonPressed = false;
bool skillButtonPressed = false;
bool skillMenuOpen = false;
float playerLungeTimer = 0.0f;
float enemyLungeTimer = 0.0f;
const float LUNGE_DURATION = 0.4f;
int skillChosenIndex = -1; //-1 means no skill picked
//win/lose//
float combatResultTimer = 0.0f; //timer for the win/lose screen
const float COMBAT_RESULT_DURATION = 2.0f; //duration of the win/lose screen

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
	TB_HFSM_Combat combatFSM;

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
				player.hp = 100; //fresh hp each combat encounter
				enemy.hp = 100;
				player.defending = false; //reset defending to false each combat encounter
				playerLungeTimer = 0.0f;
				enemyLungeTimer = 0.0f;
				combatResultTimer = 0.0f; //reset the win/lose timer

				combatFSM.InitializeStates(player); //creates the player turn state and sets it as the current state, this calls OnEnter
				currentState = GAMESTATE_COMBAT; //switch to combat screen when the enemy catches the player

			}

			//physics//
			StepPhysics(&player, dt);
			StepPhysics(&enemy, dt);

			//collision resolution
			if (AABB(player, enemy))
			{
				ResolveCollision(&player, &enemy);
				
			}

			//wraps player and enemy around the screen
			WrapBodyPosition(&player);
			WrapBodyPosition(&enemy);
		} break; //end of play case 

		case GAMESTATE_COMBAT:
		{
			if (playerLungeTimer > 0.0f) playerLungeTimer -= dt;
			if (enemyLungeTimer > 0.0f) enemyLungeTimer -= dt;
			combatFSM.Update(player, enemy); //runs the turn based combat state machine, this is where the player can attack and the enemy can attack back
				
			//go back to field state on win/lose, but wait a few seconds so the player can see the result of the combat
			e_TB_StateID combatID = combatFSM.GetCurrentState()->GetStateID();

			if (combatID == e_TB_StateID::Win || combatID == e_TB_StateID::Lose)
			{
				combatResultTimer += dt;

				if (combatResultTimer >= COMBAT_RESULT_DURATION)
				{
					// put the bodies well apart so they don't instantly collide again
					InitBody(&player, Vector2{ 200.0f, 350.0f }, Vector2{ 50.0f, 50.0f }, 1.0f);
					InitBody(&enemy, Vector2{ 1000.0f, 350.0f }, Vector2{ 50.0f, 50.0f }, 1.0f);
					enemy.acceleration = 600.0f;

					enemyFSM.InitializeStates(enemy); //recreates the states and starts back in Wander

					currentState = GAMESTATE_PLAY;
				}
			}

		}break;

		default:
			break;
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
				DrawRectangleV(player.position, player.size, GOLD); //draws character
				DrawRectangleV(enemy.position, enemy.size, RED);    //draws enemy
			} break;

			case GAMESTATE_COMBAT:
			{
	

				Vector2 combatSize = { 100.0f, 100.0f };
				Vector2 playerSpot = { 150.0f, screen_Height - 250.0f };
				Vector2 enemySpot = { screen_Width - 250.0f, 150.0f };
				Vector2 lungeOffset = { 0.0f, 0.0f };
				Vector2 enemyLungeOffset = { 0.0f, 0.0f };

				if (playerLungeTimer > 0.0f) 
				{
					float elapsed = LUNGE_DURATION - playerLungeTimer;//seconds since the attack started
					float half = LUNGE_DURATION / 2.0f;

					float lunge;
					if (elapsed < half)
					{
						lunge = EaseCubicOut(elapsed, 0.0f, 1.0f, half);//fast strike toward the enemy
					}
					else
					{
						lunge = EaseSineIn(elapsed - half, 1.0f, -1.0f, half); //slow return back 
					}
					lungeOffset = { lunge * 100.0f, -lunge * 100.0f }; //should go right then up 
				}

				Vector2 playerDrawPos = { playerSpot.x + lungeOffset.x, playerSpot.y + lungeOffset.y };
				DrawRectangleV(playerDrawPos, combatSize, GOLD);
				

				if (enemyLungeTimer > 0.0f)
				{
					float elapsed = LUNGE_DURATION - enemyLungeTimer;
					float half = LUNGE_DURATION / 2.0f;

					float lunge;
					if (elapsed < half)
					{
						lunge = EaseCubicOut(elapsed, 0.0f, 1.0f, half);
					}
					else
					{
						lunge = EaseSineIn(elapsed - half, 1.0f, -1.0f, half);
					}
					enemyLungeOffset = { -lunge * 100.0f, lunge * 100.0f }; // left then down, the reverse of the player
				}

				Vector2 enemyDrawPos = { enemySpot.x + enemyLungeOffset.x, enemySpot.y + enemyLungeOffset.y };
				DrawRectangleV(enemyDrawPos, combatSize, MAROON);
				

				//Health bars using GuiProgressBar
				//shape, x, y, width, height, text on left, text on right, value, min, max
				//player bar, sits just below their box

				float playerHpFloat = (float)player.hp; //Convert player HP to float for the progress bar
				float enemyHpFloat = (float)enemy.hp; //Convert enemy HP to float for the progress bar

				GuiProgressBar(Rectangle{ playerSpot.x - 50.0f, playerSpot.y + combatSize.y + 10.0f, 200.0f, 24.0f },
					NULL, TextFormat("%d / 100", player.hp), &playerHpFloat, 0.0f, 100.0f);
				
				//enemy bar, sits just above their box
				GuiProgressBar(Rectangle{ enemySpot.x - 50.0f, enemySpot.y - 34.0f, 200.0f, 24.0f },
					NULL, TextFormat("%d / 100", enemy.hp), & enemyHpFloat, 0.0f, 100.0f);


				static int skillScrollingIndex = 0; //static variable to keep track of the scrolling index for the skill menu
				static int skillActiveItem = -1; //static variable to keep track of the active item in the skill menu that is seperate from skillScrollingIndex so the player can scroll through the list without changing the active item

				if (!skillMenuOpen) //! not open
				{
					if (GuiButton(Rectangle{ 700, 650, 180, 50 }, "attack")) { attackButtonPressed = true; }//sets the global variable to true so the player turn state can see it and process the attack
					if (GuiButton(Rectangle{ 1080, 650, 180, 50 }, "defend")) { defendButtonPressed = true; }//sets the global variable to true so the player turn state can see it and process the attack
					if (GuiButton(Rectangle{ 890, 650, 180, 50 }, "skill")) { skillButtonPressed = true; }//sets the global variable to true so the player turn state can see it and process the attack
				}
				else
				{
					GuiListView(Rectangle{ 700, 400, 300, 150 }, "Attack;Heal", &skillScrollingIndex, &skillActiveItem); //list of skills, returns the index of the clicked skill


					if (skillActiveItem >= 0) //only fires on current frame when not when scrolling
					{
						skillChosenIndex = skillActiveItem; //set the chosen skill index to the clicked index 0 = attack skill 1 = heal skill
						skillActiveItem = -1; //reset the active item so the player can scroll through the list again

						std::cout << "CHOSE SKILL " << skillChosenIndex << ", menu closed" << std::endl;
					}
				}

				e_TB_StateID combatID = combatFSM.GetCurrentState()->GetStateID();

				if (combatID == e_TB_StateID::Win)
				{
					DrawText("You Win!", 550, 300, 40, GREEN);
				}
				else if (combatID == e_TB_StateID::Lose)
				{
					DrawText("You Died :)", 550, 300, 40, RED);
				}
			} break;

			default: break;
			}

			EndDrawing();
	}
		// destroy the window and cleanup the OpenGL context
		CloseWindow();
		return 0;
}
	
