/**********************************************************************************************
*
*   raylib - Advance Game template
*
*   Options Screen Functions Definitions (Init, Update, Draw, Unload)
*
**********************************************************************************************/

#include "raylib.h"
#include "screens.h"

//----------------------------------------------------------------------------------
// Module Variables Definition (local)
//----------------------------------------------------------------------------------
static int framesCounter = 0;
static int finishScreen = 0;

//----------------------------------------------------------------------------------
// Options Screen Functions Definition
//----------------------------------------------------------------------------------

// Options Screen Initialization logic
void InitOptionsScreen(void)
{
    framesCounter = 0;
    finishScreen = 0;
}

// Options Screen Update logic
void UpdateOptionsScreen(void)
{
    framesCounter++;

    // Press enter or tap to return to TITLE screen
    if (IsKeyPressed(KEY_ENTER) || IsGestureDetected(GESTURE_TAP) || IsKeyPressed(KEY_ESCAPE))
    {
        finishScreen = 1;
        PlaySound(fxCoin);
    }
}

// Options Screen Draw logic
void DrawOptionsScreen(void)
{
    // Draw light background
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), RAYWHITE);

    const int marginX = 80;

    const char* titleText = "OPTIONS";
    float titleFontSize = font.baseSize * 3.0f;
    DrawTextEx(font, titleText, (Vector2){ (float)marginX, GetScreenHeight()*0.15f }, titleFontSize, 4, BLACK);

    DrawText("AUDIO: ENABLED", marginX, GetScreenHeight()*0.4f, 20, LIGHTGRAY);
    DrawText("INPUT: KEYBOARD", marginX, GetScreenHeight()*0.5f, 20, LIGHTGRAY);
    
    const char* backText = "> BACK TO TITLE";
    DrawText(backText, marginX, GetScreenHeight()*0.75f, 30, BLACK);
    
    DrawText("PRESS ENTER or ESC to GO BACK", marginX, GetScreenHeight()*0.85f, 20, GRAY);
}

// Options Screen Unload logic
void UnloadOptionsScreen(void)
{
    // Nothing to unload
}

// Options Screen should finish?
int FinishOptionsScreen(void)
{
    return finishScreen;
}