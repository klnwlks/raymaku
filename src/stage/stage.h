#ifndef STAGE_H
#define STAGE_H
#include "../enemy/enemy.h"
#include "../boss/boss.h"
#include "raylib.h"

typedef enum {
    SPAWN_ENEMY,
    SPAWN_BOSS
} SpawnType;

typedef struct {
    SpawnType type;
    union {
        EnemyData enemy;
        Boss boss;
    } data;
    Vector2 pos;
    float spawnTime;
} StageSpawn;

void Stage1(void);
void UpdateSpawn(void);
void ClearStage(void);
// add to queue 
void Spawn(Vector2 pos, EnemyData data, float spawnTime);
void SpawnBossInQueue(Boss boss, float spawnTime);
#endif 
