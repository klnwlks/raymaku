#include "bullet.h"
#include "../config.h"
#include "../player/player.h"
#include "../enemy/enemy.h"
#include "../boss/boss.h"
#include "raymath.h"
#include <stdbool.h>
#include <stddef.h>
#include <float.h>

#define MAX_PLAYER_BULLETS 500
#define MAX_ENEMY_BULLETS 5000

// Pools and counters
static Bullet playerBullets[MAX_PLAYER_BULLETS] = { 0 };
static Bullet enemyBullets[MAX_ENEMY_BULLETS] = { 0 };

static int playerBulletCount = 0;
static int enemyBulletCount = 0;

// Initialize pools
void InitBulletPools(void)
{
    playerBulletCount = 0;
    enemyBulletCount = 0;
}

// Internal helper for updating and swapping
static void UpdateBulletArray(Bullet *bullets, int *count)
{
    float dt = GetFrameTime();
    Player *player = GetPlayer();
    
    int enemyCount = 0;
    Enemy *enemyPool = GetEnemies(&enemyCount);
    Boss *boss = GetActiveBoss();

    for (int i = 0; i < *count; i++)
    {
        Bullet *b = &bullets[i];
        
        // 1. Update based on behavior
        switch (b->behavior)
        {
            case BULLET_LINEAR:
                // Normal physics
                b->velocity.x += b->acceleration.x * dt;
                b->velocity.y += b->acceleration.y * dt;
                break;

            case BULLET_HOMING:
                {
                    Vector2 targetPos = { 0, 0 };
                    bool targetFound = false;

                    if (b->owner == BULLET_ENEMY)
                    {
                        targetPos = player->position;
                        targetFound = true;
                    }
                    else
                    {
                        // Find nearest enemy OR BOSS
                        float minDist = FLT_MAX;
                        
                        // Check Boss first (if active)
                        if (boss != NULL && boss->active)
                        {
                            minDist = Vector2Distance(b->position, boss->pos);
                            targetPos = boss->pos;
                            targetFound = true;
                        }

                        // Check Enemies
                        for (int j = 0; j < enemyCount; j++)
                        {
                            float dist = Vector2Distance(b->position, enemyPool[j].position);
                            if (dist < minDist)
                            {
                                minDist = dist;
                                targetPos = enemyPool[j].position;
                                targetFound = true;
                            }
                        }
                        
                        // If no target found, move slightly towards top of screen
                        if (!targetFound)
                        {
                            targetPos = (Vector2){ b->position.x, -100.0f };
                            targetFound = true;
                        }
                    }

                    if (targetFound)
                    {
                        Vector2 toTarget = Vector2Subtract(targetPos, b->position);
                        float targetAngle = atan2f(toTarget.y, toTarget.x);
                        float currentAngle = atan2f(b->velocity.y, b->velocity.x);
                        
                        // Normalize angles and get the difference
                        float angleDiff = targetAngle - currentAngle;
                        while (angleDiff > PI) angleDiff -= 2.0f * PI;
                        while (angleDiff < -PI) angleDiff += 2.0f * PI;
                        
                        // Rotate velocity vector toward target
                        float rotation = b->rotationSpeed * dt;
                        if (fabsf(angleDiff) < rotation)
                        {
                            currentAngle = targetAngle;
                        }
                        else
                        {
                            currentAngle += (angleDiff > 0) ? rotation : -rotation;
                        }
                        
                        b->velocity.x = cosf(currentAngle) * b->speed;
                        b->velocity.y = sinf(currentAngle) * b->speed;
                    }
                }
                break;

            case BULLET_CURVING:
                {
                    float currentAngle = atan2f(b->velocity.y, b->velocity.x);
                    currentAngle += b->rotationSpeed * dt;
                    
                    b->velocity.x = cosf(currentAngle) * b->speed;
                    b->velocity.y = sinf(currentAngle) * b->speed;
                }
                break;

            case BULLET_FREEZE:
                {
                    b->timer -= dt;
                    
                    if (b->state == STATE_ACTIVE)
                    {
                        if (b->timer <= 0.0f)
                        {
                            b->state = STATE_FROZEN;
                            b->velocity = (Vector2){ 0, 0 };
                            b->timer = 0.5f; // Freeze duration
                        }
                    }
                    else if (b->state == STATE_FROZEN)
                    {
                        if (b->timer <= 0.0f)
                        {
                            b->state = STATE_LOCKED;
                            // Re-aim at player
                            Vector2 toPlayer = Vector2Subtract(player->position, b->position);
                            float angle = atan2f(toPlayer.y, toPlayer.x);
                            b->velocity.x = cosf(angle) * b->speed;
                            b->velocity.y = sinf(angle) * b->speed;
                            b->state = STATE_ACTIVE; // Back to active movement
                            b->behavior = BULLET_LINEAR; // Keep moving straight after re-aim
                        }
                    }
                }
                break;
                
            case BULLET_BOMB:
                {
                    b->timer -= dt;
                    if (b->timer <= 0.0f)
                    {
                        // Remove bullet by swapping with last and decrementing count
                        bullets[i] = bullets[*count - 1];
                        (*count)--;
                        i--; // Re-process this index
                        continue;
                    }
                }
                break;
        }
        
        // 2. Apply movement
        b->position.x += b->velocity.x * dt;
        b->position.y += b->velocity.y * dt;

        // 3. Boundary Check (with 100px buffer)
        bool offScreen = (b->position.x < -100 || 
                          b->position.x > PLAY_AREA_WIDTH + 100 ||
                          b->position.y < -100 || 
                          b->position.y > PLAY_AREA_HEIGHT + 100);

        if (offScreen)
        {
            // SWAP with the last active bullet and decrement count
            bullets[i] = bullets[*count - 1];
            (*count)--;
            i--; // Re-process this index
        }
    }
}

