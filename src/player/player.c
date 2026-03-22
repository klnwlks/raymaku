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
    player.radius = 3.0f; // Small death hitbox
    player.pickupRadius = 24.0f; // Generous item pickup radius
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
        player.focused = IsKeyDown(KEY_LEFT_SHIFT);
        if (player.focused) currSpeed /= 2;

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

    // Power Tiers: 0-19 (0 options), 20-39 (1 option), 40-79 (2 options), 80-127 (3 options), 128+ (4 options)
    player.activeOptions = player.power / 20;
    if (player.activeOptions > MAX_OPTIONS) player.activeOptions = MAX_OPTIONS;

    // Update Option Positions based on Focused mode
    float spread = player.focused ? 20.0f : 40.0f;
    float heightOffset = player.focused ? 10.0f : 0.0f;

    for (int i = 0; i < player.activeOptions; i++)
    {
        float angle = (i % 2 == 0) ? -1.0f : 1.0f;
        float distFactor = ((float) i / 2 + 1);
        player.optionPos[i].x = player.position.x + (angle * spread * distFactor);
        player.optionPos[i].y = player.position.y - heightOffset + (player.focused ? 0 : 10.0f * distFactor);
    }

    // Update main pattern based on power and focus
    if (player.power < 20)
    {
        // Basic 3-way spread
        float arc = player.focused ? 0.1f : 0.4f;
        player.pattern = (PatternConfig){ 3, 1000.0f, arc, -PI/2.0f, 0, 1, BULLET_LINEAR, 0 };
    }
    else
    {
        // Advanced 3-way spread with higher speed
        float arc = player.focused ? 0.05f : 0.6f;
        player.pattern = (PatternConfig){ 3, 1200.0f, arc, -PI/2.0f, 0, 1, BULLET_LINEAR, 0 };
    }

    // Shooting logic
    player.shooting = IsKeyDown(KEY_Z) || IsKeyDown(KEY_SPACE);
    
    if (player.currentShootTimer > 0.0f) player.currentShootTimer -= GetFrameTime();

    if (player.shooting && player.currentShootTimer <= 0.0f)
    {
        // Shoot from main player
        ExecPattern(player.position, player.pattern, BULLET_PLAYER);
        
        // Shoot from active options
        PatternConfig optPattern = { 1, 900.0f, 0, -PI/2.0f, 0, 1, BULLET_LINEAR, 0 };
        for (int i = 0; i < player.activeOptions; i++)
        {
            ExecPattern(player.optionPos[i], optPattern, BULLET_PLAYER);
        }

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

    // Draw Options
    for (int i = 0; i < player.activeOptions; i++)
    {
        DrawCircleV((Vector2){player.optionPos[i].x + PLAY_AREA_X_OFFSET, player.optionPos[i].y + PLAY_AREA_Y_OFFSET}, 5.0f, ORANGE);
        DrawCircleLines(player.optionPos[i].x + PLAY_AREA_X_OFFSET, player.optionPos[i].y + PLAY_AREA_Y_OFFSET, 6.0f, WHITE);
    }

    DrawCircleV(screenPos, player.radius, RED);
    
    // Draw Hitbox (visual feedback for shift/focus)
    if (player.focused)
    {
        DrawCircleV(screenPos, 3.0f, WHITE);
    }
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
