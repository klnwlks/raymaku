#include "collision.h"
#include "../bullet/bullet.h"
#include "../enemy/enemy.h"
#include "../player/player.h"
#include "../item/item.h"
#include "../boss/boss.h"
#include "raylib.h"

void ResolveCollisions()
{
    // iterate
    // first off, the player
    int playerBulletCount;
    Bullet *playerPool = GetBulletPool(&playerBulletCount, BULLET_PLAYER);
    
    // get list of enemies
    int enemyCount;
    Enemy *enemyPool = GetEnemies(&enemyCount);
    Boss *activeBoss = GetActiveBoss();

    // iterate off player bullets and compare against enemies and boss
    for (int i = 0; i < playerBulletCount; i++) 
    {
        bool hit = false;
        // check boss collision
        if (activeBoss->active)
        {
            if (CheckCollisionCircles(playerPool[i].position, playerPool[i].radius, activeBoss->pos, activeBoss->radius))
            {
                BossHit(playerPool[i].power);
                RemoveBullet(i, BULLET_PLAYER);
                playerPool = GetBulletPool(&playerBulletCount, BULLET_PLAYER);
                i--;
                hit = true;
            }
        }
        
        if (hit) continue;

        for (int j = 0; j < enemyCount; j++) 
        {
            // if enemy has been hit
            if (CheckCollisionCircles(playerPool[i].position, playerPool[i].radius, enemyPool[j].position, enemyPool[j].radius)) 
            {
               EnemyHit(j, playerPool[i].power); 
               RemoveBullet(i, BULLET_PLAYER);
               playerPool = GetBulletPool(&playerBulletCount, BULLET_PLAYER);
               enemyPool = GetEnemies(&enemyCount); // enemy status might change
               i--;
               break;
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
            bulletPool = GetBulletPool(&enemyBulletCount, BULLET_ENEMY);
            i--;
        }

    }

    // then check if player has touched items
    int itemCount;
    Item *itemPool = GetItemPool(&itemCount);
    for (int i = 0; i < itemCount; i++)
    {
        if (CheckCollisionCircles(itemPool[i].pos, 8.0f, player->position, player->pickupRadius))
        {
            RemoveItem(i, itemPool[i].type);
            itemPool = GetItemPool(&itemCount);
            i--;
        }
    }
}
