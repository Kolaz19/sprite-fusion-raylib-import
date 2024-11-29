#include "loadMapData.h"
#include "include/jsmn.h"
#include "tilemapSF.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct LayerData *createLayer(char *jsonString, int layer, int textureWidth,
                              errTileMap *err) {

  jsmn_parser p;
  jsmntok_t t[JSON_MAX_TOKEN];
  jsmn_init(&p);
  int amountTokens =
      jsmn_parse(&p, jsonString, strlen(jsonString), t, JSON_MAX_TOKEN);
  if (amountTokens < 0) {
    *err = ERR_PARSE;
    return NULL;
  }
  jsmntok_t *layerStart = t;
  int curLayerNumber = 0;

  struct LayerData *layerData = malloc(sizeof(struct LayerData));
  layerData->tileSize = 0;

  // Skip to startOfLayers
  while (layerStart != t + amountTokens - 1) {
    if (strncmp(jsonString + layerStart->start, "layers",
                layerStart->end - layerStart->start) == 0) {
      layerStart += 2;
      break;
    } else if (strncmp(jsonString + layerStart->start, "tileSize",
                       layerStart->end - layerStart->start) == 0) {
      layerData->tileSize =
          strtol(jsonString + (layerStart + 1)->start, NULL, 0);
    }
    layerStart++;
  }

  if (layerStart == t + amountTokens - 1 || layerData->tileSize == 0) {
    *err = ERR_MISSING_PROPERTY;
    free(layerData);
    return NULL;
  }

  // Skip to selected layer (specifically name property)
  while (layerStart != t + amountTokens - 1) {
    if (strncmp(jsonString + layerStart->start, "name",
                layerStart->end - layerStart->start) == 0) {
      // printf("LayerName:\t%.*s\n", (layerStart+1)->end -
      // (layerStart+1)->start, jsonString + (layerStart+1)->start);
      curLayerNumber++;
      if (curLayerNumber == layer) {
        break;
      }
    } else if (layerStart->type == JSMN_ARRAY) {
      // Skip every tiles-array forward that is not current layer
      layerStart += layerStart->size * 7;
    }
    layerStart++;
  }

  if (layerStart == t + amountTokens - 1) {
    *err = ERR_LAYER_NOT_FOUND;
    free(layerData);
    return NULL;
  }

  // Name of layer
  int lenName = (layerStart + 1)->end - (layerStart + 1)->start;
  layerData->name = malloc(lenName + 1);
  strncpy(layerData->name, jsonString + (layerStart + 1)->start, lenName);
  layerData->name[lenName] = '\0';
  // printf("Name:\t%s\n", layerData->name);

  // Tiles
  if (strncmp(jsonString + (layerStart + 2)->start, "tiles",
              (layerStart + 2)->end - (layerStart + 2)->start) != 0) {
    *err = ERR_MISSING_PROPERTY;
    free(layerData->name);
    free(layerData);
    return NULL;
  }

  layerData->amountOfTiles = (layerStart + 3)->size;
  layerData->tileData =
      malloc(layerData->amountOfTiles * sizeof(struct TileData));

  // Start at array
  jsmntok_t *tileData = layerStart + 3;
  int arrayIndex = 0;
  int id = 0, x = 0, y = 0;
  for (int i = 0, curProperty = 1, amountAssigned = 0;
       i < layerData->amountOfTiles; i++, curProperty = 1, amountAssigned = 0) {
    while (curProperty != 7) {
      arrayIndex = (i * 7) + curProperty;
      if (strncmp(jsonString + (tileData + arrayIndex)->start, "id",
                  (tileData + arrayIndex)->end -
                      (tileData + arrayIndex)->start) == 0) {
        id = strtol(jsonString + (tileData + arrayIndex + 1)->start, NULL, 0);
        (layerData->tileData + i)->sourceX =
            (id % (textureWidth / layerData->tileSize)) * layerData->tileSize;
        (layerData->tileData + i)->sourceY =
            (int)(id / (textureWidth / layerData->tileSize)) *
            layerData->tileSize;
        amountAssigned++;
      } else if (strncmp(jsonString + (tileData + arrayIndex)->start, "x",
                         (tileData + arrayIndex)->end -
                             (tileData + arrayIndex)->start) == 0) {
        x = strtol(jsonString + (tileData + arrayIndex + 1)->start, NULL, 0);
        (layerData->tileData + i)->targetX = x * layerData->tileSize;
        amountAssigned++;
      } else if (strncmp(jsonString + (tileData + arrayIndex)->start, "y",
                         (tileData + arrayIndex)->end -
                             (tileData + arrayIndex)->start) == 0) {
        y = strtol(jsonString + (tileData + arrayIndex + 1)->start, NULL, 0);
        (layerData->tileData + i)->targetY = y * layerData->tileSize;
        amountAssigned++;
      }
      curProperty++;
    }
    if (amountAssigned != 3) {
      *err = ERR_TILEDATA_MISSING;
      free(layerData->tileData);
      free(layerData->name);
      free(layerData);
      return NULL;
    }
  }

  // Get collision data
  tileData += tileData->size * 7 + 1;
  if (strncmp(jsonString + (tileData)->start, "collider",
              (tileData)->end - (tileData)->start) == 0) {
    if (strncmp(jsonString + (tileData + 1)->start, "true",
                (tileData + 1)->end - (tileData + 1)->start) == 0) {
      layerData->isCollisionLayer = true;
    } else {
      layerData->isCollisionLayer = false;
    }
  } else {
    *err = ERR_MISSING_PROPERTY;
    free(layerData->tileData);
    free(layerData->name);
    free(layerData);
    return NULL;
  }

  return layerData;
}

int getNumberOfLayers(char *jsonString, errTileMap *err) {
  jsmn_parser p;
  jsmntok_t t[JSON_MAX_TOKEN];
  jsmn_init(&p);
  int amountTokens =
      jsmn_parse(&p, jsonString, strlen(jsonString), t, JSON_MAX_TOKEN);

  if (amountTokens < 0) {
    *err = ERR_PARSE;
    return 0;
  }

  jsmntok_t *layerStart = t;
  int curLayerNumber = 0;

  // Skip to startOfLayers
  while (layerStart != t + amountTokens - 1) {
    if (strncmp(jsonString + layerStart->start, "layers",
                layerStart->end - layerStart->start) == 0) {
      layerStart += 2;
      break;
    }
    layerStart++;
  }

  if (layerStart == t + amountTokens - 1) {
    *err = ERR_MISSING_PROPERTY;
    return 0;
  }

  // Skip to selected layer (specifically name property)
  while (layerStart != t + amountTokens - 1) {
    if (strncmp(jsonString + layerStart->start, "name",
                layerStart->end - layerStart->start) == 0) {
      curLayerNumber++;
    } else if (layerStart->type == JSMN_ARRAY) {
      // Skip every tiles-array forward that is not current layer
      layerStart += layerStart->size * 7;
    }
    layerStart++;
  }

  if (curLayerNumber == 0) {
    *err = ERR_NO_LAYER;
  }
  return curLayerNumber;
}

void unloadLayer(struct LayerData *layerData) {
  free(layerData->tileData);
  free(layerData->name);
  free(layerData);
}
