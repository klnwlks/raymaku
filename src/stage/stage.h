#ifndef STAGE_H
#define STAGE_H
#include "../enemy/enemy.h"
#include "raylib.h"

typedef struct {
    EnemyData data; 
    Vector2 pos;
    float spawnTime;
} StageSpawn;

void Stage1(void);
void UpdateSpawn(void);
void ClearStage(void);
// add to queue 
void Spawn(Vector2 pos, EnemyData data, float spawnTime);
#endif 
