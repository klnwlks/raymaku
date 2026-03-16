#include "item.h"
#include "../config.h"

#define MAX_ITEMS 200

static Item itemPool[MAX_ITEMS];
static int itemCount = 0;

// used for the ""popping up and slowly falling effect""
static float gravity = 9.8f;
static Vector2 initVelocity = {0.0f, -30.0f};

void InitItems()
{
    for (int i = 0; i < MAX_ITEMS; i++)
    {
        itemPool[i] = (Item){0};
    }
}

void DrawItems()
{
    for (int i = 0; i < itemCount; i++) 
    {
        // draw rectangles here idfk
        // TODO: add multiple drawing for texture types
        DrawRectangleV(itemPool[i].pos, (Vector2){4.0f, 4.0f}, BLUE);
    }
}

void RemoveItem(int index)
{
    itemPool[index] = itemPool[itemCount -1];
    itemCount--;
}

void UpdateItems(void) 
{
    float dt = GetFrameTime();
    for (int i = 0; i < itemCount; i++)
    {
        // update position
        itemPool[i].velocity.y += gravity * dt;
        itemPool[i].pos.y += itemPool[i].velocity.y * dt;

        // check if we're past the screen
        bool offScreen = (itemPool[i].pos.x < -100 || 
                          itemPool[i].pos.x > PLAY_AREA_WIDTH + 100 ||
                          itemPool[i].pos.y > PLAY_AREA_HEIGHT + 100);
        if (offScreen)
        {
            itemPool[i] = itemPool[itemCount - 1]; 
            itemCount--;
            i--;
        }
    }
}

// init velocity as initvelocity
void SpawnItems(int count, ItemType type, Vector2 pos, int amount){
    for (int i = 0; i < count; i++)
    {
        Vector2 newpos = {
            GetRandomValue(pos.x - 10, pos.x + 10),
            GetRandomValue(pos.y - 10, pos.y + 10)
        };
        itemPool[itemCount].pos = newpos;
        itemPool[itemCount].velocity = initVelocity;
        itemPool[itemCount].type = type;
        itemPool[itemCount].amt = amount;
        itemCount++;
    }
}

Item *GetItemPool(int *count)
{
    *count = itemCount;
    return itemPool;
}
