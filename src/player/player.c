#include "player.h"
#include "raylib.h"
#include "raymath.h"

static Player player = {0};

void InitPlayer()
{
    
}

void UpdatePlayer() 
{
    // update position of player with vector normalization
    Vector2 direction = { 0.0f, 0.0f };
    
    if (IsKeyDown(KEY_RIGHT)) direction.x += 1.0f;
    if (IsKeyDown(KEY_LEFT)) direction.x += 1.0f;
    if (IsKeyDown(KEY_UP)) direction.x += 1.0f;
    if (IsKeyDown(KEY_DOWN)) direction.x += 1.0f;

    if ((direction.x != 0) || (direction.y != 0)) {
        direction = Vector2Normalize(direction);

        // TODO: add checks to prevent player from going out of bounds
        float currSpeed = player.speed.x;
        if (IsKeyDown(KEY_LEFT_SHIFT)) currSpeed /= 2;

        player.position.x += direction.x;
        player.position.y += direction.y;

    }



}

void DrawPlayer()
{

}

void UnloadPlayer() 
{

}

Player *GetPlayer(void) {
    return &player;
}
