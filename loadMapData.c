#include "include/cJSON.h" 
#include <stdio.h>
#include <stdlib.h>
#include "loadMapData.h"

static struct TileData* createTile(cJSON* jsonTile, int tileSize, int amountTilesX, errLoadMap* err);

static struct TileData* createTile(cJSON* jsonTile, int tileSize, int amountTilesX, errLoadMap* err) {
    struct TileData* tileData = malloc(sizeof(struct TileData));

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
	    *err = ERR_ID_TO_INT_CONVERT;
	    return NULL;
	}
    }
    tileData->sourceX = (id % amountTilesX) * tileSize;
    tileData->sourceY = ((int)(id / amountTilesX)) * tileSize;
    tileData->next = NULL;

    *err = OK;
    return tileData;
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

    struct TileData* curTileData = NULL;
    struct LayerData* layerData = malloc(sizeof(struct LayerData));
    cJSON* tile = NULL;

    cJSON_ArrayForEach(tile, tiles) {
	if (curTileData == NULL) {
	    curTileData = createTile(tile, tileSize->valueint, amountTilesX, err);
	    layerData->tileData = curTileData;
	} else {
	    curTileData->next = createTile(tile, tileSize->valueint, amountTilesX, err);
	    curTileData = curTileData->next;
	}
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
    struct TileData* curTileData = layerData->tileData;
    struct TileData* nextTileData = curTileData;

    while (nextTileData != NULL) {
	curTileData = nextTileData;
	nextTileData = nextTileData->next;
	free(curTileData);
    }
    free(layerData);
}
