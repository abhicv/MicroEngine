#include "../Engine/src/MicroECS.h"

global TransformComponent lizardTransform = {
    .position = {300.0f, 300.0f},
    .size = {0.0, 0.0},
    .angle = 0.0f,
};

global RenderComponent lizardRender = {0};

global Animation lizardIdleAnim = {
    .frames = {
        {0, 16},
    },
    .currentFrameIndex = 0,
    .frameInterval  = 60,
    .frameCount = 1,
    .flip = false,
};

global Animation lizardWalkAnim = {
    .frames = {
        {0,  16},
        {16, 16},
        {32, 16},
        {48, 16},
    },
    .currentFrameIndex = 0,
    .frameInterval  = 150,
    .frameCount = 4,
    .flip = false,
};

global AnimationComponent lizardAnimation = {
    .animations = {0},
    .currentAnimationIndex = 1,
    .width = 16,
    .height = 16,
};

global EntityStatComponent lizardStat = {
    .EnemyStat = {
        .health = 1.0f,
        .startPosition = {300, 300},
        .moveRight = false,
        .patrolDistance = 50,
    }
};