#ifndef TILEMAPSF_H
#define TILEMAPSF_H

#include "include/raylib.h"

typedef struct LayerData* LD;

typedef struct TileMap {
    Texture2D texture;
    int numberLayers;
    LD layerData;
} TileMap;

#endif
