#include "score.h"
static unsigned long long int score = 0;
static unsigned long long int highscore = 0;

void InitScore(void)
{
    score = 0;
    highscore = 0;
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
