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
    int amountOfTiles;
    struct TileData* tileData;
};

struct LayerData* createLayer(char* jsonBuffer, int layer, int textureWidth, errTileMap* err);
void unloadLayer(struct LayerData* layerData);
int getNumberOfLayers(char* jsonBuffer, errTileMap *err);

#endif
