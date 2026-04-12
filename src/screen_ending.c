/**********************************************************************************************
*
*   raylib - Advance Game template
*
*   Ending Screen Functions Definitions (Init, Update, Draw, Unload)
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
#include "score/score.h"
#include <time.h>
#include <stdio.h>
#include <string.h>

//----------------------------------------------------------------------------------
// Module Variables Definition (local)
//----------------------------------------------------------------------------------
static int framesCounter = 0;
static int finishScreen = 0;

static HighScore highScores[MAX_HIGH_SCORES] = { 0 };
static int scoreCount = 0;
static bool isNewHighScore = false;
static char playerName[16] = "\0";
static int letterCount = 0;
static bool nameSaved = false;

//----------------------------------------------------------------------------------
// Ending Screen Functions Definition
//----------------------------------------------------------------------------------

// Ending Screen Initialization logic
void InitEndingScreen(void)
{
    framesCounter = 0;
    finishScreen = 0;
    
    scoreCount = LoadHighScores(highScores);
    unsigned long long int currentScore = GetScore();
    
    isNewHighScore = IsHighScore(currentScore, highScores, scoreCount);
    
    playerName[0] = '\0';
    letterCount = 0;
    nameSaved = false;
}

// Ending Screen Update logic
void UpdateEndingScreen(void)
{
    if (isNewHighScore && !nameSaved)
    {
        // Get char pressed (unicode character) on the queue
        int key = GetCharPressed();

        // Check if more characters have been pressed on the same frame
        while (key > 0)
        {
            // NOTE: Only allow keys in range [32..125]
            if ((key >= 32) && (key <= 125) && (letterCount < 15))
            {
                playerName[letterCount] = (char)key;
                playerName[letterCount+1] = '\0'; // Add null terminator at the end of the string.
                letterCount++;
            }

            key = GetCharPressed();  // Check next character in the queue
        }

        if (IsKeyPressed(KEY_BACKSPACE))
        {
            letterCount--;
            if (letterCount < 0) letterCount = 0;
            playerName[letterCount] = '\0';
        }

        if (IsKeyPressed(KEY_ENTER) && letterCount > 0)
        {
            // Save the new high score
            time_t t = time(NULL);
            struct tm tm = *localtime(&t);
            
            HighScore newEntry;
            strcpy(newEntry.name, playerName);
            newEntry.score = GetScore();
            snprintf(newEntry.date, sizeof(newEntry.date), "%02d/%02d/%04d", tm.tm_mon + 1, tm.tm_mday, tm.tm_year + 1900);
            
            AddHighScore(highScores, &scoreCount, newEntry);
            SaveHighScores(highScores, scoreCount);
            
            nameSaved = true;
            PlaySound(fxCoin);
        }
    }
    else
    {
        // Press enter or tap to return to TITLE screen
        if (IsKeyPressed(KEY_ENTER) || IsGestureDetected(GESTURE_TAP))
        {
            finishScreen = 1;
            PlaySound(fxCoin);
        }
    }
    
    framesCounter++;
}

// Ending Screen Draw logic
void DrawEndingScreen(void)
{
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), BLACK);

    Vector2 titlePos = { GetScreenWidth()/2.0 - MeasureTextEx(font, "HIGH SCORES", font.baseSize*3.0f, 4).x/2, 50 };
    DrawTextEx(font, "HIGH SCORES", titlePos, font.baseSize*3.0f, 4, GOLD);

    // Draw table header
    float startY = 150;
    float colName = 300;
    float colScore = 600;
    float colDate = 900;
    
    DrawTextEx(font, "NAME", (Vector2){ colName, startY }, font.baseSize, 2, LIGHTGRAY);
    DrawTextEx(font, "SCORE", (Vector2){ colScore, startY }, font.baseSize, 2, LIGHTGRAY);
    DrawTextEx(font, "DATE", (Vector2){ colDate, startY }, font.baseSize, 2, LIGHTGRAY);

    for (int i = 0; i < scoreCount; i++)
    {
        Color color = (isNewHighScore && nameSaved && GetScore() == highScores[i].score) ? YELLOW : WHITE;
        
        DrawTextEx(font, highScores[i].name, (Vector2){ colName, startY + 40 + i*40 }, font.baseSize, 2, color);
        DrawTextEx(font, TextFormat("%llu", highScores[i].score), (Vector2){ colScore, startY + 40 + i*40 }, font.baseSize, 2, color);
        DrawTextEx(font, highScores[i].date, (Vector2){ colDate, startY + 40 + i*40 }, font.baseSize, 2, color);
    }

    if (isNewHighScore && !nameSaved)
    {
        DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.8f));
        
        DrawText("NEW HIGH SCORE!", GetScreenWidth()/2 - MeasureText("NEW HIGH SCORE!", 40)/2, GetScreenHeight()/2 - 100, 40, YELLOW);
        DrawText(TextFormat("SCORE: %llu", GetScore()), GetScreenWidth()/2 - MeasureText(TextFormat("SCORE: %llu", GetScore()), 30)/2, GetScreenHeight()/2 - 50, 30, WHITE);
        
        DrawText("ENTER YOUR NAME:", GetScreenWidth()/2 - MeasureText("ENTER YOUR NAME:", 20)/2, GetScreenHeight()/2, 20, LIGHTGRAY);
        
        // Draw player name input box
        DrawRectangle(GetScreenWidth()/2 - 150, GetScreenHeight()/2 + 40, 300, 50, DARKGRAY);
        DrawRectangleLines(GetScreenWidth()/2 - 150, GetScreenHeight()/2 + 40, 300, 50, LIGHTGRAY);
        
        DrawText(playerName, GetScreenWidth()/2 - MeasureText(playerName, 30)/2, GetScreenHeight()/2 + 50, 30, WHITE);
        
        if (((framesCounter/30)%2) == 0) 
            DrawText("_", GetScreenWidth()/2 + MeasureText(playerName, 30)/2 + 5, GetScreenHeight()/2 + 50, 30, WHITE);

        DrawText("PRESS ENTER TO SAVE", GetScreenWidth()/2 - MeasureText("PRESS ENTER TO SAVE", 20)/2, GetScreenHeight()/2 + 110, 20, GRAY);
    }
    else
    {
        DrawText("PRESS ENTER to RETURN to TITLE SCREEN", GetScreenWidth()/2 - MeasureText("PRESS ENTER to RETURN to TITLE SCREEN", 20)/2, GetScreenHeight() - 100, 20, DARKGRAY);
    }
}

// Ending Screen Unload logic
void UnloadEndingScreen(void)
{
    // TODO: Unload ENDING screen variables here!
}

// Ending Screen should finish?
int FinishEndingScreen(void)
{
    return finishScreen;
}
