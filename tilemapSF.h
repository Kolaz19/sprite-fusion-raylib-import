#ifndef TILEMAPSF_H
#define TILEMAPSF_H

#define JSON_LEN 10000
#define JSON_MAX_TOKEN 8000

#include "include/raylib.h"

typedef struct LayerData** LD;
typedef struct TileMap* TM;

    
typedef enum errTileMap {
    OK = 1,
    ERR_FILE_READ,
    ERR_FILE_OPEN,
    ERR_PARSE,
    ERR_MISSING_PROPERTY,
    ERR_LAYER_NOT_FOUND,
    ERR_NO_LAYER,
    ERR_TILEDATA_MISSING,
    ERR_TEXTURE_LOAD,
    ERR_NO_COLLISION_DATA
}errTileMap;

TM createMap(char* textureFileName, char* jsonFileName, errTileMap* err);
void unloadMap(TM map);
void printMapData(TM map);
void renderLayer(TM map, const char* layerName, Vector2 pos, float zoom);
Rectangle* createCollisionData(TM map, Vector2 pos, float zoom, int* amount, errTileMap* err);
Rectangle* createCollisionDataLayer(TM map, int layer, Vector2 pos, float zoom, int* amount, errTileMap* err);
void unloadCollisionData(Rectangle* col);

#endif
