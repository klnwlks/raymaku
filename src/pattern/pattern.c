#include "pattern.h"
#include "raylib.h"
#include "raymath.h"
#include "../bullet/bullet.h"
#include "../player/player.h"
#include <math.h>

// this will execute the pattern
void ExecPattern(Vector2 pos, PatternConfig pat, BulletOwner owner)
{
    // get "arc" (how spread out or bullet will be) into radians
    float rad = pat.arc * (M_PI / 180);
    // get the starting angle of out shot
    float startAngle = pat.angleOffset - (rad / 2.0f);
    // step is the degree between shots
    float step = (pat.bulletCount > 1) ? (rad / (pat.bulletCount - 1)) : 0;
    int i = 0;

    
    // for intervals of firing
    while (i < pat.shots)
    {
      
        // apply delay between firing
        if (( (int) GetFrameTime() / pat.delay ) != 0 ) continue;

        // spawn bullets
        for (int j = 0; j < pat.bulletCount; j++)
        {
            // move their firing angle
            float finalAngle = startAngle + (step * i);

            // get vector velocity
            Vector2 velocity = {
                cosf(finalAngle) * pat.speed,
                sinf(finalAngle) * pat.speed
            };
          
            // spawn the bullet
            SpawnBullet(pos, velocity, (Vector2){0,0}, owner);
        }
        i++;
    }
}

// returns the radian value of the direction from initial vector to player vector
float AimPlayer(Vector2 pos)
{
    Vector2 direction = Vector2Subtract(GetPlayer()->position, pos);
    return atan2f(direction.y, direction.x);
}
