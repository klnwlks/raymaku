#include "pattern.h"
#include "raylib.h"
#include "raymath.h"
#include "../bullet/bullet.h"
#include "../player/player.h"
#include <math.h>

void ExecPattern(Vector2 pos, PatternConfig pat, BulletOwner owner)
{
    float rad = pat.arc * (M_PI / 180);
    float startAngle = pat.angleOffset - (rad / 2.0f);
    float step = (pat.bulletCount > 1) ? (rad / (pat.bulletCount - 1)) : 0;
    int i = 0;

    
    while (i < pat.shots)
    {
      if (( (int) GetFrameTime() / pat.delay ) != 0 ) continue;
      for (int j = 0; j < pat.bulletCount; j++)
      {
        float finalAngle = startAngle + (step * i);
        Vector2 velocity = {
            cosf(finalAngle) * pat.speed,
            sinf(finalAngle) * pat.speed
        };
          
        SpawnBullet(pos, velocity, (Vector2){0,0}, owner);
      }
      i++;
    }
}

float AimPlayer(Vector2 pos)
{
    Vector2 direction = Vector2Subtract(GetPlayer()->position, pos);
    return atan2f(direction.y, direction.x);
}
