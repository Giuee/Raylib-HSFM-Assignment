#include "AI_Seek_State.h"
#include <iostream>
#include "raylib.h"
#include "raymath.h"

const char* SEEK_STATE_NAME = "Seek"; //constant for the state

AI_Seek_State::AI_Seek_State() : AI_State(e_AI_StateID::Seek, SEEK_STATE_NAME)
{
}
//runs once state has been entered
void AI_Seek_State::OnEnter(body& _agent)
{
}
//will run every frame while chasing
e_AI_StateID AI_Seek_State::OnUpdate(body& _agent, body& _target, float dt)
{
    //position is a public field on body, not a function like Pawn's GetPosition(),
    float distanceToTarget = Vector2Distance(_target.position, _agent.position);

    if (distanceToTarget >= maxSeekDistance)
    {
        return e_AI_StateID::Wander; //target got away, give up and go back to wandering
    }
    else if (distanceToTarget <= minSeekDistance)
    {
        return e_AI_StateID::Attack; //close enough start combat

        Vector2 desired = Vector2Subtract(_target.position, _agent.position); // 1. desired direction, points from agent to target (target - agent, NOT agent - target, or it'd point the wrong way)
        desired = Vector2Normalize(desired); // 2. normalize shrinks it to length 1 (a unit vector), then scale stretches it out to the agent's top speed
        float topSpeed = _agent.acceleration / _agent.drag; //body has no GetSpeed(), so we work out top speed the same way Wander's cap works: acceleration / drag
        desired = Vector2Scale(desired, topSpeed);

        Vector2 steer = Vector2Subtract(desired, _agent.velocity);  // 3. steering force, how far our current velocity is from the velocity we actually want

        if (Vector2LengthSqr(steer) > (_agent.acceleration * _agent.acceleration))  //4. clamp the steering force so it can't push harder than the agent's acceleration allows in one go
        {
            steer = Vector2Normalize(steer);
            steer = Vector2Scale(steer, _agent.acceleration);
        }
        // 5. apply it, body has no ApplyForce() so we add straight onto velocity
        _agent.velocity.x += steer.x * dt;
        _agent.velocity.y += steer.y * dt;

        return e_AI_StateID::Seek; // 6. return our own ID to stay in this state
    }
}

void AI_Seek_State::OnExit(body& _agent) //runs once when we leave the state, nothing to clean up here since drag in StepPhysics slows the enemy down by itself
{
}