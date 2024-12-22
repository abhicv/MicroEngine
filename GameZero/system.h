#ifndef SYSTEM_H
#define SYSTEM_H

#include "..\Engine\src\MicroECS.h"
#include "..\Engine\src\ME_Vector2D.h"

#include "data.h"
void PlayerInputSystem(InputComponent *input, SDL_Event *event);

void PlayerControlSystem(TransformComponent *transform, AnimationComponent *animation,
                         InputComponent *input, PhysicsComponent *physics, EntityStateComponent *stat);

void EnemyPatrolSystem(TransformComponent *transform, EntityStateComponent *stat,
                       AnimationComponent *animation, f32 deltaTime);

void FiringSystem(MicroECSWorld *ecsWorld, InputComponent *input,
                  u32 playerFacingDirection, Vector2 playerPosition);

#endif //SYSTEM_H
