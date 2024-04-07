#include "include/cJSON.h" 
#include <stdio.h>
#include <stdlib.h>
#include "loadMapData.h"

struct TileData* createTile(cJSON* jsonTile) {
    struct TileData* tileData = malloc(sizeof(struct TileData));

    cJSON* property = cJSON_GetObjectItem(jsonTile, "x");
    tileData->targetX = property->valueint;
    property = cJSON_GetObjectItem(jsonTile, "y");
    tileData->targetY = property->valueint;
    return tileData;
}

struct LayerData* createLayer(char* jsonBuffer, int layer, errLoadMap* err) {
    cJSON* json = cJSON_Parse(jsonBuffer);
    if (json == NULL) {
	*err = ERR_PARSE;
	return NULL;
    }

    cJSON* layers = cJSON_GetObjectItem(json, "layers");
    if (json == NULL) {
	*err = ERR_NO_LAYERS_PROPERTY;
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
	curTileData = createTile(tile);
	free(curTileData);
    }

    cJSON_Delete(json);

cleanup:
    cJSON_Delete(json);
    return NULL;
}


