#ifndef PLAYER_H
#define PLAYER_H

#include "raylib.h"
#include <stdbool.h>
#include "../pattern/pattern.h"

#define MAX_OPTIONS 4

// player struct
typedef struct {
    Vector2 position;
    Vector2 speed;
    int lives;
    int bombs;
    int power;
    float radius; // hitbox
    float pickupRadius; // item collection
    float grazeRadius; // graze
    int graze; // graze counter
    float invincibilityTimer; // internal timer
    int points;
    float invincibility;
    bool shooting;
    bool focused; // holding shift
    PatternConfig pattern;
    float shootTimer;
    float currentShootTimer;
    
    // Options (drones)
    Vector2 optionPos[MAX_OPTIONS];
    int activeOptions;
} Player;

// function prototypes for player movement and shooting
void InitPlayer(void);
void UpdatePlayer(void);
void DrawPlayer(void);
void UnloadPlayer(void);
Player *GetPlayer(void);
void PlayerHit(void);

#endif
