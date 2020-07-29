#include "../src/MicroEngine.c"

//Initializing
global ME_GameObject **vectors = NULL;

global u32 num_vectors = 0;

global Vector2 mousePos;
global Vector2 ref_vect = {1.0f, 0.0f}; //x_axis unit vector()

internal void UpdateVectorField(void);

void handleEvent(SDL_Event event)
{
    mousePos.x = event.motion.x;
    mousePos.y = event.motion.y;
}

void update(f32 deltaTime)
{
    UpdateVectorField();
}

void render(SDL_Renderer *renderer)
{
    u32 n = 0;
    for (n = 0; n < num_vectors; n++)
    {
        ME_RenderGameObject(vectors[n], renderer);
    }
}

//ME_Game specific
internal void PlaceVectors(const u32 size)
{
    u32 x = 0, y = 0;
    i32 pos_x = size / 2;
    i32 pos_y = size / 2;

    int x_limit = ME_GetScreenWidth() / size;
    int y_limit = ME_GetScreenHeight() / size;

    num_vectors = (x_limit * y_limit);

    vectors = (ME_GameObject **)malloc(sizeof(ME_GameObject) * num_vectors);

    //placing vectors
    for (y = 0; y < y_limit; y++)
    {
        pos_x = size / 2;
        for (x = 0; x < x_limit; x++)
        {
            vectors[(y * x_limit) + x] = ME_CreateGameObject(pos_x, pos_y);
            vectors[(y * x_limit) + x]->destRect.w = size;
            vectors[(y * x_limit) + x]->destRect.h = size;
            vectors[(y * x_limit) + x]->texture = IMG_LoadTexture(ME_GetRenderer(), "assets/Sprites/Vector_arrow.png");
            pos_x += size;
        }
        pos_y += size;
    }
}

internal void UpdateVectorField()
{
    u32 n = 0;
    for (n = 0; n < num_vectors; n++)
    {
        Vector2 dir = Vector2Subtract(vectors[n]->position, mousePos);
        f32 dot = Vector2Dot(ref_vect, dir);
        //printf(" %d : dot = %f\n", n + 1, dot);

        if (dot >= 0.0f)
            vectors[n]->angle = 180 * acos(dot / Vector2Mag(dir)) / M_PI;
        else
            vectors[n]->angle = 360 - (180 * acos(dot / Vector2Mag(dir)) / M_PI);
    }
}

int main(int argc, char *argv[])
{
    if (!ME_Init("Vector field visualize", 800, 800))
    {
        ME_Quit();
        return 1;
    }

    PlaceVectors(20);
    ME_Run(handleEvent, update, render);

    //cleaning routines
    u32 n = 0;
    for (n = 0; n < num_vectors; n++)
    {
        ME_DestroyGameObject(vectors[n]);
    }

    ME_Quit();

    return 0;
}
