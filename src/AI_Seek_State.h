#pragma once
#include "AI_State.h"

class AI_Seek_State : public AI_State {
public:
    AI_Seek_State();
    ~AI_Seek_State() override = default;

    // Override the AI_State methods
    void OnEnter(body& _agent) override;   // Called when entering the seek state
    e_AI_StateID OnUpdate(body& _agent, body& _target, float dt) override;  // Called every frame while in the seek state
    void OnExit(body& _agent) override;    // Called when exiting the seek state
private:
    // The distance at which the agent considers it has "arrived" at the target.
    const float maxSeekDistance = 80.0f;
    const float minSeekDistance = 20.0f; // Minimum distance to the target to stop seeking
};