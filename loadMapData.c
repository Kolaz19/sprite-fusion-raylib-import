#include "include/cJSON.h" 
#include <stdio.h>
#include <stdlib.h>
#include "loadMapData.h"

struct TileData* createTile(cJSON* jsonTile, int textureWidth, int tileSize, errLoadMap* err) {
    struct TileData* tileData = malloc(sizeof(struct TileData));

    cJSON* property = cJSON_GetObjectItem(jsonTile, "x");
    tileData->targetX = property->valueint;
    property = cJSON_GetObjectItem(jsonTile, "y");
    tileData->targetY = property->valueint;

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
	goto cleanup;
    }

    cJSON* layers = cJSON_GetObjectItem(json, "layers");
    if (json == NULL) {
	*err = ERR_MISSING_PROPERTY;
	goto cleanup;
    }

    cJSON* curLayer = cJSON_GetArrayItem(layers,layer);
    if (json == NULL) {
	*err = ERR_LAYER_NOT_FOUND;
	goto cleanup;
    }

    cJSON* tiles = cJSON_GetObjectItem(curLayer, "tiles");

    struct TileData* curTileData = NULL;
    cJSON* tile = NULL;
    cJSON_ArrayForEach(tile, tiles) {
	curTileData = createTile(tile, textureWidth, tileSize->valueint, err);
	free(curTileData);
    }

    cJSON_Delete(json);

cleanup:
    cJSON_Delete(json);
    return NULL;
}


