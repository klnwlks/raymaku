#ifndef BULLET_H
#define BULLET_H

#include "raylib.h"

// Bullet owner types
typedef enum {
    BULLET_PLAYER,
    BULLET_ENEMY
} BulletOwner;

// Bullet behavior types
typedef enum {
    BULLET_LINEAR,  // Standard straight-line movement
    BULLET_HOMING,  // Tracks player position
    BULLET_CURVING, // Constant angular velocity (spiral)
    BULLET_FREEZE,  // Spawns, stops, re-aims, then fires
    BULLET_BOMB     // Clears enemy bullets on contact
} BulletBehavior;

// Bullet state for complex behaviors (like freeze and aim)
typedef enum {
    STATE_ACTIVE,   // Normal movement/update logic
    STATE_FROZEN,   // Movement halted, waiting for timer
    STATE_LOCKED    // Targeting complete, ready for launch
} BulletState;

// Optimized Bullet structure (no active flag needed with swapping)
typedef struct {
    Vector2 position;
    Vector2 velocity;
    Vector2 acceleration;
    float radius;
    int power;
    BulletOwner owner;
    BulletBehavior behavior;
    BulletState state;
    float timer;
    float speed;
    float rotationSpeed;
    bool grazed;
} Bullet;

// Lifecycle functions
void InitBulletPools(void);
void UpdateBulletPools(void);
void DrawBulletPools(void);

// Spawning
void SpawnBullet(Vector2 pos, Vector2 vel, Vector2 accel, int power, BulletOwner owner, BulletBehavior behavior, float speed, float rotationSpeed);
Bullet *GetBulletPool(int *count, BulletOwner owner);
void RemoveBullet(int index, BulletOwner owner);

#endif
