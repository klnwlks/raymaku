#include "stage.h"
#include "../enemy/enemy.h"
#include "../config.h"
#include <stdio.h>

#define MAX_SPAWN 1000

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
    
    // Pause stage timer if a boss is active
    // This ensures no regular enemies spawn during spellcards
    if (GetActiveBoss() && GetActiveBoss()->active) return;

    timer += dt;
    
    // check if empty
    if(spawnQueue.front == -1) return;

    while (spawnQueue.front != -1 && timer >= spawnQueue.queue[spawnQueue.front].spawnTime)
    {
        StageSpawn *item = &spawnQueue.queue[spawnQueue.front];
        
        if (item->type == SPAWN_ENEMY)
        {
            SpawnEnemy(item->pos, item->data.enemy.vel, item->data.enemy.acceleration, item->data.enemy.drag, item->data.enemy.health, item->data.enemy.config, item->data.enemy.shootTimer, item->data.enemy.radius, item->data.enemy.angularVelocity, item->data.enemy.lifeTime, item->data.enemy.volleyShots, item->data.enemy.volleyDelay);
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
    spawnQueue.queue[spawnQueue.rear].pos = (Vector2){0, 0};
    spawnQueue.queue[spawnQueue.rear].spawnTime = spawnTime;
}

void Stage1(void)
{
    ClearStage();

    // --- PATTERN CONFIGURATIONS ---
    PatternConfig patAimed = { .bulletCount = 1, .speed = 300.0f, .arc = 0, .angleOffset = 0, .spin = 0, .power = 10, .behavior = BULLET_LINEAR, .rotationSpeed = 0, .jitter = 0.05f, .aimAtPlayer = true, .bulletRadius = 0 };
    PatternConfig patAimedSpread = { .bulletCount = 3, .speed = 200.0f, .arc = PI/6, .angleOffset = 0, .spin = 0, .power = 10, .behavior = BULLET_LINEAR, .rotationSpeed = 0, .jitter = 0, .aimAtPlayer = true, .bulletRadius = 0 };
    PatternConfig patCone = { .bulletCount = 7, .speed = 250.0f, .arc = PI/6, .angleOffset = PI/2, .spin = 0, .power = 10, .behavior = BULLET_LINEAR, .rotationSpeed = 0, .jitter = 0, .aimAtPlayer = false, .bulletRadius = 0 };
    PatternConfig patSpiral = { .bulletCount = 8, .speed = 180.0f, .arc = PI*2, .angleOffset = 0, .spin = 0.5f, .power = 10, .behavior = BULLET_CURVING, .rotationSpeed = 0.4f, .jitter = 0, .aimAtPlayer = false, .bulletRadius = 10.0f }; // Large bullets for tanks
    PatternConfig patFreeze = { .bulletCount = 12, .speed = 200.0f, .arc = PI*2, .angleOffset = 0, .spin = 0, .power = 10, .behavior = BULLET_FREEZE, .rotationSpeed = 0, .jitter = 0, .aimAtPlayer = false, .bulletRadius = 8.0f }; // Large bullets for tanks


    // --- ENEMY DATA ARCHETYPES ---
    EnemyData lingerer = {
        .health = 40, .radius = 20.0f,
        .config = patAimed, .shootTimer = 2.0f, .volleyShots = 10, .volleyDelay = 0.08f,
        .vel = {0, 450}, .acceleration = {30, 0}, .drag = 2.0f,
        .lifeTime = 15.0f
    };

    EnemyData sweeperR = {
        .health = 30, .radius = 18.0f,
        .config = patAimedSpread, .shootTimer = 1.5f, .volleyShots = 6, .volleyDelay = 0.12f,
        .vel = {-200, 30}, .acceleration = {0, 10}, .drag = 0.1f,
        .lifeTime = 20.0f
    };
    EnemyData sweeperL = {
        .health = 30, .radius = 18.0f,
        .config = patAimedSpread, .shootTimer = 1.5f, .volleyShots = 6, .volleyDelay = 0.12f,
        .vel = {200, 30}, .acceleration = {0, 10}, .drag = 0.1f,
        .lifeTime = 20.0f
    };

    EnemyData coneSupport = {
        .health = 50, .radius = 20.0f,
        .config = patCone, .shootTimer = 2.5f, .volleyShots = 5, .volleyDelay = 0.15f,
        .vel = {0, 200}, .acceleration = {0, 0}, .drag = 0.5f,
        .lifeTime = 15.0f
    };

    EnemyData sentry = {
        .health = 250, .radius = 35.0f,
        .config = patSpiral, .shootTimer = 3.0f, .volleyShots = 12, .volleyDelay = 0.15f,
        .vel = {0, 300}, .acceleration = {0, -60}, .drag = 1.0f,
        .lifeTime = 30.0f
    };

    EnemyData freezeSentry = {
        .health = 300, .radius = 35.0f,
        .config = patFreeze, .shootTimer = 4.0f, .volleyShots = 1, .volleyDelay = 0.0f,
        .vel = {0, 300}, .acceleration = {0, -60}, .drag = 1.0f,
        .lifeTime = 40.0f
    };

    EnemyData spiraler = {
        .health = 50, .radius = 20.0f,
        .config = patSpiral, .shootTimer = 2.5f, .volleyShots = 8, .volleyDelay = 0.1f,
        .vel = {150, 150}, .acceleration = {0, 0}, .drag = 0.0f, .angularVelocity = 0.8f,
        .lifeTime = 15.0f
    };

    float t = 2.0f;

    // --- 0:00 - 1:15: INITIAL WAVES (Ramping up density) ---
    // Start with a small squad
    for (int i = 0; i < 5; i++) {
        Spawn((Vector2){150 + i * 150, -30}, lingerer, t);
        if (i % 2 == 0) Spawn((Vector2){150 + i * 150, -60}, coneSupport, t + 0.5f);
        t += 3.0f;
    }

    // Ramp up: Sweeper pairs + Lingerers
    for (int i = 0; i < 6; i++) {
        Spawn((Vector2){830, 100 + (i % 2) * 120}, sweeperR, t);
        Spawn((Vector2){-30, 150 + (i % 2) * 120}, sweeperL, t);
        Spawn((Vector2){GetRandomValue(100, 700), -30}, lingerer, t + 1.0f);
        t += 6.0f;
    }

    // Heavy early wave
    for (int i = 0; i < 6; i++) {
        Spawn((Vector2){GetRandomValue(100, 700), -30}, coneSupport, t);
        Spawn((Vector2){GetRandomValue(100, 700), -60}, lingerer, t + 0.5f);
        t += 2.0f;
    }

    // --- 1:15 - 2:20: ESCALATION ---
    t = 75.0f;
    for (int i = 0; i < 25; i++) {
        Spawn((Vector2){GetRandomValue(100, 700), -30}, spiraler, t);
        if (i % 3 == 0) Spawn((Vector2){400, -50}, sentry, t);
        if (i % 5 == 0) {
            Spawn((Vector2){GetRandomValue(150, 350), -50}, freezeSentry, t + 1.0f);
            Spawn((Vector2){GetRandomValue(450, 650), -50}, freezeSentry, t + 1.2f);
        }
        t += 2.5f; 
    }

    // --- 2:30: MIDBOSS: SENTRY UNIT MK-I ---
    t = 150.0f;
    Boss midboss = {0};
    midboss.name = "Sentry Unit MK-I";
    midboss.totalPhases = 3;
    midboss.radius = 45.0f;
    midboss.active = true;

    // Phase 1: Data Stream
    midboss.phases[0] = (SpellCard){
        .name = "Data Stream: Sequential Feed",
        .timer = 25.0f, .health = 1500, .maxHealth = 1500,
        .startPos = (Vector2){400, 150}, .moveMode = BOSS_MOVE_OSCILLATE,
        .pattern = (PatternConfig){ .bulletCount = 3, .speed = 220.0f, .arc = PI/4, .angleOffset = PI/2, .spin = 0, .power = 10, .behavior = BULLET_LINEAR, .rotationSpeed = 0, .jitter = 0, .aimAtPlayer = false, .bulletRadius = 12.0f },
        .shootDelay = 1.8f, .volleyShots = 12, .volleyDelay = 0.08f,
        .options = {
            { .active = true, .offset = (Vector2){-100, 0}, .moveMode = BOSS_OPTION_MOVE_ROTATE, .moveSpeed = 3.0f,
              .pattern = (PatternConfig){ .bulletCount = 1, .speed = 320.0f, .arc = 0, .angleOffset = 0, .spin = 0, .power = 10, .behavior = BULLET_LINEAR, .rotationSpeed = 0, .jitter = 0, .aimAtPlayer = true, .bulletRadius = 6.0f }, .shootDelay = 2.0f, .volleyShots = 15, .volleyDelay = 0.04f },
            { .active = true, .offset = (Vector2){100, 0}, .moveMode = BOSS_OPTION_MOVE_ROTATE, .moveSpeed = -3.0f,
              .pattern = (PatternConfig){ .bulletCount = 1, .speed = 320.0f, .arc = 0, .angleOffset = 0, .spin = 0, .power = 10, .behavior = BULLET_LINEAR, .rotationSpeed = 0, .jitter = 0, .aimAtPlayer = true, .bulletRadius = 6.0f }, .shootDelay = 2.0f, .volleyShots = 15, .volleyDelay = 0.04f }
        }
    };

    midboss.phases[1] = (SpellCard){
        .name = "Recursive Loop: Buffer Stack",
        .timer = 30.0f, .health = 2000, .maxHealth = 2000,
        .startPos = (Vector2){400, 200}, .moveMode = BOSS_MOVE_STATIC,
        .pattern = (PatternConfig){ .bulletCount = 20, .speed = 240.0f, .arc = PI*2, .angleOffset = 0, .spin = 0.5f, .power = 10, .behavior = BULLET_FREEZE, .rotationSpeed = 0, .jitter = 0, .aimAtPlayer = false, .bulletRadius = 10.0f },
        .shootDelay = 3.5f, .volleyShots = 15, .volleyDelay = 0.15f
    };

    midboss.phases[2] = (SpellCard){
        .name = "Buffer Overflow: Memory Leak",
        .timer = 20.0f, .health = 2500, .maxHealth = 2500,
        .startPos = (Vector2){400, 150}, .moveMode = BOSS_MOVE_BOUNCE,
        .pattern = (PatternConfig){ .bulletCount = 11, .speed = 280.0f, .arc = PI/1.5f, .angleOffset = PI/2, .spin = 0, .power = 10, .behavior = BULLET_LINEAR, .rotationSpeed = 0, .jitter = 0.2f, .aimAtPlayer = false, .bulletRadius = 8.0f },
        .shootDelay = 1.5f, .volleyShots = 20, .volleyDelay = 0.05f
    };

    SpawnBossInQueue(midboss, t);

    // --- 3:30 - 4:45: FINAL GAUNTLET ---
    t = 210.0f; 
    for (int i = 0; i < 45; i++) {
        Spawn((Vector2){GetRandomValue(50, 750), -30}, lingerer, t);
        if (i % 2 == 0) Spawn((Vector2){GetRandomValue(100, 700), -40}, coneSupport, t + 0.5f);
        if (i % 4 == 0) Spawn((Vector2){(i % 2 == 0 ? 830 : -30), 200}, (i % 2 == 0 ? sweeperR : sweeperL), t + 1.0f);
        if (i % 5 == 0) Spawn((Vector2){400, -50}, sentry, t + 2.0f);
        t += 1.8f; 
    }

    // --- 5:00: FINAL BOSS ---
    t = 300.0f;
    Boss boss = {0};
    boss.name = "Sentry Unit MK-II: Overclocked";
    boss.totalPhases = 4;
    boss.radius = 50.0f;
    boss.active = true;

    boss.phases[0] = (SpellCard){
        .name = "Hyper-Data Stream: Overclocked Input",
        .timer = 30.0f, .health = 3500, .maxHealth = 3500,
        .startPos = (Vector2){400, 150}, .moveMode = BOSS_MOVE_OSCILLATE,
        .pattern = (PatternConfig){ .bulletCount = 7, .speed = 280.0f, .arc = PI/2.5f, .angleOffset = PI/2, .spin = 0.3f, .power = 10, .behavior = BULLET_LINEAR, .rotationSpeed = 0, .jitter = 0.05f, .aimAtPlayer = false, .bulletRadius = 14.0f },
        .shootDelay = 1.8f, .volleyShots = 20, .volleyDelay = 0.06f,
        .options = {
            { .active = true, .offset = (Vector2){-120, -20}, .moveMode = BOSS_OPTION_MOVE_ROTATE, .moveSpeed = 5.0f, .pattern = (PatternConfig){ .bulletCount = 1, .speed = 400.0f, .arc = 0, .angleOffset = 0, .spin = 0, .power = 10, .behavior = BULLET_LINEAR, .rotationSpeed = 0, .jitter = 0.02f, .aimAtPlayer = true, .bulletRadius = 8.0f }, .shootDelay = 1.8f, .volleyShots = 25, .volleyDelay = 0.02f },
            { .active = true, .offset = (Vector2){120, -20}, .moveMode = BOSS_OPTION_MOVE_ROTATE, .moveSpeed = -5.0f, .pattern = (PatternConfig){ .bulletCount = 1, .speed = 400.0f, .arc = 0, .angleOffset = 0, .spin = 0, .power = 10, .behavior = BULLET_LINEAR, .rotationSpeed = 0, .jitter = 0.02f, .aimAtPlayer = true, .bulletRadius = 8.0f }, .shootDelay = 1.8f, .volleyShots = 25, .volleyDelay = 0.02f },
            { .active = true, .offset = (Vector2){-80, 80}, .moveMode = BOSS_OPTION_MOVE_ROTATE, .moveSpeed = 5.0f, .pattern = (PatternConfig){ .bulletCount = 1, .speed = 400.0f, .arc = 0, .angleOffset = 0, .spin = 0, .power = 10, .behavior = BULLET_LINEAR, .rotationSpeed = 0, .jitter = 0.02f, .aimAtPlayer = true, .bulletRadius = 8.0f }, .shootDelay = 1.8f, .volleyShots = 25, .volleyDelay = 0.02f },
            { .active = true, .offset = (Vector2){80, 80}, .moveMode = BOSS_OPTION_MOVE_ROTATE, .moveSpeed = -5.0f, .pattern = (PatternConfig){ .bulletCount = 1, .speed = 400.0f, .arc = 0, .angleOffset = 0, .spin = 0, .power = 10, .behavior = BULLET_LINEAR, .rotationSpeed = 0, .jitter = 0.02f, .aimAtPlayer = true, .bulletRadius = 8.0f }, .shootDelay = 1.8f, .volleyShots = 25, .volleyDelay = 0.02f }
        }
    };

    boss.phases[1] = (SpellCard){
        .name = "Recursive Loop EX: Overflow Stack",
        .timer = 35.0f, .health = 4000, .maxHealth = 4000,
        .startPos = (Vector2){400, 200}, .moveMode = BOSS_MOVE_STATIC,
        .pattern = (PatternConfig){ .bulletCount = 32, .speed = 260.0f, .arc = PI*2, .angleOffset = 0, .spin = 0.8f, .power = 10, .behavior = BULLET_FREEZE, .rotationSpeed = 0, .jitter = 0, .aimAtPlayer = false, .bulletRadius = 12.0f },
        .shootDelay = 4.0f, .volleyShots = 25, .volleyDelay = 0.12f
    };

    boss.phases[2] = (SpellCard){
        .name = "Buffer Overflow: Critical Exception",
        .timer = 30.0f, .health = 5000, .maxHealth = 5000,
        .startPos = (Vector2){400, 150}, .moveMode = BOSS_MOVE_BOUNCE,
        .pattern = (PatternConfig){ .bulletCount = 15, .speed = 350.0f, .arc = PI/1.2f, .angleOffset = PI/2, .spin = 0, .power = 10, .behavior = BULLET_CURVING, .rotationSpeed = 0.8f, .jitter = 0.3f, .aimAtPlayer = false, .bulletRadius = 10.0f },
        .shootDelay = 1.2f, .volleyShots = 50, .volleyDelay = 0.03f
    };

    boss.phases[3] = (SpellCard){
        .name = "System Shutdown: Total Memory Wipe",
        .timer = 35.0f, .isSurvival = true,
        .startPos = (Vector2){400, 300}, .moveMode = BOSS_MOVE_TARGET_PLAYER,
        .pattern = (PatternConfig){ .bulletCount = 128, .speed = 180.0f, .arc = PI*2, .angleOffset = 0, .spin = 0.15f, .power = 10, .behavior = BULLET_LINEAR, .rotationSpeed = 0, .jitter = 0, .aimAtPlayer = false, .bulletRadius = 16.0f },
        .shootDelay = 2.0f, .volleyShots = 1, .volleyDelay = 0.0f,
        .options = {
            { .active = true, .offset = (Vector2){-200, 0}, .moveMode = BOSS_OPTION_MOVE_STATIC, .pattern = (PatternConfig){ .bulletCount = 7, .speed = 280.0f, .arc = PI/4, .angleOffset = PI/2, .spin = 0, .power = 10, .behavior = BULLET_LINEAR, .rotationSpeed = 0, .jitter = 0, .aimAtPlayer = true, .bulletRadius = 8.0f }, .shootDelay = 2.0f, .volleyShots = 30, .volleyDelay = 0.04f },
            { .active = true, .offset = (Vector2){200, 0}, .moveMode = BOSS_OPTION_MOVE_STATIC, .pattern = (PatternConfig){ .bulletCount = 7, .speed = 280.0f, .arc = PI/4, .angleOffset = PI/2, .spin = 0, .power = 10, .behavior = BULLET_LINEAR, .rotationSpeed = 0, .jitter = 0, .aimAtPlayer = true, .bulletRadius = 8.0f }, .shootDelay = 2.0f, .volleyShots = 30, .volleyDelay = 0.04f },
            { .active = true, .offset = (Vector2){0, -200}, .moveMode = BOSS_OPTION_MOVE_STATIC, .pattern = (PatternConfig){ .bulletCount = 7, .speed = 280.0f, .arc = PI/4, .angleOffset = PI/2, .spin = 0, .power = 10, .behavior = BULLET_LINEAR, .rotationSpeed = 0, .jitter = 0, .aimAtPlayer = true, .bulletRadius = 8.0f }, .shootDelay = 2.0f, .volleyShots = 30, .volleyDelay = 0.04f },
            { .active = true, .offset = (Vector2){0, 200}, .moveMode = BOSS_OPTION_MOVE_STATIC, .pattern = (PatternConfig){ .bulletCount = 7, .speed = 280.0f, .arc = PI/4, .angleOffset = PI/2, .spin = 0, .power = 10, .behavior = BULLET_LINEAR, .rotationSpeed = 0, .jitter = 0, .aimAtPlayer = true, .bulletRadius = 8.0f }, .shootDelay = 2.0f, .volleyShots = 30, .volleyDelay = 0.04f }
        }
    };

    SpawnBossInQueue(boss, t);
}
