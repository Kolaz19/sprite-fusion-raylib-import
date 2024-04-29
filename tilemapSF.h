#ifndef TILEMAPSF_H
#define TILEMAPSF_H

#include "include/raylib.h"
#define JSON_LEN 10000

typedef struct LayerData** LD;

typedef struct TileMap {
    Texture2D texture;
    int numberLayers;
    LD layerData;
} TileMap;

typedef enum errTileMap {
    OK = 1,
    ERR_PARSE,
    ERR_MISSING_PROPERTY,
    ERR_LAYER_NOT_FOUND,
    ERR_ID_TO_INT_CONVERT,
    ERR_TEXTURE_LOAD	       
}errTileMap;

TileMap* createMap(char* textureFileName, char* jsonFileName, errTileMap* err);
void unloadMap(TileMap* map);
void printMapData(TileMap* map);

#endif
