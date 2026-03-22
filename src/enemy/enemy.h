#ifndef ENEMY_H
#define ENEMY_H

#include "raylib.h"
#include "../pattern/pattern.h"

// Enemy struct with lifetime
typedef struct {
    Vector2 position;
    Vector2 velocity;
    int health;
    PatternConfig config;
    float shootTimer; // delay between shots 
    float radius;
    float angularVelocity; // radian rotation applied to velocity vector
    float currentShootTimer;
    float lifeTime; // how long they stay on screen
    float currentLifeTime;
    // TODO: add support for loading textures
} Enemy;

// same struct as Enemy, but this will be used for stage to load enemies beforehand
// no pos 
typedef struct {
    int health;
    PatternConfig config;
    float shootTimer; // delay between shots 
    float radius;
    Vector2 vel; // velocity
    float angularVelocity; // radian rotation applied to velocity vector
    float currentShootTimer;
    float lifeTime;
    // TODO: loading textures / spritesheets
} EnemyData;

void InitEnemyPool(void);
void UpdateEnemyPool(void);
void DrawEnemyPool(void);
void SpawnEnemy(Vector2 pos, Vector2 vel, int health, PatternConfig pattern, float shootTimer, float radius, float angularVelocity, float lifeTime);
Enemy *GetEnemies(int* count);
void EnemyHit(int index, int power);
float GetAngleToPlayer(Vector2 pos);
Vector2 GetDirToPlayer(Vector2 pos);

#endif
