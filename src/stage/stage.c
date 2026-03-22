#include "stage.h"
#include "../enemy/enemy.h"
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
        StageSpawn *enem = &spawnQueue.queue[spawnQueue.front];
        SpawnEnemy(enem->pos, enem->data.vel, enem->data.health, enem->data.config, enem->data.shootTimer, enem->data.radius, enem->data.angularVelocity, enem->data.lifeTime);
        
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
    spawnQueue.queue[spawnQueue.rear] = (StageSpawn) {data, pos, spawnTime};
}

void Stage1(void)
{
    ClearStage();

    // ENEMY DATA DEFINITIONS
    EnemyData fodder = { 5, (PatternConfig){ 1, 350.0f, 0, PI/2.0f, 0, 1, BULLET_LINEAR, 0 }, 1.2f, 15.0f, (Vector2){ 0, 120.0f }, 0.0f, 1.2f, 10.0f };
    EnemyData curvedLeft = { 10, (PatternConfig){ 2, 250.0f, 0.2f, PI/2.0f, 0, 1, BULLET_LINEAR, 0 }, 0.8f, 18.0f, (Vector2){ 100.0f, 40.0f }, 0.6f, 0.8f, 12.0f };
    EnemyData curvedRight = { 10, (PatternConfig){ 2, 250.0f, 0.2f, PI/2.0f, 0, 1, BULLET_LINEAR, 0 }, 0.8f, 18.0f, (Vector2){ -100.0f, 40.0f }, -0.6f, 0.8f, 12.0f };
    EnemyData tank = { 50, (PatternConfig){ 5, 200.0f, PI/3.0f, PI/2.0f, 0, 1, BULLET_LINEAR, 0 }, 1.5f, 25.0f, (Vector2){ 0, 40.0f }, 0.0f, 1.5f, 20.0f };
    EnemyData crossShooter = { 15, (PatternConfig){ 4, 300.0f, PI/2.0f, 0, 0, 1, BULLET_LINEAR, 0 }, 0.6f, 20.0f, (Vector2){ 0, 80.0f }, 0.0f, 0.6f, 15.0f };
    EnemyData elite = { 100, (PatternConfig){ 12, 400.0f, 2*PI, 0, 0, 1, BULLET_LINEAR, 0 }, 2.0f, 30.0f, (Vector2){ 0, 30.0f }, 0.0f, 2.0f, 30.0f };

    // WAVE 1: 0s - 10s (Opening Fodder)
    for (int i = 0; i < 10; i++)
    {
        float x = (i % 2 == 0) ? 100 : 500;
        Spawn((Vector2){ x, -20 }, fodder, 1.0f + i * 0.8f);
    }

    // WAVE 2: 12s - 20s (Curved Entrances)
    for (int i = 0; i < 6; i++)
    {
        Spawn((Vector2){ 20, -20 }, curvedLeft, 12.0f + i * 1.0f);
        Spawn((Vector2){ 580, -20 }, curvedRight, 12.0f + i * 1.0f);
    }

    // WAVE 3: 22s - 30s (Tanks and Cross Shooters)
    Spawn((Vector2){ 300, -30 }, tank, 22.0f);
    for (int i = 0; i < 4; i++)
    {
        Spawn((Vector2){ 150 + i * 100, -20 }, crossShooter, 25.0f + i * 1.2f);
    }

    // WAVE 4: 32s - 40s (Fodder Stream)
    for (int i = 0; i < 20; i++)
    {
        float x = 50 + (i * 25) % 500;
        Spawn((Vector2){ x, -20 }, fodder, 32.0f + i * 0.4f);
    }

    // WAVE 5: 42s (Mid-stage Elite)
    Spawn((Vector2){ 300, -40 }, elite, 42.0f);

    // WAVE 6: 45s - 55s (Mixed Curved and Cross)
    for (int i = 0; i < 8; i++)
    {
        if (i % 2 == 0) Spawn((Vector2){ 50, -20 }, curvedLeft, 45.0f + i * 1.0f);
        else Spawn((Vector2){ 550, -20 }, curvedRight, 45.0f + i * 1.0f);
        Spawn((Vector2){ 300, -20 }, crossShooter, 46.0f + i * 1.2f);
    }

    // WAVE 7: 60s - 70s (The Wall)
    for (int i = 0; i < 3; i++)
    {
        Spawn((Vector2){ 150 + i * 150, -30 }, tank, 60.0f + i * 2.0f);
    }
    for (int i = 0; i < 15; i++)
    {
        Spawn((Vector2){ GetRandomValue(50, 550), -20 }, fodder, 62.0f + i * 0.5f);
    }

    // WAVE 8: 72s - 78s (Final Rush)
    for (int i = 0; i < 10; i++)
    {
        Spawn((Vector2){ 100, -20 }, curvedLeft, 72.0f + i * 0.5f);
        Spawn((Vector2){ 500, -20 }, curvedRight, 72.0f + i * 0.5f);
    }

    // WAVE 9: 80s (Double Elites before Boss?)
    Spawn((Vector2){ 150, -40 }, elite, 80.0f);
    Spawn((Vector2){ 450, -40 }, elite, 80.0f);
}
