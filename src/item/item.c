#include "item.h"
#include "../config.h"
#include "raylib.h"
#include "raymath.h"
#include "../player/player.h"
#include "../score/score.h"

#define MAX_ITEMS 200

static Item itemPool[MAX_ITEMS];
static int itemCount = 0;

// items pop up and fall down slowly
static float gravity = 45.0f;
static Vector2 initVelocity = {0.0f, -45.0f};

void InitItems()
{
    for (int i = 0; i < MAX_ITEMS; i++)
    {
        itemPool[i] = (Item){0};
    }
    itemCount = 0;
}

void DrawItems()
{
    for (int i = 0; i < itemCount; i++) 
    {
        // Don't draw if outside play area
        if (itemPool[i].pos.x < 0 || itemPool[i].pos.x > PLAY_AREA_WIDTH ||
            itemPool[i].pos.y < 0 || itemPool[i].pos.y > PLAY_AREA_HEIGHT) continue;

        // draw different colors for different types
        Color itemColor = WHITE;
        switch(itemPool[i].type)
        {
            case ITEM_POWER: itemColor = RED; break;
            case ITEM_POINT: itemColor = BLUE; break;
            case ITEM_LIFE: itemColor = PINK; break;
            case ITEM_BOMB: itemColor = GREEN; break;
        }

        Vector2 screenPos = {
            itemPool[i].pos.x + PLAY_AREA_X_OFFSET,
            itemPool[i].pos.y + PLAY_AREA_Y_OFFSET
        };

        DrawCircleV(screenPos, 4.0f, itemColor);
        DrawCircleLines(screenPos.x, screenPos.y, 5.0f, WHITE);
    }
}

// this will also handle stat changes
void RemoveItem(int index, ItemType type)
{
    int amt = itemPool[index].amt;
    itemPool[index] = itemPool[itemCount - 1];
    itemCount--;

    Player* player = GetPlayer();

    switch(type) 
    {
        case ITEM_LIFE:
            player->lives++;
            break;
        case ITEM_POINT:
            AddScore(amt); 
            player->points++;
            if (player->points >= 200)
            {
                player->lives++;
                player->points = 0;
            }
            break;
        case ITEM_POWER:
            player->power += amt;
            break;
        case ITEM_BOMB:
            player->bombs++;
            break;
        default:
            break;
    }
}

void UpdateItems(void) 
{
    float dt = GetFrameTime();
    Player *player = GetPlayer();
    
    // Point of Collection (POC): top 25% of screen pulls everything
    float pocLine = PLAY_AREA_HEIGHT * 0.3f;
    bool playerAbovePOC = player->position.y < pocLine;

    for (int i = 0; i < itemCount; i++)
    {
        // Attraction check
        if (playerAbovePOC) 
        {
            itemPool[i].attracted = true;
        }
        else
        {
            float dist = Vector2Distance(itemPool[i].pos, player->position);
            if (dist < 100.0f) itemPool[i].attracted = true;
        }

        if (itemPool[i].attracted)
        {
            // Accelerated flight toward player
            Vector2 dir = Vector2Normalize(Vector2Subtract(player->position, itemPool[i].pos));
            float attractionSpeed = 800.0f;
            itemPool[i].pos = Vector2Add(itemPool[i].pos, Vector2Scale(dir, attractionSpeed * dt));
        }
        else
        {
            // Normal falling position
            itemPool[i].velocity.y += gravity * dt;
            itemPool[i].pos.y += itemPool[i].velocity.y * dt;
            itemPool[i].pos.x += itemPool[i].velocity.x * dt;
        }

        // check if we're past the screen boundaries
        bool offScreen = (itemPool[i].pos.y > PLAY_AREA_HEIGHT + 20);
        
        if (offScreen)
        {
            itemPool[i] = itemPool[itemCount - 1]; 
            itemCount--;
            i--;
        }
    }
}

// init velocity as initvelocity
void SpawnItems(int count, ItemType type, Vector2 pos, int amount)
{
    for (int i = 0; i < count; i++)
    {
        if (itemCount >= MAX_ITEMS) break;

        Vector2 newpos = {
            GetRandomValue(pos.x - 15, pos.x + 15),
            GetRandomValue(pos.y - 15, pos.y + 15)
        };
        
        itemPool[itemCount].pos = newpos;
        itemPool[itemCount].velocity = (Vector2) {0, initVelocity.y};
        itemPool[itemCount].type = type;
        itemPool[itemCount].amt = amount;
        itemPool[itemCount].attracted = false;
        itemCount++;
    }
}

Item *GetItemPool(int *count)
{
    *count = itemCount;
    return itemPool;
}
