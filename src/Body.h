#pragma once
#include "raylib.h"

//Body Struct, moved out of main so the states can use it//
//named struct (not typedef struct {} body) so in-class member initializers like hp/attack work under MSVC//
 struct body
{
	Vector2 position; //represets top-left corner for aabb collision
	Vector2 size;
	Vector2 velocity;

	float acceleration;
	float drag; //high number = stops faster
	float mass;
	float invMass;

	//combat block//
	int hp = 100;
	int attack = 10;
	bool defending = false;

}; 