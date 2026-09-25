#pragma once
#include "raylib.h"

//Body Struct, moved out of main so the states can use it//
typedef struct
{
	Vector2 position; //represets top-left corner for aabb collision
	Vector2 size;
	Vector2 velocity;

	float acceleration;
	float drag; // high number = stops faster
	float mass;
	float invMass;
}body; //struct tag