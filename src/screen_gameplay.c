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

//----------------------------------------------------------------------------------
// Module Variables Definition (local)
//----------------------------------------------------------------------------------
static int framesCounter = 0;
static int finishScreen = 0;
static bool levelPaused = false;

//----------------------------------------------------------------------------------
// Gameplay Screen Functions Definition
//----------------------------------------------------------------------------------

// Gameplay Screen Initialization logic
void InitGameplayScreen(void)
{
    framesCounter = 0;
    finishScreen = 0;
    levelPaused = false;
    
    InitPlayer();
    InitEnemyPool();
    InitBoss();
    InitBulletPools();
}

// Gameplay Screen Update logic
void UpdateGameplayScreen(void)
{
    if (IsKeyPressed(KEY_P)) levelPaused = !levelPaused;
    if (levelPaused) return;

    framesCounter++;

    UpdatePlayer();
    UpdateEnemyPool();
    UpdateBoss();
    UpdatePatterns();
    UpdateBulletPools();
    
    ResolveCollisions();

    // TEMPORARY: Spawn a test boss with B key
    if (IsKeyPressed(KEY_B))
    {
        Boss testBoss = {0};
        testBoss.name = "REAVER";
        testBoss.active = true;
        testBoss.radius = 40.0f;
        testBoss.totalPhases = 2;
        testBoss.moveMode = BOSS_MOVE_OSCILLATE;

        // Phase 1: Simple Circle
        testBoss.phases[0].name = "VOID CIRCLE";
        testBoss.phases[0].health = 500;
        testBoss.phases[0].maxHealth = 500;
        testBoss.phases[0].timer = 30.0f;
        testBoss.phases[0].shootDelay = 1.0f;
        testBoss.phases[0].startPos = (Vector2){ GetScreenWidth()/2, 150 };
        testBoss.phases[0].pattern = (PatternConfig){ 16, 200.0f, 2.0f * PI, 0, 0, 1, BULLET_LINEAR, 0 };

        // Phase 2: Survival Spiral
        testBoss.phases[1].name = "DESPERATION SPIRAL";
        testBoss.phases[1].isSurvival = true;
        testBoss.phases[1].timer = 15.0f;
        testBoss.phases[1].shootDelay = 0.1f;
        testBoss.phases[1].startPos = (Vector2){ GetScreenWidth()/2, 150 };
        testBoss.phases[1].pattern = (PatternConfig){ 1, 300.0f, 0, 0, 10.0f, 1, BULLET_LINEAR, 0 };

        SpawnBoss(testBoss);
    }

    if (IsKeyPressed(KEY_ENTER))
    {
        finishScreen = 1;
        PlaySound(fxCoin);
    }
}

// Gameplay Screen Draw logic
void DrawGameplayScreen(void)
{
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), BLACK);
    
    // main play area
    DrawRectangle(PLAY_AREA_X_OFFSET, PLAY_AREA_Y_OFFSET, PLAY_AREA_WIDTH, PLAY_AREA_HEIGHT, DARKGRAY);

    DrawPlayer();
    DrawEnemyPool();
    DrawBoss();
    DrawBulletPools();

    if (levelPaused)
    {
        DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.5f));
        DrawText("PAUSED", GetScreenWidth()/2 - MeasureText("PAUSED", 40)/2, GetScreenHeight()/2 - 40, 40, WHITE);
    }
}

// Gameplay Screen Unload logic
void UnloadGameplayScreen(void)
{
    // TODO: Unload GAMEPLAY screen variables here!
    UnloadPlayer();
}

// Gameplay Screen should finish?
int FinishGameplayScreen(void)
{
    return finishScreen;
}
