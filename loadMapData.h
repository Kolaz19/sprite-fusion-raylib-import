#ifndef LOADMAP_H
#define LOADMAP_H
#include <stdbool.h>
//#include "include/raylib.h"

struct TileData {
    int targetX;
    int targetY;
    float sourceX;
    float sourceY;
    struct TileData* next;
};

struct LayerData {
    char* name;
    bool collisionLayer;
    struct TileData* tileData;
};

/*
struct TileMap {
    Texture2D texture;
    int numberLayers;
    struct LayerData* layerData;
};
*/

typedef enum errLoadMap {
    OK = 1,
    ERR_PARSE,
    ERR_NO_LAYERS_PROPERTY,
    ERR_LAYER_NOT_FOUND
}errLoadMap;

#endif
