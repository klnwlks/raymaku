#include "boss.h"
#include "../pattern/pattern.h"
#include "../player/player.h"
#include "../score/score.h"
#include "../item/item.h"
#include <string.h>

static Boss currBoss = {0};

void InitBoss(void)
{
    memset(&currBoss, 0, sizeof(Boss));
    currBoss.active = false;
}

void SpawnBoss(Boss data)
{
    currBoss = data;
    currBoss.active = true;
    currBoss.currPhase = 0;
    currBoss.pos = currBoss.phases[0].startPos;
    currBoss.velocity = (Vector2){150.0f, 0.0f}; // Default horizontal speed
}

static void NextPhase(void)
{
    // Bonus for completing phase
    AddScore(1000);
    SpawnItems(10, ITEM_POINT, currBoss.pos, 100);
    SpawnItems(5, ITEM_POWER, currBoss.pos, 1);

    currBoss.currPhase++;
    if (currBoss.currPhase >= currBoss.totalPhases)
    {
        currBoss.active = false;
        return;
    }
    
    // Reset position for next phase
    currBoss.pos = currBoss.phases[currBoss.currPhase].startPos;
}

void BossHit(int damage)
{
    if (!currBoss.active) return;
    
    SpellCard *current = &currBoss.phases[currBoss.currPhase];
    if (current->isSurvival) return; // Cannot damage survival phases

    current->health -= damage;
    if (current->health <= 0)
    {
        NextPhase();
    }
}

void UpdateBoss(void)
{
    if (!currBoss.active) return;

    float dt = GetFrameTime();
    SpellCard *current = &currBoss.phases[currBoss.currPhase];
    
    // Update Timer
    current->internalTimer += dt;
    if (current->internalTimer >= current->timer)
    {
        NextPhase();
        if (!currBoss.active) return;
    }

    // Movement Logic
    switch (currBoss.moveMode)
    {
        case BOSS_MOVE_STATIC:
            break;
        case BOSS_MOVE_OSCILLATE:
            currBoss.pos.x += currBoss.velocity.x * dt;
            if (currBoss.pos.x > GetScreenWidth() - 100 || currBoss.pos.x < 100)
            {
                currBoss.velocity.x *= -1.0f;
            }
            break;
        case BOSS_MOVE_BOUNCE:
            currBoss.pos.x += currBoss.velocity.x * dt;
            currBoss.pos.y += currBoss.velocity.y * dt;
            if (currBoss.pos.x > GetScreenWidth() - 50 || currBoss.pos.x < 50) currBoss.velocity.x *= -1.0f;
            if (currBoss.pos.y > 300 || currBoss.pos.y < 50) currBoss.velocity.y *= -1.0f;
            break;
        case BOSS_MOVE_TARGET_PLAYER:
        {
            Player *p = GetPlayer();
            float targetX = p->position.x;
            float step = 100.0f * dt;
            if (currBoss.pos.x < targetX - 5.0f) currBoss.pos.x += step;
            else if (currBoss.pos.x > targetX + 5.0f) currBoss.pos.x -= step;
            break;
        }
    }

    // Shooting Logic
    if (current->internalTimer - current->lastShotTime >= current->shootDelay)
    {
        // Spawning a multi-volley pattern (single volley for now, but using the pool)
        SpawnPattern(current->pattern, BULLET_ENEMY, currBoss.pos, 1, 0.0f, 0.0f);
        current->lastShotTime = current->internalTimer;
    }
}

void DrawBoss(void)
{
    if (!currBoss.active) return;

    // Draw Boss Body
    DrawCircleV(currBoss.pos, currBoss.radius, PURPLE);
    DrawCircleLines(currBoss.pos.x, currBoss.pos.y, currBoss.radius + 2.0f, WHITE);

    // Draw Health/Time Indicator (Simple for now)
    SpellCard *current = &currBoss.phases[currBoss.currPhase];
    float ratio = current->isSurvival ? 
        (1.0f - current->internalTimer / current->timer) : 
        ((float)current->health / (float)current->maxHealth);
    
    DrawRectangle(10, 10, (GetScreenWidth() - 20) * ratio, 10, RED);
    DrawText(current->name, 10, 25, 20, RAYWHITE);
}

Boss *GetActiveBoss(void)
{
    return &currBoss;
}
