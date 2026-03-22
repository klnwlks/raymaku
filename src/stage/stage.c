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

    EnemyData fodder = { 5, (PatternConfig){ 1, 300.0f, 0, PI/2.0f, 0, 1, BULLET_LINEAR, 0 }, 1.0f, 15.0f, (Vector2){ 0, 100.0f }, 0.0f, 1.0f, 10.0f };
    EnemyData curved = { 10, (PatternConfig){ 3, 200.0f, PI/4.0f, PI/2.0f, 0, 1, BULLET_LINEAR, 0 }, 0.5f, 20.0f, (Vector2){ 50.0f, 50.0f }, 0.5f, 0.5f, 15.0f };
    
    // Wave 1: Fodder from top
    for (int i = 0; i < 5; i++)
    {
        Spawn((Vector2){ 100 + i * 100, -20 }, fodder, 1.0f + i * 0.5f);
    }

    // Wave 2: Curved enemies
    Spawn((Vector2){ 50, -20 }, curved, 5.0f);
    Spawn((Vector2){ 550, -20 }, curved, 5.0f);

    // Wave 3: More fodder
    for (int i = 0; i < 8; i++)
    {
        Spawn((Vector2){ GetRandomValue(50, 550), -20 }, fodder, 10.0f + i * 0.3f);
    }
}
