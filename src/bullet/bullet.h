#ifndef BULLET_H
#define BULLET_H

#include "raylib.h"

// Bullet owner types
typedef enum {
    BULLET_PLAYER,
    BULLET_ENEMY
} BulletOwner;

// Optimized Bullet structure (no active flag needed with swapping)
typedef struct {
    Vector2 position;
    Vector2 velocity;
    Vector2 acceleration;
    float radius;
    int power;
} Bullet;

// Lifecycle functions
void InitBulletPools(void);
void UpdateBulletPools(void);
void DrawBulletPools(void);

// Spawning
void SpawnBullet(Vector2 pos, Vector2 vel, Vector2 accel,int power, BulletOwner owner);
int *GetBulletPool(Bullet *pool, BulletOwner owner);

#endif
