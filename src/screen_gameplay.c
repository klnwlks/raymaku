/**********************************************************************************************
*
*   raylib - Advance Game template
*
*   Gameplay Screen Functions Definitions (Init, Update, Draw, Unload)
*
*   Copyright (c) 2014-2022 Ramon Santamaria (@raysan5)
*
*   This software is provided "as-is", without any express or implied warranty. In no event
*   will the authors be held liable for any damages arising from the use of this software.
*
*   Permission is granted to anyone to use this software for any purpose, including commercial
*   applications, and to alter it and redistribute it freely, subject to the following restrictions:
*
*     1. The origin of this software must not be misrepresented; you must not claim that you
*     wrote the original software. If you use this software in a product, an acknowledgment
*     in the product documentation would be appreciated but is not required.
*
*     2. Altered source versions must be plainly marked as such, and must not be misrepresented
*     as being the original software.
*
*     3. This notice may not be removed or altered from any source distribution.
*
**********************************************************************************************/

#include "raylib.h"
#include "screens.h"
#include "config.h"
#include "player/player.h"
#include "boss/boss.h"
#include "collision/collision.h"
#include "bullet/bullet.h"
#include "enemy/enemy.h"
#include "pattern/pattern.h"
#include "score/score.h"
#include "item/item.h"
#include "stage/stage.h"

//----------------------------------------------------------------------------------
// Module Variables Definition (local)
//----------------------------------------------------------------------------------
static int framesCounter = 0;
static int finishScreen = 0;
static bool levelPaused = false;
static float survivalTimer = 0.0f;

//----------------------------------------------------------------------------------
// Gameplay Screen Functions Definition
//----------------------------------------------------------------------------------

// Gameplay Screen Initialization logic
void InitGameplayScreen(void)
{
    framesCounter = 0;
    finishScreen = 0;
    levelPaused = false;
    survivalTimer = 0.0f;
    
    InitScore();
    InitItems();
    InitPlayer();
    InitEnemyPool();
    InitBoss();
    InitBulletPools();
    
    // Start Stage 1
    Stage1();
}

// Gameplay Screen Update logic
void UpdateGameplayScreen(void)
{
    if (IsKeyPressed(KEY_P)) levelPaused = !levelPaused;
    if (levelPaused) return;

    framesCounter++;

    UpdateSpawn();
    UpdatePlayer();
    UpdateEnemyPool();
    UpdateBoss();
    UpdatePatterns();
    UpdateBulletPools();
    UpdateItems();
    
    ResolveCollisions();

    // Survival points: 100 points per second alive
    survivalTimer += GetFrameTime();
    if (survivalTimer >= 0.1f)
    {
        AddScore(1);
        survivalTimer -= 0.1f;
    }

    // Check for game over
    if (GetPlayer()->lives < 0) finishScreen = 1;

}

// Helper to draw UI
static void DrawUI(void)
{
    int uiX = PLAY_AREA_X_OFFSET + PLAY_AREA_WIDTH + 20;
    int uiY = 40;
    
    DrawText("SCORE", uiX, uiY, 20, BLACK);
    DrawText(TextFormat("%020llu", GetScore()), uiX, uiY + 25, 20, BLACK);
    
    DrawText("HI-SCORE", uiX, uiY + 60, 20, BLACK);
    DrawText(TextFormat("%020llu", GetHiScore()), uiX, uiY + 85, 20, BLACK);
    
    Player *p = GetPlayer();
    DrawText("LIVES", uiX, uiY + 130, 20, BLACK);
    for (int i = 0; i < p->lives; i++) DrawCircle(uiX + 15 + i * 25, uiY + 160, 10, RED);
    
    DrawText("BOMBS", uiX, uiY + 190, 20, BLACK);
    for (int i = 0; i < p->bombs; i++) DrawCircle(uiX + 15 + i * 25, uiY + 220, 10, GREEN);

    DrawText("POWER", uiX, uiY + 250, 20, BLACK);
    DrawText(TextFormat("%d", p->power), uiX, uiY + 275, 20, BLUE);

    DrawText("POINTS", uiX, uiY + 310, 20, BLACK);
    DrawText(TextFormat("%d / 200", p->points), uiX, uiY + 335, 20, PINK);

    DrawText("GRAZE", uiX, uiY + 370, 20, BLACK);
    DrawText(TextFormat("%d", p->graze), uiX, uiY + 395, 20, DARKPURPLE);

    DrawText("RAYMAKU", uiX, uiY + 450, 40, BLACK);
}

// Gameplay Screen Draw logic
void DrawGameplayScreen(void)
{
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), WHITE);
    
    // main play area background
    DrawRectangle(PLAY_AREA_X_OFFSET, PLAY_AREA_Y_OFFSET, PLAY_AREA_WIDTH, PLAY_AREA_HEIGHT, ColorAlpha(DARKGRAY, 0.3f));
    DrawRectangleLines(PLAY_AREA_X_OFFSET, PLAY_AREA_Y_OFFSET, PLAY_AREA_WIDTH, PLAY_AREA_HEIGHT, GRAY);

    // Draw game elements within play area (clipping would be nice but simple offset for now)
    DrawPlayer();
    DrawEnemyPool();
    DrawBoss();
    DrawItems();
    DrawBulletPools();

    // UI
    DrawUI();

    if (levelPaused)
    {
        DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.5f));
        DrawText("PAUSED", GetScreenWidth()/2 - MeasureText("PAUSED", 40)/2, GetScreenHeight()/2 - 40, 40, WHITE);
    }
}

// Gameplay Screen Unload logic
void UnloadGameplayScreen(void)
{
    UnloadPlayer();
    ClearStage();
}

// Gameplay Screen should finish?
int FinishGameplayScreen(void)
{
    return finishScreen;
}
