#include "../../include/MicroEngine/MicroEngine.h"

//Initializing
ME_GameObject **vectors;
Vector2 mousePos;
int num_vectors = 0;
Vector2 ref_vect = {1, 0}; //x_axis unit vector()

//fnc declare
void UpdateVectorField();

void HandleEvents(SDL_Event event)
{
    mousePos.x = event.motion.x;
    mousePos.y = event.motion.y;
}

void Update(float deltaTime)
{
    UpdateVectorField();
}

void Render(SDL_Renderer *renderer)
{
    int n = 0;
    for(n = 0; n < num_vectors; n++)
    {
        ME_RenderGameObject(vectors[n], renderer);
    }
}

//app specific functions
void PlaceVectors(const int size)
{
    int x, y;
    int pos_x = size / 2;
    int pos_y = size / 2;

    int x_limit = ME_GetScreenWidth() / size;
    int y_limit = ME_GetScreenHeight() / size;

    num_vectors  = (x_limit * y_limit);

    vectors = (ME_GameObject*)malloc(sizeof(ME_GameObject*) * num_vectors);

    //placing vectors
    for(y = 0; y < y_limit; y++)
    {
        pos_x = size / 2;
        for(x = 0; x < x_limit; x++)
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

void UpdateVectorField()
{
    int n = 0;
    for(n = 0; n < num_vectors; n++)
    {
        Vector2 dir = Vector2Subtract(vectors[n]->position, mousePos);
        float dot = Vector2Dot(ref_vect, dir);
        //printf(" %d : dot = %f\n", n + 1, dot);

        if(dot >= 0.0f)
            vectors[n]->angle = 180 * acos(dot / Vector2Mag(dir)) / M_PI;
        else
            vectors[n]->angle = 360 - (180 * acos(dot / Vector2Mag(dir)) / M_PI);
    }
}

int main(int argc, char *argv[])
{
    if(!ME_Init("Vector field visualize",800, 800))
    {
        SDL_Quit();
        return EXIT_FAILURE;
    }

    PlaceVectors(20);
    ME_Run(&HandleEvents, &Update, &Render);

    //cleaning routines
    int n = 0;
    for(n = 0; n < num_vectors; n++)
    {
        ME_DestroyGameObject(vectors[n]);
    }

    ME_Quit();

    return EXIT_SUCCESS;
}
