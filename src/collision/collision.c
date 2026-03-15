#include "collision.h"
#include "../bullet/bullet.h"
#include "../enemy/enemy.h"
#include "../player/player.h"

void ResolveCollisions()
{
    // iterate
    // first off, the player
    int playerBulletCount;
    Bullet *playerPool = GetBulletPool(&playerBulletCount, BULLET_PLAYER);
    
    // get list of enemies
    int enemyCount;
    Enemy *enemyPool = GetEnemies(&enemyCount);

    // iterate off player bullets and compare against enemies
    for (int i = 0; i < playerBulletCount; i++) 
    {
        for (int j = 0; i < enemyCount; i++) 
        {
            // if enemy has been hit
            if (CheckCollisionCircles(playerPool[i].position, playerPool[i].radius, enemyPool[j].position, enemyPool[j].radius)) 
            {
               EnemyHit(j, playerPool[i].power); 
               RemoveBullet(i, BULLET_PLAYER);
            }
        }
    }

    // now we iterate off enemy bullets
    int enemyBulletCount;
    Player *player = GetPlayer();
    Bullet *bulletPool = GetBulletPool(&enemyBulletCount, BULLET_ENEMY);
    for (int i = 0; i < enemyBulletCount; i++)
    {
        if (CheckCollisionCircles(bulletPool[i].position, bulletPool[i].radius, player->position, player->radius))
        {
            PlayerHit();
            RemoveBullet(i, BULLET_ENEMY);
        }

    }
}
