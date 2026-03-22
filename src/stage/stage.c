#include "stage.h"
#include "../enemy/enemy.h"
#include "../config.h"
#include <stdio.h>

#define MAX_SPAWN 500

// this is used to optimize checking for events to run
typedef struct {
    StageSpawn queue[MAX_SPAWN];
    int front;
    int rear;
} SpawnQueue;

static SpawnQueue spawnQueue;
static float timer = 0;

void UpdateSpawn(void)
{
    float dt = GetFrameTime();
    timer += dt;
    
    // check if empty
    if(spawnQueue.front == -1) return;

    while (spawnQueue.front != -1 && timer >= spawnQueue.queue[spawnQueue.front].spawnTime)
    {
        StageSpawn *item = &spawnQueue.queue[spawnQueue.front];
        
        if (item->type == SPAWN_ENEMY)
        {
            SpawnEnemy(item->pos, item->data.enemy.vel, item->data.enemy.health, item->data.enemy.config, item->data.enemy.shootTimer, item->data.enemy.radius, item->data.enemy.angularVelocity, item->data.enemy.lifeTime);
        }
        else if (item->type == SPAWN_BOSS)
        {
            SpawnBoss(item->data.boss);
        }
        
        // dequeue
        if (spawnQueue.front == spawnQueue.rear) 
        {
            spawnQueue.front = -1;
            spawnQueue.rear = -1;
        }
        else 
        {
            spawnQueue.front++;
        }
    }
}

void ClearStage(void) 
{
    for (int i = 0; i < MAX_SPAWN; i++)
    {
        spawnQueue.queue[i] = (StageSpawn){0};
    }
    timer = 0;
    spawnQueue.front = -1;
    spawnQueue.rear = -1;
}

void Spawn(Vector2 pos, EnemyData data, float spawnTime)
{
    if (spawnQueue.rear == MAX_SPAWN - 1)
    {
        printf("WARNING: STAGE QUEUE is full!\n");
        return;
    }
    if (spawnQueue.front == -1) spawnQueue.front = 0;
    spawnQueue.rear++;
    
    spawnQueue.queue[spawnQueue.rear].type = SPAWN_ENEMY;
    spawnQueue.queue[spawnQueue.rear].data.enemy = data;
    spawnQueue.queue[spawnQueue.rear].pos = pos;
    spawnQueue.queue[spawnQueue.rear].spawnTime = spawnTime;
}

void SpawnBossInQueue(Boss boss, float spawnTime)
{
    if (spawnQueue.rear == MAX_SPAWN - 1)
    {
        printf("WARNING: STAGE QUEUE is full!\n");
        return;
    }
    if (spawnQueue.front == -1) spawnQueue.front = 0;
    spawnQueue.rear++;
    
    spawnQueue.queue[spawnQueue.rear].type = SPAWN_BOSS;
    spawnQueue.queue[spawnQueue.rear].data.boss = boss;
    spawnQueue.queue[spawnQueue.rear].pos = (Vector2){0, 0}; // Bosses usually have their own pos or startPos in spellcards
    spawnQueue.queue[spawnQueue.rear].spawnTime = spawnTime;
}

void Stage1(void)
{
    ClearStage();

}
