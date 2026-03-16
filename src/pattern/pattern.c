#include "pattern.h"
#include "raymath.h"
#include "../bullet/bullet.h"
#include "../player/player.h"
#include <math.h>

// TODO: REFACTOR FOR MUTLI VOLLEY PATTERNS

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

// TODO: DIFFFERENT BULLET TYPES
