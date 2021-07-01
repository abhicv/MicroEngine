#include "../src/MicroEngine.c"

#define MUI_ORIGIN_ID 1000

global MUI ui;
global MUI_Input uiInput;
global Vector2 gravity = {0.0f, 500.0f};

ME_Rect targetRect = {1280/ 2 - 100, 720 / 2 - 50, 200, 100};
ME_Rect rect = {20, 20, 40, 40};
Vector2 velocity = {0.0f, 0.0f};
f32 contactTime;
Vector2 contactNormal;
SDL_Color color = {255, 255, 255, 0};

void SWAP(f32 *a, f32 *b)
{
    f32 temp = *a;
    *a = *b;
    *b = temp;
}

bool DetectRayRectCollisionT(Vector2 origin, Vector2 dir, ME_Rect *target, f32 *t, Vector2 *contactPoint, Vector2 *contactNormal)
{
    f32 x = target->x - target->width / 2;
    f32 y = target->y - target->height / 2;
    
    f32 xNear = (x - origin.x) / dir.x;
    f32 xFar = (x + target->width  - origin.x) / dir.x;
    f32 yNear = (y - origin.y) / dir.y;
    f32 yFar = (y + target->height  - origin.y) / dir.y;
    
    if(xNear > xFar)
    {
        SWAP(&xNear, &xFar);
    }
    if(yNear > yFar)
    {
        SWAP(&yNear, &yFar);
    }
    
    if(xNear > yFar || yNear > xFar) 
    {
        return false;
    }
    
    f32 tNear = MAX(xNear, yNear);
    f32 tFar = MIN(xFar, yFar);
    
    if(tFar < 0.0f)
    {
        return false;
    }
    
    *t = tNear;
    
    //P`(t) = P(t) + d * t;
    contactPoint->x = origin.x + (dir.x * tNear);
    contactPoint->y = origin.y + (dir.y * tNear);
    
    if(xNear > yNear)
    {
        if(dir.x < 0.0f)
        {
            contactNormal->x = 1.0f;
            contactNormal->y = 0.0f;
        }
        else
        {
            contactNormal->x = -1.0f;
            contactNormal->y = 0.0f;
        }
    }
    else if(xNear < yNear)
    {
        if(dir.y < 0.0f)
        {
            contactNormal->x = 0.0f;
            contactNormal->y = 1.0f;
        }
        else
        {
            contactNormal->x = 0.0f;
            contactNormal->y = -1.0f;
        }
    }
    
    return true;
}

bool DetectRectVsRectCollisionT(ME_Rect *rect, ME_Rect *target, f32 deltaTime, Vector2 *velocity, f32 *t, Vector2 *contactNormal)
{
    Vector2 origin = {rect->x, rect->y};
    Vector2 dir  = {velocity->x * deltaTime, velocity->y * deltaTime};
    Vector2 contactPoint;
    
    //NOTE(abhicv): rect after Minkowski Sum
    ME_Rect enlargedRect = {0};
    enlargedRect.x = target->x;
    enlargedRect.y = target->y;
    enlargedRect.width = target->width + rect->width;
    enlargedRect.height = target->height + rect->height;
    
    if(DetectRayRectCollisionT(origin, dir, &enlargedRect, t, &contactPoint, contactNormal))
    {
        if(*t < 1.0f)
        {
            return true;
        }
    }
    
    return false;
}

#define SPEED 100.0f
void HandleEvent(SDL_Event event)
{
    MUI_GetInput(&uiInput, &event);
    
    switch (event.type)
    {
        case SDL_KEYDOWN:
        switch (event.key.keysym.sym)
        {
            case SDLK_UP:
            velocity.y = -SPEED;
            break;
            
            case SDLK_DOWN:
            velocity.y = SPEED;
            break;
            
            case SDLK_LEFT:
            velocity.x = -SPEED;
            break;
            
            case SDLK_RIGHT:
            velocity.x = SPEED;
            break;
        }
        break;
    }
}

//TODO(abhicv): need some more tests.
void Update(float deltaTime)
{
    if(DetectRectVsRectCollisionT(&rect, &targetRect, 0.016f, &velocity, &contactTime, &contactNormal))
    {
        ME_DEBUG_LOG_COORDINATES(contactNormal);
        
        //velocity.x = velocity.x + (velocity.x) * contactNormal.x * (1.0f - contactTime));
        //velocity.y = velocity.y + (velocity.y) * contactNormal.y * (1.0f - contactTime));
        
        //velocity.x = 10.0f * contactNormal.x;
        //velocity.y = -10.0f * contactNormal.y;
        
        color.g = 0;
        color.b = 0;
    }
    else
    {
        color.g = 255;
        color.b = 255;
    }
    
    rect.x += velocity.x * 0.016f;
    rect.y += velocity.y * 0.016f;
}

void Render(SDL_Renderer *renderer)
{
    {
        SDL_Rect sRect = {0};
        sRect.x = rect.x - rect.width / 2;
        sRect.y = rect.y - rect.height / 2;
        sRect.w = rect.width;
        sRect.h = rect.height;
        ME_RenderDrawRect(renderer, &sRect, color);
    }
    
    {
        SDL_Rect sRect = {0};
        sRect.x = targetRect.x - targetRect.width / 2;
        sRect.y = targetRect.y - targetRect.height / 2;
        sRect.w = targetRect.width;
        sRect.h = targetRect.height;
        ME_RenderDrawRect(renderer, &sRect, color);
    }
    
    MUI_EndFrame(&ui, renderer);
}

int main(int argc, char *argv[])
{
    ME_Game game = ME_CreateGame("MicroPhysics_test", 1280, 720);
    game.update = Update;
    game.handleEvent = HandleEvent;
    game.render = Render;
    
    ME_RunGame(&game, false);
    
    ME_QuitGame(&game);
    
    return 0;
}