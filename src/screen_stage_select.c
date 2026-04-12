/**********************************************************************************************
*
*   raylib - Advance Game template
*
*   Stage Select Screen Functions Definitions (Init, Update, Draw, Unload)
*
**********************************************************************************************/

#include "raylib.h"
#include "screens.h"
#include <math.h>

//----------------------------------------------------------------------------------
// Module Variables Definition (local)
//----------------------------------------------------------------------------------
static int framesCounter = 0;
static int finishScreen = 0;
static int stageSelected = 0;    // 0: STAGE 1, 1: STAGE 2, ...

#define MAX_STAGES 5

//----------------------------------------------------------------------------------
// Stage Select Screen Functions Definition
//----------------------------------------------------------------------------------

// Stage Select Screen Initialization logic
void InitStageSelectScreen(void)
{
    framesCounter = 0;
    finishScreen = 0;
    stageSelected = 0;
}

// Stage Select Screen Update logic
void UpdateStageSelectScreen(void)
{
    framesCounter++;

    if (IsKeyPressed(KEY_DOWN))
    {
        stageSelected++;
        if (stageSelected >= MAX_STAGES) stageSelected = 0;
    }
    else if (IsKeyPressed(KEY_UP))
    {
        stageSelected--;
        if (stageSelected < 0) stageSelected = MAX_STAGES - 1;
    }

    // Press enter to select stage
    if (IsKeyPressed(KEY_ENTER))
    {
        if (stageSelected == 0) // Only stage 1 is selectable
        {
            selectedStage = stageSelected + 1;
            finishScreen = 2;   // GAMEPLAY
            PlaySound(fxCoin);
        }
    }

    if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_BACKSPACE))
    {
        finishScreen = 1;       // TITLE
    }
}

// Stage Select Screen Draw logic
void DrawStageSelectScreen(void)
{
    // Draw light background
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), RAYWHITE);

    const int marginX = 80;
    
    // Draw Title
    const char* titleText = "SELECT STAGE";
    float titleFontSize = font.baseSize * 3.0f;
    Vector2 titlePos = { (float)marginX, GetScreenHeight()*0.15f };
    DrawTextEx(font, titleText, titlePos, titleFontSize, 8, BLACK);

    // Draw stage options
    float menuFontSize = 30.0f;
    int startY = GetScreenHeight()*0.35f;

    for (int i = 0; i < MAX_STAGES; i++)
    {
        int currentY = startY + i * 60;
        bool isAvailable = (i == 0);
        const char* stageName = TextFormat("STAGE %d", i + 1);
        
        if (!isAvailable)
        {
            stageName = TextFormat("STAGE %d (LOCKED)", i + 1);
        }

        if (stageSelected == i)
        {
            float pulse = (sinf(framesCounter * 0.1f) + 1.0f) * 0.5f;
            Color highlight = isAvailable ? Fade(BLACK, 0.7f + pulse * 0.3f) : DARKGRAY;
            DrawText(TextFormat("> %s", stageName), marginX, currentY, menuFontSize, highlight);
        }
        else
        {
            Color color = isAvailable ? LIGHTGRAY : Fade(LIGHTGRAY, 0.5f);
            DrawText(TextFormat("  %s", stageName), marginX, currentY, menuFontSize, color);
        }
    }

    const char* hintText = "UP/DOWN to select - ENTER to confirm - ESC to back";
    DrawText(hintText, marginX, GetScreenHeight() * 0.85f, 20, GRAY);
}

// Stage Select Screen Unload logic
void UnloadStageSelectScreen(void)
{
    // Nothing to unload
}

// Stage Select Screen should finish?
int FinishStageSelectScreen(void)
{
    return finishScreen;
}
