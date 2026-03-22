#include "boss.h"
#include "../pattern/pattern.h"
#include "../player/player.h"
#include "../score/score.h"
#include "../item/item.h"
#include "../config.h"
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
    currBoss.velocity = (Vector2){150.0f, 100.0f}; // Default speed
}

static void NextPhase(void)
{
    // Bonus for completing phase
    AddScore(1000);
    SpawnItems(30, ITEM_POINT, currBoss.pos, 100);
    SpawnItems(15, ITEM_POWER, currBoss.pos, 1);

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

    // Movement Logic (relative to play area)
    switch (current->moveMode)
    {
        case BOSS_MOVE_STATIC:
            break;
        case BOSS_MOVE_OSCILLATE:
            currBoss.pos.x += currBoss.velocity.x * dt;
            if (currBoss.pos.x > PLAY_AREA_WIDTH - 100 || currBoss.pos.x < 100)
            {
                currBoss.velocity.x *= -1.0f;
            }
            break;
        case BOSS_MOVE_BOUNCE:
            currBoss.pos.x += currBoss.velocity.x * dt;
            currBoss.pos.y += currBoss.velocity.y * dt;
            if (currBoss.pos.x > PLAY_AREA_WIDTH - 50 || currBoss.pos.x < 50) currBoss.velocity.x *= -1.0f;
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

    // Global Clamping to Play Area
    if (currBoss.pos.x < currBoss.radius) currBoss.pos.x = currBoss.radius;
    if (currBoss.pos.x > PLAY_AREA_WIDTH - currBoss.radius) currBoss.pos.x = PLAY_AREA_WIDTH - currBoss.radius;
    if (currBoss.pos.y < currBoss.radius) currBoss.pos.y = currBoss.radius;
    if (currBoss.pos.y > PLAY_AREA_HEIGHT - currBoss.radius) currBoss.pos.y = PLAY_AREA_HEIGHT - currBoss.radius;

    // Shooting Logic
    if (current->internalTimer - current->lastShotTime >= current->shootDelay)
    {
        int shots = current->volleyShots > 0 ? current->volleyShots : 1;
        float vDelay = current->volleyDelay > 0.0f ? current->volleyDelay : 0.0f;
        SpawnPattern(current->pattern, BULLET_ENEMY, currBoss.pos, shots, 0.0f, vDelay);
        current->lastShotTime = current->internalTimer;
    }
}

void DrawBoss(void)
{
    if (!currBoss.active) return;

    Vector2 screenPos = {
        currBoss.pos.x + PLAY_AREA_X_OFFSET,
        currBoss.pos.y + PLAY_AREA_Y_OFFSET
    };

    // Draw Boss Body
    DrawCircleV(screenPos, currBoss.radius, PURPLE);
    DrawCircleLines(screenPos.x, screenPos.y, currBoss.radius + 2.0f, WHITE);

    // Draw Health/Time Indicator
    SpellCard *current = &currBoss.phases[currBoss.currPhase];
    float ratio = current->isSurvival ? 
        (1.0f - current->internalTimer / current->timer) : 
        ((float)current->health / (float)current->maxHealth);
    
    DrawRectangle(PLAY_AREA_X_OFFSET, 10, (PLAY_AREA_WIDTH) * ratio, 10, RED);
    DrawText(current->name, PLAY_AREA_X_OFFSET, 25, 20, RAYWHITE);
}

Boss *GetActiveBoss(void)
{
    return &currBoss;
}

