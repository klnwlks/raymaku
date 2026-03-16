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
    int power;
} PatternConfig;

void ExecPattern(Vector2 pos, PatternConfig pat, BulletOwner owner);
float AimPlayer(Vector2 pos);

#endif
