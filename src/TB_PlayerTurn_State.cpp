#include "TB_PlayerTurn_State.h"
#include "raylib.h"
#include <iostream>

extern bool attackButtonPressed; //set in main.cpp's draw phase when the Attack button is clicked
extern bool defendButtonPressed; //set in main.cpp's draw phase when the Defend button is clicked
extern bool skillButtonPressed; //set in main.cpp's draw phase when the Skill button is clicked
extern bool skillMenuOpen; //set in main.cpp's draw phase when the Skill button is clicked
extern float playerLungeTimer;
extern int skillChosenIndex; //set in main.cpp's draw phase when a skill is clicked, -1 means no skill picked, 0 means first skill, 1 means second skill, etc.


const char* PLAYER_TURN_STATE_NAME = "Player Turn"; //constructor for the player turn state, sets the name and ID of the state

//constructor for the player turn state//
TB_PlayerTurn_State::TB_PlayerTurn_State() : TB_State(e_TB_StateID::PlayerTurn, PLAYER_TURN_STATE_NAME) //Call the base class constructor with the state ID and name
{
} 
//On turn start runs and resets all the button flags and the chosen skill//
void TB_PlayerTurn_State::OnEnter(body& agent)
{
	std::cout << "Entering state: " << GetStateName() << std::endl;
	//Reset the buttons state when entering the player's turn
	attackButtonPressed = false;
	defendButtonPressed = false;
	skillButtonPressed = false;
	skillMenuOpen = false;
	skillChosenIndex = -1; //reset skill chosen index
}
//runs during the player's turn checks which button was pressed and applies it, then returns the next state//
e_TB_StateID TB_PlayerTurn_State::OnUpdate(body& agent, body& target, float dt)
{
	if (attackButtonPressed)
	{
		attackButtonPressed = false; //Reset the button state after processing the attack

		int damage = agent.attack;//dmg is just attacks attribute for now, can be modified later for skills or buffs or even defence stats 
		target.hp -= damage; //Apply damage to the target's HP

		std::cout << "Attack triggered" << std::endl; //Log when the attack is triggered
		playerLungeTimer = 0.4f;
	
		if (target.hp <= 0)
		{
			target.hp = 0; //Ensure HP doesn't go below 0
			std::cout << "Target defeated!" << std::endl; //Log when the target is defeated
			return e_TB_StateID::Win; //Transition to the Win state if the target is defeated
		}
		return e_TB_StateID::EnemyTurn; //Transition to the Enemy Turn state after the attack
	}


	if (defendButtonPressed)
	{
		defendButtonPressed = false; //Reset the button state after processing the defend action
		agent.defending = true; //Set the defending flag to true for the agent
		std::cout << "Defend triggered" << std::endl; // Log when the defend is triggered
		return e_TB_StateID::EnemyTurn; //Transition to the Enemy Turn state after defending
	}

	if (skillButtonPressed)
	{
		skillButtonPressed = false; //Reset the button state after processing the skill action
		skillMenuOpen = true; //Open the skill menu for the player to choose a skill
		skillChosenIndex = -1;//forcing a clean state each time the menu opens
		
	}
	if (skillMenuOpen && skillChosenIndex >= 0) //Check if the skill menu is open and a skill has been clicked
	{
		skillMenuOpen = false; //Close the skill menu after a skill has been chosen
		int damage = 0; //Initialize damage variable
		switch (skillChosenIndex)
		{
		case 0: //attack skill
			damage = agent.attack * 2;
			break;
		case 1: //heal skill
			agent.hp += 15;
			if (agent.hp > 100) agent.hp = 100; //Cap HP at 100
			break;
		default:
			break;
		}
		std::cout << "Skill triggered" << std::endl; //Log when the skill is triggered
		
		playerLungeTimer = 0.05f;
		if (damage > 0)
		{
			target.hp -= damage;
		}

		skillChosenIndex = -1;   //reset for next time//

		if (target.hp <= 0)
		{
			target.hp = 0;
			return e_TB_StateID::Win;
		}
		return e_TB_StateID::EnemyTurn;
	}

	return GetStateID(); //Remain in the Player Turn state until an external event triggers a state change
}
//player turn ends changes state to enemy turn//
void TB_PlayerTurn_State::OnExit(body& agent)
{
	std::cout << "Exiting state: " << GetStateName() << std::endl; //Log when exiting the state
}