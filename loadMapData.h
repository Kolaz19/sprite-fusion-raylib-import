#ifndef LOADMAP_H
#define LOADMAP_H
#include <stdbool.h>
//For error codes
#include "tilemapSF.h"

struct TileData {
    float targetX;
    float targetY;
    float sourceX;
    float sourceY;
};

struct LayerData {
    char* name;
    bool isCollisionLayer;
    int tileSize;
    int amountOfTiles;
    struct TileData* tileData;
};

struct LayerData* createLayer(char* jsonString, int layer, int textureWidth, errTileMap* err);
void unloadLayer(struct LayerData* layerData);
int getNumberOfLayers(char* jsonString, errTileMap *err);

#endif
