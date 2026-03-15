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
    // TODO: Initialize GAMEPLAY screen variables here!
    framesCounter = 0;
    finishScreen = 0;
    levelPaused = false;
    InitPlayer();
}

// Gameplay Screen Update logic
void UpdateGameplayScreen(void)
{
    framesCounter++;
    if (IsKeyPressed(KEY_P)) levelPaused = !levelPaused;

    // if guard
    if (levelPaused) return;

    UpdatePlayer();

    // TODO: Update GAMEPLAY screen variables here!
    // NOTE: to streamline development, we will use relative positions inside the rectangle square to streamline thinigs

    // Press enter or tap to change to ENDING screen
    if (IsKeyPressed(KEY_ENTER) || IsGestureDetected(GESTURE_TAP))
    {
        finishScreen = 1;
        PlaySound(fxCoin);
    }
}

// Gameplay Screen Draw logic
void DrawGameplayScreen(void)
{
    // TODO: Draw GAMEPLAY screen here!
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), PURPLE);
    
    // main play area
    DrawRectangle(PLAY_AREA_X_OFFSET, PLAY_AREA_Y_OFFSET, PLAY_AREA_WIDTH, PLAY_AREA_HEIGHT, WHITE);
    DrawRectangleLines(PLAY_AREA_X_OFFSET, PLAY_AREA_Y_OFFSET, PLAY_AREA_WIDTH, PLAY_AREA_HEIGHT, BLACK);

    DrawPlayer();
    
    // DrawText(TextFormat("%020lu", score), 40, 20, 40, BLACK);

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
