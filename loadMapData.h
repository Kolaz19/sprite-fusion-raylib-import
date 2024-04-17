#ifndef LOADMAP_H
#define LOADMAP_H
#include <stdbool.h>

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

typedef enum errLoadMap {
    OK = 1,
    ERR_PARSE,
    ERR_MISSING_PROPERTY,
    ERR_LAYER_NOT_FOUND,
    ERR_ID_TO_INT_CONVERT
}errLoadMap;

struct LayerData* createLayer(char* jsonBuffer, int layer, int textureWidth, errLoadMap* err);
void unloadLayer(struct LayerData* layerData);
int getNumberOfLayers(char* jsonBuffer, errLoadMap *err);

#endif
