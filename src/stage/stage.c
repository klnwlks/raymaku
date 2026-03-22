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
            SpawnEnemy(item->pos, item->data.enemy.vel, item->data.enemy.health, item->data.enemy.config, item->data.enemy.shootTimer, item->data.enemy.radius, item->data.enemy.angularVelocity, item->data.enemy.lifeTime, item->data.enemy.volleyShots, item->data.enemy.volleyDelay);
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

    // Base Patterns
    PatternConfig patBasic = {1, 250.0f, 0, PI/2, 0, 10, BULLET_LINEAR, 0};
    PatternConfig patAimed = {1, 200.0f, 0, PI/2, 0, 10, BULLET_FREEZE, 0}; // Starts straight, freezes, aims
    PatternConfig patSpread = {3, 200.0f, PI/4, PI/2, 0, 10, BULLET_LINEAR, 0};
    PatternConfig patRing = {8, 150.0f, PI*2, 0, 0, 10, BULLET_LINEAR, 0};
    PatternConfig patCurving = {2, 150.0f, PI/6, PI/2, 0, 10, BULLET_CURVING, 1.5f}; // curves outward
    
    // Base Enemies (EnemyData)
    // Fodder 1 (Straight, basic shot)
    EnemyData fodder1 = {10, patBasic, 1.0f, 15.0f, (Vector2){0, 120}, 0.0f, 1.0f, 10.0f, 1, 0.0f};

    // Fodder 2 (Aimed, slower, NOW WITH 3-SHOT VOLLEY)
    EnemyData fodder2 = {15, patAimed, 1.5f, 15.0f, (Vector2){0, 80}, 0.0f, 1.0f, 15.0f, 3, 0.15f};

    // Sweeper (Moves horizontally)
    EnemyData sweeperR = {20, patSpread, 1.0f, 20.0f, (Vector2){100, 10}, 0.0f, 0.5f, 12.0f, 1, 0.0f};
    EnemyData sweeperL = {20, patSpread, 1.0f, 20.0f, (Vector2){-100, 10}, 0.0f, 0.5f, 12.0f, 1, 0.0f};

    // Tank (Slow, Ring burst, NOW WITH 2-SHOT VOLLEY)
    EnemyData tank = {100, patRing, 2.0f, 30.0f, (Vector2){0, 30}, 0.0f, 2.0f, 20.0f, 2, 0.2f};

    // Curve flyway (Flies in, curves away using angularVelocity, 4-SHOT VOLLEY)
    EnemyData curvedFlyerR = {15, patCurving, 1.5f, 15.0f, (Vector2){50, 150}, 0.5f, 1.0f, 15.0f, 4, 0.1f};
    EnemyData curvedFlyerL = {15, patCurving, 1.5f, 15.0f, (Vector2){-50, 150}, -0.5f, 1.0f, 15.0f, 4, 0.1f};

    // Timing Variables
    float t = 2.0f; // Start at 2 seconds

    // Wave 1: Simple Fodder 1 (0:02 - 0:15)
    for (int i = 0; i < 5; i++) {
        Spawn((Vector2){100 + i * 100, -20}, fodder1, t);
        t += 1.0f;
    }
    t += 3.0f;
    for (int i = 0; i < 5; i++) {
        Spawn((Vector2){500 - i * 100, -20}, fodder1, t);
        t += 1.0f;
    }
    t += 3.0f;

    // Wave 2: Sweepers (0:15 - 0:30)
    for (int i = 0; i < 3; i++) {
        Spawn((Vector2){-20, 50 + i * 40}, sweeperR, t);
        t += 1.5f;
    }
    t += 2.0f;
    for (int i = 0; i < 3; i++) {
        Spawn((Vector2){620, 50 + i * 40}, sweeperL, t);
        t += 1.5f;
    }
    t += 4.0f;

    // Wave 3: Curved Flyers + Tank (0:30 - 0:50)
    Spawn((Vector2){300, -30}, tank, t);
    t += 2.0f;
    for (int i = 0; i < 4; i++) {
        Spawn((Vector2){100, -20}, curvedFlyerR, t);
        Spawn((Vector2){500, -20}, curvedFlyerL, t);
        t += 1.5f;
    }
    t += 5.0f;

    // Wave 4: Aimed Fodder (0:50 - 1:10)
    for (int i = 0; i < 8; i++) {
        float xPos = (i % 2 == 0) ? 150.0f : 450.0f;
        Spawn((Vector2){xPos, -20}, fodder2, t);
        t += 1.0f;
    }
    t += 4.0f;

    // Wave 5: Sweepers and Fodder 1 together (1:10 - 1:30)
    for(int i = 0; i < 4; i++) {
        Spawn((Vector2){-20, 50}, sweeperR, t);
        Spawn((Vector2){620, 100}, sweeperL, t);
        Spawn((Vector2){300, -20}, fodder1, t);
        t += 2.0f;
    }
    t += 4.0f;

    // Wave 6: Double Tank + Curved Flyers (1:30 - 1:50)
    Spawn((Vector2){200, -30}, tank, t);
    Spawn((Vector2){400, -30}, tank, t);
    t += 3.0f;
    for (int i = 0; i < 5; i++) {
        Spawn((Vector2){150, -20}, curvedFlyerR, t);
        Spawn((Vector2){450, -20}, curvedFlyerL, t);
        t += 1.5f;
    }
    t += 5.0f;

    // Wave 7: Bullet Hell Primer - lots of spread and rings (1:50 - 2:20)
    for (int i = 0; i < 10; i++) {
        float xPos = 100.0f + (GetRandomValue(0, 4) * 100.0f);
        Spawn((Vector2){xPos, -20}, fodder2, t);
        
        if (i % 3 == 0) {
            Spawn((Vector2){300, -20}, tank, t);
        }
        t += 1.5f;
    }
    t += 5.0f;

    // Wave 8: Finale before boss - Sweeper barrage (2:20 - 2:40)
    for (int i = 0; i < 6; i++) {
        Spawn((Vector2){-20, 40 + i*20}, sweeperR, t);
        Spawn((Vector2){620, 160 - i*20}, sweeperL, t);
        t += 1.0f;
    }
    t += 6.0f;

    // Wave 9: Curved flyers swarm (2:00 - 2:20)
    for (int i = 0; i < 10; i++) {
        Spawn((Vector2){100 + (i % 3) * 50, -20}, curvedFlyerR, t);
        Spawn((Vector2){500 - (i % 3) * 50, -20}, curvedFlyerL, t);
        t += 1.5f;
    }
    t += 5.0f;

    // Wave 10: Double Tanks + Fodder Rain (2:20 - 2:35)
    Spawn((Vector2){200, -30}, tank, t);
    Spawn((Vector2){400, -30}, tank, t);
    t += 2.0f;
    for (int i = 0; i < 12; i++) {
        Spawn((Vector2){50 + (i % 6) * 100, -20}, fodder1, t);
        t += 1.0f;
    }
    t += 5.0f;

    // Wave 11: The true finale (2:35 - 2:50)
    Spawn((Vector2){300, -30}, tank, t);
    for (int i = 0; i < 5; i++) {
        Spawn((Vector2){-20, 50 + i*20}, sweeperR, t);
        Spawn((Vector2){620, 150 - i*20}, sweeperL, t);
        t += 2.0f;
    }
    t += 8.0f; // Gap before boss (Wait until ~2:50)

    // Ensure we are near 170-180 seconds for the Boss
    if (t < 170.0f) {
        t = 170.0f; 
    }

    // BOSS
    Boss boss = {0};
    boss.name = "Mecha-Spider Core";
    boss.totalPhases = 3;
    boss.radius = 40.0f;

    // Phase 1: Spread and Aim (Bouncing)
    PatternConfig bossPat1 = {5, 200.0f, PI/2, PI/2, 0.5f, 10, BULLET_LINEAR, 0};
    boss.phases[0] = (SpellCard){
        .timer = 40.0f,
        .internalTimer = 0.0f,
        .pattern = bossPat1,
        .name = "Phase 1: Spread Volley",
        .startPos = (Vector2){300, 100},
        .health = 800,
        .maxHealth = 800,
        .shootDelay = 1.5f,
        .lastShotTime = 0.0f,
        .isSurvival = false,
        .volleyShots = 5,
        .volleyDelay = 0.1f,
        .moveMode = BOSS_MOVE_BOUNCE
    };

    // Phase 2: Ring bursts and curving (Oscillating)
    PatternConfig bossPat2 = {16, 150.0f, PI*2, 0, -0.2f, 10, BULLET_CURVING, 0.5f};
    boss.phases[1] = (SpellCard){
        .timer = 40.0f,
        .internalTimer = 0.0f,
        .pattern = bossPat2,
        .name = "Phase 2: Spiral Death",
        .startPos = (Vector2){300, 150},
        .health = 1000,
        .maxHealth = 1000,
        .shootDelay = 2.0f,
        .lastShotTime = 0.0f,
        .isSurvival = false,
        .volleyShots = 3,
        .volleyDelay = 0.2f,
        .moveMode = BOSS_MOVE_OSCILLATE
    };

    // Phase 3: Freeze/Aim spam (Survival) (Targets Player)
    PatternConfig bossPat3 = {3, 300.0f, PI/4, PI/2, 1.0f, 10, BULLET_FREEZE, 0};
    boss.phases[2] = (SpellCard){
        .timer = 30.0f,
        .internalTimer = 0.0f,
        .pattern = bossPat3,
        .name = "Phase 3: Target Locked",
        .startPos = (Vector2){300, 100},
        .health = 1500,
        .maxHealth = 1500,
        .shootDelay = 1.0f,
        .lastShotTime = 0.0f,
        .isSurvival = true,
        .volleyShots = 8,
        .volleyDelay = 0.08f,
        .moveMode = BOSS_MOVE_TARGET_PLAYER
    };

    SpawnBossInQueue(boss, t);
}
