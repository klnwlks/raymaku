#include "stage.h"
#include "../enemy/enemy.h"

#include <stdio.h>
#define MAX_SPAWN 100

// SAWA NAKO SAS POOL
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
    // peek into front
    // check first if empty
    if(spawnQueue.front == -1)
    {
        return;
    }
    while (spawnQueue.front != -1 && timer >= spawnQueue.queue[spawnQueue.front].spawnTime)
    {
        // shorthand so to spare myself the pity of typing that 7 times
        // TODO: rewrite spawnenemy as accepting of enemydata
        StageSpawn *enem = &spawnQueue.queue[spawnQueue.front];
        SpawnEnemy(enem->pos, enem->data.vel, enem->data.health, enem->data.config, enem->data.shootTimer, enem->data.radius, enem->data.angularVelocity, enem->data.lifeTime);
        // spawn enemy and dequeue
        // if empty reset pointers
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

// unload enemy data here, reset queue
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

// enqueue to spawnQueue 
void Spawn(Vector2 pos, EnemyData data, float spawnTime)
{
    // if full
    if (spawnQueue.rear == MAX_SPAWN - 1)
    {
        printf("WARNING: STAGE QUEUE is full, enqueue is ignored!\n");
        return;
    }
    // if empty
    if (spawnQueue.front == -1) 
    {
        spawnQueue.front = 0;
    }
    spawnQueue.rear++;
    spawnQueue.queue[spawnQueue.rear] = (StageSpawn) {data, pos, spawnTime};
}

// actual stage definitions go here
// this will load EnemyData, initialize queue, call Spawn with apt enemy data
void Stage1(void)
{
    ClearStage();
}
