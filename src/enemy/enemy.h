#ifndef ENEMY_H
#define ENEMY_H

#include "raylib.h"
#include "../pattern/pattern.h"

typedef struct {
    Vector2 position;
    Vector2 velocity;
    int health;
    PatternConfig config;
    float shootTimer; // delay between shots 
    float radius;
    // TODO: add support for loading textures
} Enemy;

void InitEnemyPool(void);
void UpdateEnemyPool(void);
void DrawEnemyPool(void);
void SpawnEnemy(Vector2 pos, Vector2 vel, int health, PatternConfig pattern, float shootTimer, float radius);
Enemy *GetEnemies(int* count);
void EnemyHit(int index, int power);

#endif
