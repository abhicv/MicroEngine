#ifndef SYSTEM_H
#define SYSTEM_H

#include "..\Engine\src\MicroECS.h"
#include "data.h"

void PlayerInputSystem(InputComponent *input, SDL_Event *event, GameResource *gameResource);

void PlayerControlSystem(TransformComponent *transform,
                         AnimationComponent *animation,
                         InputComponent *input,
                         PhysicsComponent *physics,
                         EntityStatComponent *stat);

void EnemyPatrolSystem(TransformComponent *transform,
                       EntityStatComponent *stat,
                       AnimationComponent *animation,
                       f32 deltaTime);

void FiringSystem(MicroECSWorld *ecsWorld,
                  InputComponent *input,
                  EntityStatComponent *playerStat,
                  Vector2 playerPosition);

#endif //SYSTEM_H
