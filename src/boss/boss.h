#ifndef BOSS_H
#define BOSS_H

#include "../pattern/pattern.h"
#include "raylib.h"
#include <stdbool.h>

#define MAX_SPELLS 10

typedef enum {
    BOSS_MOVE_STATIC,
    BOSS_MOVE_OSCILLATE,
    BOSS_MOVE_BOUNCE,
    BOSS_MOVE_TARGET_PLAYER
} BossMovement;

typedef struct {
    float timer;            // Time limit for this phase
    float internalTimer;    // Current time elapsed
    PatternConfig pattern;  // Bullet pattern for this phase
    const char *name;
    Vector2 startPos;       // Initial position for this phase
    int health;
    int maxHealth;
    float shootDelay;       // Delay between pattern volleys
    float lastShotTime;
    bool isSurvival;        // If true, phase only ends on timer
} SpellCard;

typedef struct {
    const char *name;
    int currPhase;
    int totalPhases;
    Vector2 pos;
    Vector2 velocity;
    bool active;
    float radius;
    BossMovement moveMode;
    SpellCard phases[MAX_SPELLS];
} Boss;

void InitBoss(void);
void SpawnBoss(Boss data);
void UpdateBoss(void);
void DrawBoss(void);
void BossHit(int damage);
Boss *GetActiveBoss(void);

#endif
