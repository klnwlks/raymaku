/**********************************************************************************************
*
*   raylib - Advance Game template
*
*   Title Screen Functions Definitions (Init, Update, Draw, Unload)
*
**********************************************************************************************/

#include "raylib.h"
#include "screens.h"
#include "audio/audio_manager.h"
#include <math.h>

//----------------------------------------------------------------------------------
// Module Variables Definition (local)
//----------------------------------------------------------------------------------
static int framesCounter = 0;
static int finishScreen = 0;
static int menuSelected = 0;    // 0: START, 1: OPTIONS

#define MAX_TITLE_PARTICLES 80

typedef struct TitleParticle {
    Vector2 pos;
    float speed;
    float size;
    float alpha;
} TitleParticle;

static TitleParticle titleParticles[MAX_TITLE_PARTICLES] = { 0 };

//----------------------------------------------------------------------------------
// Title Screen Functions Definition
//----------------------------------------------------------------------------------

// Title Screen Initialization logic
void InitTitleScreen(void)
{
    framesCounter = 0;
    finishScreen = 0;
    menuSelected = 0;

    // Initialize monochrome background particles
    for (int i = 0; i < MAX_TITLE_PARTICLES; i++)
    {
        titleParticles[i].pos = (Vector2){ (float)GetRandomValue(0, GetScreenWidth()), (float)GetRandomValue(0, GetScreenHeight()) };
        titleParticles[i].speed = (float)GetRandomValue(50, 200)/100.0f;
        titleParticles[i].size = (float)GetRandomValue(1, 3);
        titleParticles[i].alpha = (float)GetRandomValue(2, 8)/10.0f;
    }
}

// Title Screen Update logic
void UpdateTitleScreen(void)
{
    framesCounter++;

    // Update particles (monochrome drift)
    for (int i = 0; i < MAX_TITLE_PARTICLES; i++)
    {
        titleParticles[i].pos.y += titleParticles[i].speed;
        if (titleParticles[i].pos.y > GetScreenHeight())
        {
            titleParticles[i].pos.y = -titleParticles[i].size;
            titleParticles[i].pos.x = (float)GetRandomValue(0, GetScreenWidth());
        }
    }

    if (IsKeyPressed(KEY_DOWN))
    {
        menuSelected++;
        if (menuSelected > 1) menuSelected = 0;
        PlaySoundEvent(SND_MENU_MOVE);
    }
    else if (IsKeyPressed(KEY_UP))
    {
        menuSelected--;
        if (menuSelected < 0) menuSelected = 1;
        PlaySoundEvent(SND_MENU_MOVE);
    }

    // Press enter or tap to change to GAMEPLAY screen
    if (IsKeyPressed(KEY_ENTER) || IsGestureDetected(GESTURE_TAP))
    {
        if (menuSelected == 0) finishScreen = 2;   // GAMEPLAY
        else finishScreen = 1;                     // OPTIONS

        PlaySoundEvent(SND_MENU_SELECT);
    }
}

// Title Screen Draw logic
void DrawTitleScreen(void)
{
    // Draw light background
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), RAYWHITE);

    // Draw background particles (dark on light)
    for (int i = 0; i < MAX_TITLE_PARTICLES; i++)
    {
        DrawCircleV(titleParticles[i].pos, titleParticles[i].size, Fade(BLACK, titleParticles[i].alpha * 0.15f));
    }

    // Flush left layout constants
    const int marginX = 80;
    
    // Draw Title: RAYMAKU
    const char* titleText = "RAYMAKU";
    float titleFontSize = font.baseSize * 5.0f;
    Vector2 titlePos = { (float)marginX, GetScreenHeight()*0.3f };
    
    // Black title on light background
    DrawTextEx(font, titleText, titlePos, titleFontSize, 8, BLACK);

    // Draw menu options
    const char* startText = "START GAME";
    const char* optionsText = "OPTIONS";
    float menuFontSize = 30.0f;
    
    int startY = GetScreenHeight()*0.55f;
    int optionsY = startY + 50;

    // Start Game Option
    if (menuSelected == 0)
    {
        float pulse = (sinf(framesCounter * 0.1f) + 1.0f) * 0.5f;
        Color highlight = Fade(BLACK, 0.7f + pulse * 0.3f);
        DrawText(TextFormat("> %s", startText), marginX, startY, menuFontSize, highlight);
    }
    else
    {
        DrawText(TextFormat("  %s", startText), marginX, startY, menuFontSize, LIGHTGRAY);
    }

    // Options Option
    if (menuSelected == 1)
    {
        float pulse = (sinf(framesCounter * 0.1f) + 1.0f) * 0.5f;
        Color highlight = Fade(BLACK, 0.7f + pulse * 0.3f);
        DrawText(TextFormat("> %s", optionsText), marginX, optionsY, menuFontSize, highlight);
    }
    else
    {
        DrawText(TextFormat("  %s", optionsText), marginX, optionsY, menuFontSize, LIGHTGRAY);
    }

    const char* hintText = "UP/DOWN to select - ENTER to confirm";
    DrawText(hintText, marginX, GetScreenHeight() * 0.85f, 20, GRAY);
}

// Title Screen Unload logic
void UnloadTitleScreen(void)
{
    // Nothing to unload
}

// Title Screen should finish?
int FinishTitleScreen(void)
{
    return finishScreen;
}
