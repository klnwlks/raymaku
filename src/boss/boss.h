#ifndef BOSS_H
#define BOSS_H

#include "../pattern/pattern.h"
#include "raylib.h"
#include <stdbool.h>

#define MAX_SPELLS 10
#define MAX_BOSS_OPTIONS 4

typedef enum {
    BOSS_MOVE_STATIC,
    BOSS_MOVE_OSCILLATE,
    BOSS_MOVE_BOUNCE,
    BOSS_MOVE_TARGET_PLAYER
} BossMovement;

typedef enum {
    BOSS_OPTION_MOVE_STATIC,
    BOSS_OPTION_MOVE_ROTATE
} BossOptionMovement;

typedef struct {
    bool active;
    Vector2 offset;             // Base offset from boss
    float angle;                // Current angle (for rotation)
    BossOptionMovement moveMode;
    float moveSpeed;            // Rotation speed in radians/sec
    
    PatternConfig pattern;
    float shootDelay;
    float lastShotTime;
    int maxShots;               // Destroyed after this many shots (if > 0)
    int shotsFired;
    int volleyShots;
    float volleyDelay;
    
    Vector2 currentPos;         // Computed current position
} BossOption;

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
    int volleyShots;        // Number of shots in a multi-volley
    float volleyDelay;      // Delay between shots in a multi-volley
    BossMovement moveMode;  // Movement style for this phase
    BossOption options[MAX_BOSS_OPTIONS]; // Boss options/drones
} SpellCard;

typedef struct {
    const char *name;
    int currPhase;
    int totalPhases;
    Vector2 pos;
    Vector2 velocity;
    bool active;
    float radius;
    SpellCard phases[MAX_SPELLS];
} Boss;

void InitBoss(void);
void SpawnBoss(Boss data);
void UpdateBoss(void);
void DrawBoss(void);
void BossHit(int damage);
Boss *GetActiveBoss(void);

#endif
