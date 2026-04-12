#ifndef SCORE_H
#define SCORE_H

#define MAX_HIGH_SCORES 10

typedef struct HighScore {
    char name[16];
    unsigned long long int score;
    char date[32]; // MM/DD/YYYY + null terminator
} HighScore;

void InitScore(void);
void AddScore(int points);
void SetScore(int points);
unsigned long long int GetScore(void);
unsigned long long int GetHiScore(void);

// High score management
int LoadHighScores(HighScore *scores);
void SaveHighScores(HighScore *scores, int count);
int IsHighScore(unsigned long long int score, HighScore *scores, int count);
void AddHighScore(HighScore *scores, int *count, HighScore newScore);

#endif
