#ifndef ITEM_H
#define ITEM_H

#include "raylib.h"

typedef enum {
    ITEM_POWER,
    ITEM_POINT,
    ITEM_HEALTH,
    ITEM_LIFE
} ItemType;

typedef struct {
    int amt;
    ItemType type;
    Vector2 pos;
    Vector2 velocity;
} Item;

void InitItems();
void SpawnItems(int count, ItemType type, Vector2 pos, int amount);
void DrawItems(void);
void UpdateItems(void);
void RemoveItem(int index);
Item *GetItemPool(int *count);
#endif
