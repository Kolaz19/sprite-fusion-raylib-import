#include "include/cJSON.h" 
#include <stdio.h>
#include <stdlib.h>
#include "loadMapData.h"

errLoadMap initTileData(struct TileData* tileData, cJSON* jsonTile, int tileSize, int amountTilesX);

errLoadMap initTileData(struct TileData* tileData, cJSON* jsonTile, int tileSize, int amountTilesX) {
    cJSON* property = cJSON_GetObjectItem(jsonTile, "x");
    tileData->targetX = property->valueint * tileSize;
    property = cJSON_GetObjectItem(jsonTile, "y");
    tileData->targetY = property->valueint * tileSize;

    //Convert ID string to int
    property = cJSON_GetObjectItem(jsonTile, "id");
    int id = 0;
    if (*(property->valuestring) != '0') {
	char* strEnd = NULL;
	id = strtol(property->valuestring, &strEnd , 10);
	if (id == 0) {
	    free(tileData);
	    return ERR_ID_TO_INT_CONVERT;
	}
    }
    tileData->sourceX = (id % amountTilesX) * tileSize;
    tileData->sourceY = ((int)(id / amountTilesX)) * tileSize;

    return OK;
}

struct LayerData* createLayer(char* jsonBuffer, int layer, int textureWidth, errLoadMap* err) {
    cJSON* json = cJSON_Parse(jsonBuffer);
    if (json == NULL) {
	*err = ERR_PARSE;
	return NULL;
    }

    cJSON* tileSize = cJSON_GetObjectItem(json, "tileSize");
    if (json == NULL) {
	*err = ERR_MISSING_PROPERTY;
	cJSON_Delete(json);
	return NULL;
    }
    int amountTilesX = textureWidth / tileSize->valueint;

    cJSON* layers = cJSON_GetObjectItem(json, "layers");
    if (json == NULL) {
	*err = ERR_MISSING_PROPERTY;
	cJSON_Delete(json);
	return NULL;
    }

    cJSON* curLayer = cJSON_GetArrayItem(layers,layer);
    if (json == NULL) {
	*err = ERR_LAYER_NOT_FOUND;
	cJSON_Delete(json);
	return NULL;
    }

    cJSON* tiles = cJSON_GetObjectItem(curLayer, "tiles");

    struct LayerData* layerData = malloc(sizeof(struct LayerData));
    cJSON* tile = NULL;
    layerData->amountOfTiles = cJSON_GetArraySize(tiles);
    layerData->tileData = malloc(layerData->amountOfTiles * sizeof(struct TileData));
    int curTileIndex = 0;

    cJSON_ArrayForEach(tile, tiles) {
	*err = initTileData(layerData->tileData+curTileIndex, tile, tileSize->valueint, amountTilesX);
	curTileIndex++;
	if (*err != OK) {
	    unloadLayerData(layerData);
	    cJSON_Delete(json);
	    return NULL;
	}
    }

    cJSON_Delete(json);
    return layerData;
}

void unloadLayerData(struct LayerData* layerData) {
    free(layerData->tileData);
    free(layerData);
}
