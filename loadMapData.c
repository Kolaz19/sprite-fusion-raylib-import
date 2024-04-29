#include "include/cJSON.h" 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "loadMapData.h"

static errTileMap initTileData(struct TileData* tileData, cJSON* jsonTile, int tileSize, int amountTilesX);

struct LayerData* createLayer(char* jsonBuffer, int layer, int textureWidth, errTileMap* err) {
    cJSON* json = cJSON_Parse(jsonBuffer);
    if (json == NULL) {
	*err = ERR_PARSE;
	return NULL;
    }

    cJSON* tileSize = cJSON_GetObjectItem(json, "tileSize");
    if (tileSize == NULL) {
	*err = ERR_MISSING_PROPERTY;
	cJSON_Delete(json);
	return NULL;
    }

    cJSON* layers = cJSON_GetObjectItem(json, "layers");
    if (layers == NULL) {
	*err = ERR_MISSING_PROPERTY;
	cJSON_Delete(json);
	return NULL;
    }


    cJSON* curLayer = cJSON_GetArrayItem(layers,layer);
    if (curLayer == NULL) {
	*err = ERR_LAYER_NOT_FOUND;
	cJSON_Delete(json);
	return NULL;
    }

    cJSON* name = cJSON_GetObjectItem(curLayer, "name");
    if (name == NULL) {
	*err = ERR_MISSING_PROPERTY;
	cJSON_Delete(json);
	return NULL;
    }

    cJSON* tiles = cJSON_GetObjectItem(curLayer, "tiles");
    if (curLayer == NULL) {
	*err = ERR_MISSING_PROPERTY;
	cJSON_Delete(json);
	return NULL;
    }

    cJSON* collision = cJSON_GetObjectItem(curLayer, "collider");
    if (collision == NULL) {
	*err = ERR_MISSING_PROPERTY;
	cJSON_Delete(json);
	return NULL;
    }

    //We have all the required properties, now initialize the layer data
    struct LayerData* layerData = malloc(sizeof(struct LayerData));
    cJSON* tile = NULL;

    layerData->name = malloc(strlen(name->string)+1);
    strcpy(layerData->name, cJSON_GetStringValue(name));

    layerData->amountOfTiles = cJSON_GetArraySize(tiles);
    layerData->tileData = malloc(layerData->amountOfTiles * sizeof(struct TileData));
    layerData->isCollisionLayer = cJSON_IsTrue(collision);
    int amountTilesX = textureWidth / tileSize->valueint;
    int curTileIndex = 0;

    cJSON_ArrayForEach(tile, tiles) {
	*err = initTileData(layerData->tileData+curTileIndex, tile, tileSize->valueint, amountTilesX);
	curTileIndex++;
	if (*err != OK) {
	    unloadLayer(layerData);
	    cJSON_Delete(json);
	    return NULL;
	}
    }

    cJSON_Delete(json);
    return layerData;
}

errTileMap initTileData(struct TileData* tileData, cJSON* jsonTile, int tileSize, int amountTilesX) {
    //Set X and Y property of target
    cJSON* property = cJSON_GetObjectItem(jsonTile, "x");
    tileData->targetX = property->valueint * tileSize;
    property = cJSON_GetObjectItem(jsonTile, "y");
    tileData->targetY = property->valueint * tileSize;

    //Convert ID string to int and calculate source tile
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

int getNumberOfLayers(char* jsonBuffer, errTileMap *err) {
    cJSON* json = cJSON_Parse(jsonBuffer);
    if (json == NULL) {
	*err = ERR_PARSE;
	return 0;
    }

    cJSON* layers = cJSON_GetObjectItem(json, "layers");
    if (layers == NULL) {
	cJSON_Delete(json);
	*err = ERR_MISSING_PROPERTY;
	return 0;
    }

    *err = OK;
    int numberOfLayers = cJSON_GetArraySize(layers);
    cJSON_Delete(layers);
    return numberOfLayers;
}

void unloadLayer(struct LayerData* layerData) {
    free(layerData->tileData);
    free(layerData->name);
    free(layerData);
}
