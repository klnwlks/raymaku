#include "pattern.h"
#include "raymath.h"
#include "../bullet/bullet.h"
#include "../player/player.h"
#include <math.h>

#define MAX_PATTERNS 64

static PatternInstance patternPool[MAX_PATTERNS];
static int activePatterns = 0;

// this will execute the pattern
void ExecPattern(Vector2 pos, PatternConfig pat, BulletOwner owner)
{
    // get "arc" (how spread out or bullet will be) 
    float rad = pat.arc;
    // get the starting angle of out shot
    float startAngle = pat.angleOffset - (rad / 2.0f);
    // step is the degree between shots
    float step = (pat.bulletCount > 1) ? (rad / (pat.bulletCount - 1)) : 0;

    
    // spawn bullets
    for (int j = 0; j < pat.bulletCount; j++)
    {
        // move their firing angle
        float finalAngle = startAngle + (step * j);
        // get vector velocity
        Vector2 velocity = {
            cosf(finalAngle) * pat.speed,
            sinf(finalAngle) * pat.speed
        };
      
        // spawn the bullet
        SpawnBullet(pos, velocity, (Vector2){0,0}, pat.power,  owner);
    }
}

// returns the radian value of the direction from initial vector to player vector
float AimPlayer(Vector2 pos)
{
    Vector2 direction = Vector2Subtract(GetPlayer()->position, pos);
    return atan2f(direction.y, direction.x);
}

// used for multivolley patterns, we run an internal update loop to keep track of delay between shots
// then we update patterns accordingly
void UpdatePatterns()
{
    float dt = GetFrameTime();
    for (int i = 0; i < activePatterns; i++)
    {
        patternPool[i].timer += dt;
        if (patternPool[i].timer >= patternPool[i].shotDelay) 
        {
            ExecPattern(patternPool[i].origin, patternPool[i].config, patternPool[i].owner);
            patternPool[i].shots--;
            patternPool[i].timer = 0;

            if (patternPool[i].shots <= 0)
            {
                patternPool[i] = patternPool[activePatterns - 1];
                activePatterns--;
                i--;
            }
        }
    }

}

// create new pattern and add to pool
void SpawnPattern(PatternConfig config, BulletOwner owner, Vector2 pos, int shots, float timer, float shotDelay)
{
    patternPool[activePatterns].config = config;
    patternPool[activePatterns].owner = owner;
    patternPool[activePatterns].origin = pos;
    patternPool[activePatterns].shots = shots;
    patternPool[activePatterns].timer = timer;
    patternPool[activePatterns].shotDelay = shotDelay;
    activePatterns++;
}
