#include "enemy.h"
#include <math.h>
#include "../config.h"

#define MAX_ENEMY_COUNT 100

static Enemy enemies[MAX_ENEMY_COUNT] = { 0 };
static int enemyCount = 0;

void InitEnemyPool()
{
    enemyCount = 0;
}

void UpdateEnemyPool()
{
    float dt = GetFrameTime();
    for (int i = 0; i < enemyCount; i++)
    {
       // update position of enemies
        enemies[i].position.x += enemies[i].velocity.x * dt;
        enemies[i].position.y += enemies[i].velocity.y * dt;

// shoot 
        if (fmodf(dt, enemies[i].shootTimer) == 0)
        {
            ExecPattern(enemies[i].position, enemies[i].config, BULLET_ENEMY); 
        }

        // update if out of bounds / destroyed
        bool offScreen = (enemies[i].position.x < -50 || enemies[i].position.x > PLAY_AREA_WIDTH + 50 
                        || enemies[i].position.y < - 50 || enemies[i].position.y > PLAY_AREA_HEIGHT + 50 );
        bool isDead = (enemies[i].health <= 0);

        if (offScreen || isDead) 
        {
            enemies[i] = enemies[enemyCount - 1];
            enemyCount--;
            i--;
        }
    } 

}

void DrawEnemyPool()
{
    for (int i = 0; i < enemyCount; i++)
    {
        DrawCircleV(enemies[i].position, enemies[i].radius, RED);
    }

}

void SpawnEnemy(Vector2 pos, Vector2 vel, int health, PatternConfig pattern, float shootTimer, float radius)
{
    enemies[enemyCount].position = pos;
    enemies[enemyCount].velocity = vel;
    enemies[enemyCount].health = health;
    enemies[enemyCount].shootTimer = shootTimer;
    enemies[enemyCount].config = pattern;
    enemies[enemyCount].radius = radius;
    enemyCount++;
}


// RETURNS THE MEMORY ADDRESS OF ENEMY COUNT, AND SETS A POINTER TO HAVE THE SAME ADDRESS OF ENEMIES
Enemy *GetEnemies(int *count) 
{
    count = &enemyCount;
    return enemies;
}
