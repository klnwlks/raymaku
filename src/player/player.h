#ifndef PLAYER_H
#define PLAYER_H

#include "raylib.h"
#include <stdbool.h>

// player struct
typedef struct {
    Vector2 position;
    Vector2 speed;
    int lives;
    int bombs;
    int power;
    float radius; // hitbox
    int points;
    int iframes;
    bool shooting;
} Player;

// function prototypes for player movement and shooting
void InitPlayer(void);
void UpdatePlayer(void);
void DrawPlayer(void);
void UnloadPlayer(void);
Player *GetPlayer(void);

#endif
