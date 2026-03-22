#ifndef PLAYER_H
#define PLAYER_H

#include "raylib.h"
#include <stdbool.h>
#include "../pattern/pattern.h"

// player struct
typedef struct {
    Vector2 position;
    Vector2 speed;
    int lives;
    int bombs;
    int power;
    float radius; // hitbox
    float invincibilityTimer; // internal timer
    int points;
    float invincibility;
    bool shooting;
    PatternConfig pattern;
    float shootTimer;
    float currentShootTimer;
} Player;

// function prototypes for player movement and shooting
void InitPlayer(void);
void UpdatePlayer(void);
void DrawPlayer(void);
void UnloadPlayer(void);
Player *GetPlayer(void);
void PlayerHit(void);

#endif
