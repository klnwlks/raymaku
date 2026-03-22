#include "player.h"
#include "raylib.h"
#include "raymath.h"
#include "config.h"

static Player player = {0};

void InitPlayer()
{
    // init player values
    player.position = (Vector2) { PLAY_AREA_WIDTH / 2.0f, PLAY_AREA_HEIGHT * 0.8f};
    player.speed = (Vector2) {4.0f, 4.0f};
    player.radius = 10.0f;
    player.lives = 3;
    player.bombs = 5;
    player.invincibility = 1;
    player.invincibilityTimer = player.invincibility;
    player.shooting = false;
    player.power = 1;
    
    // Default player pattern: Fast straight bullets shooting UP
    player.pattern = (PatternConfig){ 1, 800.0f, 0, -PI/2.0f, 0, 1, BULLET_LINEAR, 0 };
    player.shootTimer = 0.08f;
    player.currentShootTimer = 0.0f;
}

void UpdatePlayer() 
{
    // update position of player with vector normalization
    Vector2 direction = { 0.0f, 0.0f };
    
    if (IsKeyDown(KEY_RIGHT)) direction.x += 1.0f;
    if (IsKeyDown(KEY_LEFT)) direction.x -= 1.0f;
    if (IsKeyDown(KEY_UP)) direction.y -= 1.0f;
    if (IsKeyDown(KEY_DOWN)) direction.y += 1.0f;

    if ((direction.x != 0) || (direction.y != 0)) {
        direction = Vector2Normalize(direction);

        float currSpeed = player.speed.x;
        if (IsKeyDown(KEY_LEFT_SHIFT)) currSpeed /= 2;

        player.position.x += direction.x * currSpeed;
        player.position.y += direction.y * currSpeed;
    }

    // check if player is within playing boundary
    player.position.x = Clamp(player.position.x, player.radius, PLAY_AREA_WIDTH - player.radius);
    player.position.y = Clamp(player.position.y, player.radius, PLAY_AREA_HEIGHT - player.radius);

    // check iframes
    if (player.invincibilityTimer > 0.0f)
    {
        player.invincibilityTimer -= GetFrameTime();
    }

    // Shooting logic
    player.shooting = IsKeyDown(KEY_Z) || IsKeyDown(KEY_SPACE);
    
    if (player.currentShootTimer > 0.0f) player.currentShootTimer -= GetFrameTime();

    if (player.shooting && player.currentShootTimer <= 0.0f)
    {
        ExecPattern(player.position, player.pattern, BULLET_PLAYER);
        player.currentShootTimer = player.shootTimer;
    }
}

void DrawPlayer()
{
     // actual screen position
    Vector2 screenPos = {
        player.position.x + PLAY_AREA_X_OFFSET,
        player.position.y + PLAY_AREA_Y_OFFSET
    };

    DrawCircleV(screenPos, player.radius, RED);

}

void PlayerHit()
{
    if (player.invincibilityTimer <= 0.0f)
    {
        player.lives--;
        player.invincibilityTimer = player.invincibility;
    }
}

void UnloadPlayer() 
{

}

Player *GetPlayer(void) {
    return &player;
}
