#include "../Engine/src/MicroECS.h"

global TransformComponent transform = {
    .position = {500.0f, 300.0f},
    .size = {0.0, 0.0},
    .angle = 0.0f,
};

global RenderComponent render = {0};

global Animation idleAnim = {
    .frames = {
        {16, 16},
    },
    .currentFrameIndex = 0,
    .frameInterval  = 60,
    .frameCount = 1,
    .flip = false,
};

global Animation walkAnim = {
    .frames = {
        {0,  0},
        {16, 0},
        {32, 0},
        {16, 0},
    },
    .currentFrameIndex = 0,
    .frameInterval  = 150,
    .frameCount = 4,
    .flip = false,
};

global Animation jumpAnim = {
    .frames = {
        {0, 16},
    },
    .currentFrameIndex = 0,
    .frameInterval  = 150,
    .frameCount = 1,
    .flip = false,
};

global AnimationComponent anim = {
    .animations = {0},
    .currentAnimationIndex = 0,
    .width = 16,
    .height = 16,
};

global EntityStatComponent PlayerStat = {
    .PlayerStat = {
        .health = 100,
        .facingDir = Right,
    }
};
