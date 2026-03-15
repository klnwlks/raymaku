#ifndef ENEMY_H
#define ENEMY_H

#include "raylib.h"
#include "../pattern/pattern.h"

typedef struct {
    Vector2 position;
    Vector2 speed;
    int health;
    PatternConfig config;
    Vector2 endPosition;
    int appearanceLength;
    float shootTimer; // delay between shots 
    // TODO: add support for loading textures
} Enemy;

void InitEnemyPool(void);
void UpdateEnemyPool(void);
void DrawEnemyPool(void);
void SpawnEnemy(Vector2 pos, Vector2 vel, int health, PatternConfig pattern,
                Vector2 endpos, int appear, float shootTimer);

#endif
