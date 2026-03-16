#ifndef PATTERN_H
#define PATTERN_H

#include "raylib.h"
#include "../bullet/bullet.h"

typedef struct {
    int bulletCount; // how many bullets in one shot
    float speed;
    int arc; // in rad
    int angleOffset; // starting direction
    int spin; // if it will spin
    int power; // attack power
} PatternConfig;

// for multivolley
typedef struct {
    PatternConfig config;
    BulletOwner owner;
    Vector2 origin;
    int shots; // number of shots
    float timer; // internal timer to keep track of frametime
    float shotDelay; // delay in seconds between shots
} PatternInstance; // will be used for a pool

void ExecPattern(Vector2 pos, PatternConfig pat, BulletOwner owner);
float AimPlayer(Vector2 pos);
void UpdatePatterns(void);
void SpawnPattern(PatternConfig config, BulletOwner owner, Vector2 pos, int shots, float timer, float shotDelay);

#endif
