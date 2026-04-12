#include "boss.h"
#include "../pattern/pattern.h"
#include "../player/player.h"
#include "../score/score.h"
#include "../item/item.h"
#include "../bullet/bullet.h"
#include "../config.h"
#include "../audio/audio_manager.h"
#include <string.h>
#include <math.h>

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
    SpawnItems(15, ITEM_POINT, currBoss.pos, 100);
    SpawnItems(5, ITEM_POWER, currBoss.pos, 1);
    
    // Clear bullets on phase transition
    ClearEnemyBullets();
    ClearEnemyPatterns();

    PlaySoundEvent(SND_BOSS_DEFEAT);

    currBoss.currPhase++;
    if (currBoss.currPhase >= currBoss.totalPhases)
    {
        currBoss.active = false;
        return;
    }
    
    // Reset position for next phase
    currBoss.pos = currBoss.phases[currBoss.currPhase].startPos;
    PlaySoundEvent(SND_BOSS_PHASE);
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
        PlaySoundEvent(SND_BOSS_SHOOT);
        current->lastShotTime = current->internalTimer;
    }
    
    // Options Logic
    for (int i = 0; i < MAX_BOSS_OPTIONS; i++)
    {
        BossOption *opt = &current->options[i];
        if (!opt->active) continue;

        // Update Position
        if (opt->moveMode == BOSS_OPTION_MOVE_STATIC)
        {
            opt->currentPos.x = currBoss.pos.x + opt->offset.x;
            opt->currentPos.y = currBoss.pos.y + opt->offset.y;
        }
        else if (opt->moveMode == BOSS_OPTION_MOVE_ROTATE)
        {
            opt->angle += opt->moveSpeed * dt;
            float s = sinf(opt->angle);
            float c = cosf(opt->angle);
            opt->currentPos.x = currBoss.pos.x + (opt->offset.x * c - opt->offset.y * s);
            opt->currentPos.y = currBoss.pos.y + (opt->offset.x * s + opt->offset.y * c);
        }

        // Shooting
        if (current->internalTimer - opt->lastShotTime >= opt->shootDelay)
        {
            int shots = opt->volleyShots > 0 ? opt->volleyShots : 1;
            float vDelay = opt->volleyDelay > 0.0f ? opt->volleyDelay : 0.0f;
            SpawnPattern(opt->pattern, BULLET_ENEMY, opt->currentPos, shots, 0.0f, vDelay);
            PlaySoundEvent(SND_BOSS_SHOOT);
            opt->lastShotTime = current->internalTimer;
            
            opt->shotsFired++;
            if (opt->maxShots > 0 && opt->shotsFired >= opt->maxShots)
            {
                opt->active = false;
            }
        }
    }
}

void DrawBoss(void)
{
    if (!currBoss.active) return;

    Vector2 screenPos = {
        currBoss.pos.x + PLAY_AREA_X_OFFSET,
        currBoss.pos.y + PLAY_AREA_Y_OFFSET
    };

    SpellCard *current = &currBoss.phases[currBoss.currPhase];

    // Draw Options
    for (int i = 0; i < MAX_BOSS_OPTIONS; i++)
    {
        BossOption *opt = &current->options[i];
        if (!opt->active) continue;
        
        Vector2 optScreenPos = {
            opt->currentPos.x + PLAY_AREA_X_OFFSET,
            opt->currentPos.y + PLAY_AREA_Y_OFFSET
        };
        
        DrawCircleV(optScreenPos, 12.0f, DARKPURPLE);
        DrawCircleLines(optScreenPos.x, optScreenPos.y, 14.0f, WHITE);
    }

    // Draw Boss Body
    DrawCircleV(screenPos, currBoss.radius, PURPLE);
    DrawCircleLines(screenPos.x, screenPos.y, currBoss.radius + 2.0f, WHITE);

    // Draw Health/Time Indicator
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

