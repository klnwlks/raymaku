#include "score.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static unsigned long long int score = 0;
static unsigned long long int highscore = 0;

void InitScore(void)
{
    score = 0;
    highscore = 0;
    
    HighScore tempScores[MAX_HIGH_SCORES];
    int count = LoadHighScores(tempScores);
    if (count > 0)
    {
        highscore = tempScores[0].score;
    }
}

void AddScore(int points)
{
    score += points;
    if (score > highscore)
    {
        highscore = score;
    }

}

void SetScore(int points) 
{
    score = points;
}

unsigned long long int GetScore(void)
{
    return score;
}

unsigned long long int GetHiScore(void)
{
    return highscore;
}

int LoadHighScores(HighScore *scores)
{
    FILE *file = fopen("scores", "rb");
    if (file == NULL)
    {
        // If file doesn't exist, create it (empty)
        file = fopen("scores", "wb");
        if (file) fclose(file);
        return 0;
    }

    int count = 0;
    while (count < MAX_HIGH_SCORES && fread(&scores[count], sizeof(HighScore), 1, file) == 1)
    {
        count++;
    }

    fclose(file);
    return count;
}

void SaveHighScores(HighScore *scores, int count)
{
    FILE *file = fopen("scores", "wb");
    if (file)
    {
        fwrite(scores, sizeof(HighScore), count, file);
        fclose(file);
    }
}

int IsHighScore(unsigned long long int score, HighScore *scores, int count)
{
    if (score == 0) return 0;
    if (count < MAX_HIGH_SCORES) return 1;
    
    for (int i = 0; i < count; i++)
    {
        if (score > scores[i].score) return 1;
    }
    
    return 0;
}

void AddHighScore(HighScore *scores, int *count, HighScore newScore)
{
    if (*count < MAX_HIGH_SCORES)
    {
        scores[*count] = newScore;
        (*count)++;
    }
    else
    {
        // Replace the lowest score if it's better
        if (newScore.score > scores[*count - 1].score)
        {
            scores[*count - 1] = newScore;
        }
    }

    // Simple bubble sort to keep scores in descending order
    for (int i = 0; i < *count - 1; i++)
    {
        for (int j = 0; j < *count - i - 1; j++)
        {
            if (scores[j].score < scores[j + 1].score)
            {
                HighScore temp = scores[j];
                scores[j] = scores[j + 1];
                scores[j + 1] = temp;
            }
        }
    }
}