void UpdateBulletPools(void)
{
    UpdateBulletArray(playerBullets, &playerBulletCount);
    UpdateBulletArray(enemyBullets, &enemyBulletCount);
}

void DrawBulletPools(void)
{
    // Draw Player Bullets (Blue)
    for (int i = 0; i < playerBulletCount; i++)
    {
        // Don't draw if outside play area
        if (playerBullets[i].position.x < 0 || playerBullets[i].position.x > PLAY_AREA_WIDTH ||
            playerBullets[i].position.y < 0 || playerBullets[i].position.y > PLAY_AREA_HEIGHT) continue;

        if (playerBullets[i].behavior == BULLET_BOMB)
        {
            // Bomb looks like an expanding/pulsing glowing ball
            DrawCircleGradient(playerBullets[i].position.x + PLAY_AREA_X_OFFSET, playerBullets[i].position.y + PLAY_AREA_Y_OFFSET, playerBullets[i].radius * 2.0f, WHITE, PURPLE);
        }
        else
        {
            DrawCircle(playerBullets[i].position.x + PLAY_AREA_X_OFFSET, playerBullets[i].position.y + PLAY_AREA_Y_OFFSET, playerBullets[i].radius, SKYBLUE);
        }
    }

    // Draw Enemy Bullets (Red)
    for (int i = 0; i < enemyBulletCount; i++)
    {
        // Don't draw if outside play area
        if (enemyBullets[i].position.x < 0 || enemyBullets[i].position.x > PLAY_AREA_WIDTH ||
            enemyBullets[i].position.y < 0 || enemyBullets[i].position.y > PLAY_AREA_HEIGHT) continue;

        DrawCircle(enemyBullets[i].position.x + PLAY_AREA_X_OFFSET, enemyBullets[i].position.y + PLAY_AREA_Y_OFFSET, enemyBullets[i].radius, RED);
    }
}

void SpawnBullet(Vector2 pos, Vector2 vel, Vector2 accel, int power, BulletOwner owner, BulletBehavior behavior, float speed, float rotationSpeed)
{
    Bullet *b = NULL;
    
    if (owner == BULLET_PLAYER)
    {
        if (playerBulletCount < MAX_PLAYER_BULLETS)
        {
            b = &playerBullets[playerBulletCount];
            playerBulletCount++;
        }
    }
    else
    {
        if (enemyBulletCount < MAX_ENEMY_BULLETS)
        {
            b = &enemyBullets[enemyBulletCount];
            enemyBulletCount++;
        }
    }
    
    if (b != NULL)
    {
        b->position = pos;
        b->velocity = vel;
        b->acceleration = accel;
        b->radius = (owner == BULLET_PLAYER) ? 4.0f : 6.0f;
        
        if (behavior == BULLET_BOMB)
        {
            b->radius = 12.0f;
            b->timer = 2.0f; // Bomb bullets last for 2 seconds
        }
        else
        {
            b->timer = (behavior == BULLET_FREEZE) ? 0.3f : 0.0f; // Time to move before freeze
        }

        b->power = power;
        b->owner = owner;
        b->behavior = behavior;
        b->state = STATE_ACTIVE;
        b->speed = (speed == 0.0f) ? Vector2Length(vel) : speed;
        b->rotationSpeed = rotationSpeed;
        b->grazed = false;
    }
}

void RemoveBullet(int index, BulletOwner owner)
{
    if (owner == BULLET_ENEMY) 
    {
        enemyBullets[index] = enemyBullets[enemyBulletCount - 1];
        enemyBulletCount--;
    }
    else 
    {
        playerBullets[index] = playerBullets[playerBulletCount - 1];
        playerBulletCount--;
    }
}


void ClearEnemyBullets(void)
{
    enemyBulletCount = 0;
}

Bullet *GetBulletPool(int *count, BulletOwner owner) {
    if (owner == BULLET_ENEMY) 
    {
        *count = enemyBulletCount;
        return enemyBullets;
    }
    else {
        *count = playerBulletCount;
        return playerBullets;
    }
}
