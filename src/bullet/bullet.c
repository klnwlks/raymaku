#include "bullet.h"
#include "../config.h"
#include <stdbool.h>

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
static void UpdateBulletArray(Bullet *bullets, int *count, int max)
{
    float dt = GetFrameTime();
    
    for (int i = 0; i < *count; i++)
    {
        // 1. Update Physics
        bullets[i].velocity.x += bullets[i].acceleration.x * dt;
        bullets[i].velocity.y += bullets[i].acceleration.y * dt;
        
        bullets[i].position.x += bullets[i].velocity.x * dt;
        bullets[i].position.y += bullets[i].velocity.y * dt;

        // 2. Boundary Check (with 100px buffer)
        bool offScreen = (bullets[i].position.x < -100 || 
                          bullets[i].position.x > PLAY_AREA_WIDTH + 100 ||
                          bullets[i].position.y < -100 || 
                          bullets[i].position.y > PLAY_AREA_HEIGHT + 100);

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
    UpdateBulletArray(playerBullets, &playerBulletCount, MAX_PLAYER_BULLETS);
    UpdateBulletArray(enemyBullets, &enemyBulletCount, MAX_ENEMY_BULLETS);
}

void DrawBulletPools(void)
{
    // Draw Player Bullets (Blue)
    for (int i = 0; i < playerBulletCount; i++)
    {
        DrawCircle(playerBullets[i].position.x + PLAY_AREA_X_OFFSET, playerBullets[i].position.y + PLAY_AREA_Y_OFFSET, playerBullets[i].radius, SKYBLUE);
    }

    // Draw Enemy Bullets (Red)
    for (int i = 0; i < enemyBulletCount; i++)
    {
        DrawCircle(enemyBullets[i].position.x + PLAY_AREA_X_OFFSET, enemyBullets[i].position.y + PLAY_AREA_Y_OFFSET, enemyBullets[i].radius, RED);
    }
}

void SpawnBullet(Vector2 pos, Vector2 vel, Vector2 accel, int power, BulletOwner owner)
{
    if (owner == BULLET_PLAYER)
    {
        if (playerBulletCount < MAX_PLAYER_BULLETS)
        {
            playerBullets[playerBulletCount].position = pos;
            playerBullets[playerBulletCount].velocity = vel;
            playerBullets[playerBulletCount].acceleration = accel;
            playerBullets[playerBulletCount].radius = 4.0f;
            playerBullets[playerBulletCount].power = power;
            playerBulletCount++;
        }
    }
    else
    {
        if (enemyBulletCount < MAX_ENEMY_BULLETS)
        {
            enemyBullets[enemyBulletCount].position = pos;
            enemyBullets[enemyBulletCount].velocity = vel;
            enemyBullets[enemyBulletCount].acceleration = accel;
            enemyBullets[enemyBulletCount].radius = 6.0f;
            enemyBullets[enemyBulletCount].power = 1;
            enemyBulletCount++;
        }
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
