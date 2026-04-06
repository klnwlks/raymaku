#include "player.h"
#include "raylib.h"
#include "raymath.h"
#include "config.h"
#include "../item/item.h"

static Player player = {0};

void InitPlayer()
{
    // init player values
    player.position = (Vector2) { PLAY_AREA_WIDTH / 2.0f, PLAY_AREA_HEIGHT * 0.8f};
    player.speed = (Vector2) {4.0f, 4.0f};
    player.radius = 3.0f; // Small death hitbox
    player.pickupRadius = 12.0f; // Generous item pickup radius
    player.lives = 3;
    player.bombs = 5;
    player.points = 0;
    player.invincibility = 2.5f; // Increased for recovery
    player.invincibilityTimer = 0.0f;
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
        if (player.focused) currSpeed /= 1.5;

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
        float distFactor = ((float) (i / 2) + 1);
        player.optionPos[i].x = player.position.x + (angle * spread * distFactor);
        player.optionPos[i].y = player.position.y - heightOffset + (player.focused ? 0 : 10.0f * distFactor);
    }

    // Update main pattern based on power and focus
    if (player.power < 40)
    {
        // Basic 4-way spread
        float arc = player.focused ? 0.1f : 0.4f;
        player.pattern = (PatternConfig){ 4, 1000.0f, arc, -PI/2.0f, 0, 1, BULLET_LINEAR, 0 };
    }
    else
    {
        // Advanced 5-way spread (tighter than 4-way when focused)
        float arc = player.focused ? 0.08f : 0.5f;
        player.pattern = (PatternConfig){ 5, 1200.0f, arc, -PI/2.0f, 0, 1, BULLET_LINEAR, 0 };
    }

    // Shooting logic
    player.shooting = IsKeyDown(KEY_Z) || IsKeyDown(KEY_SPACE);
    
    // Bomb logic
    if (IsKeyPressed(KEY_X) && player.bombs > 0)
    {
        player.bombs--;
        player.invincibilityTimer = 3.0f; // 3 seconds of invincibility during bomb
        
        // Complex big pattern: Multi-volley expanding circle
        PatternConfig bombPattern = { 32, 400.0f, 2.0f * PI, 0, PI, 10, BULLET_BOMB, 0 };
        SpawnPattern(bombPattern, BULLET_PLAYER, player.position, 12, 0, 0.15f);
    }
    
    if (player.currentShootTimer > 0.0f) player.currentShootTimer -= GetFrameTime();

    if (player.shooting && player.currentShootTimer <= 0.0f)
    {
        // Shoot from main player
        ExecPattern(player.position, player.pattern, BULLET_PLAYER);
        
        // Shoot from active options (Homing bullets)
        // rotationSpeed PI * 4 means it turns 720 degrees per second
        PatternConfig optPattern = { 1, 800.0f, 0, -PI/2.0f, 0, 1, BULLET_HOMING, PI * 4.0f };
        for (int i = 0; i < player.activeOptions; i++)
        {
            ExecPattern(player.optionPos[i], optPattern, BULLET_PLAYER);
        }

        player.currentShootTimer = player.shootTimer;
    }
}

void DrawPlayer()
{
    // Blinking effect if invincible
    if (player.invincibilityTimer > 0.0f)
    {
        if (((int)(player.invincibilityTimer * 10)) % 2 == 0) return;
    }

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

    DrawCircleV(screenPos, player.radius + 7.0f, RED);
    DrawCircleLines(screenPos.x, screenPos.y, player.radius + 8.0f, WHITE);
    
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
        
        // Drop some power
        int dropAmount = player.power / 2; // Drop 50%
        if (dropAmount > 10) dropAmount = 10; // Max 10 items
        if (dropAmount < 1 && player.power > 0) dropAmount = 1;

        if (dropAmount > 0)
        {
            SpawnItems(dropAmount, ITEM_POWER, player.position, 1);
            player.power -= dropAmount;
            if (player.power < 0) player.power = 0;
        }
        // Revive in bottom center
        player.position = (Vector2){ PLAY_AREA_WIDTH / 2.0f, PLAY_AREA_HEIGHT * 0.9f };
        player.invincibilityTimer = player.invincibility;
    }
}

void UnloadPlayer() 
{

}

Player *GetPlayer(void) {
    return &player;
}
