#include "tilemapSF.h"
#include "include/raylib.h"
#include "loadMapData.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int readFromFile(char* buf, char* filename, int buflen);


TileMap* createMap(char* textureFileName, char* jsonFileName, errTileMap* err) {
    *err = OK;
    TileMap* map = malloc(sizeof(TileMap));
    //Load texture
    map->texture = LoadTexture(textureFileName);
    if (map->texture.id <= 0) {
	*err = ERR_TEXTURE_LOAD;
	return NULL;
    }
    //Load number of layers
    char jsonBuffer[JSON_LEN];
    readFromFile(jsonBuffer, jsonFileName, JSON_LEN);
    map->numberLayers = getNumberOfLayers(jsonBuffer, err);
    if (*err != OK) {
	UnloadTexture(map->texture);
	free(map);
	return NULL;
    }
    //Create layers
    struct LayerData** curLayer;
    map->layerData = malloc(sizeof(struct LayerData*) * map->numberLayers);
    for (int i = 0; i < map->numberLayers; i++) {
	curLayer = map->layerData + i;
	*curLayer = createLayer(jsonBuffer, i, map->texture.width, err);
	if (*err != OK) {
	    for (int inner = 0; inner < i; inner++) {
		unloadLayer((*map->layerData)+inner);
	    }
	    free(map->layerData);
	    UnloadTexture(map->texture);
	    free(map);
	    return NULL;
	}
    }

    return map;
}

void printMapData(TileMap* map) {
    printf("Number of Layers:\t%d\n",map->numberLayers);
    struct LayerData* ld;
    for (int i = 0; i < map->numberLayers; i++) {
	ld = *(map->layerData+i);
	printf("Name:\t%s\n",ld->name);
	printf("Number of layers:\t%d\n", map->numberLayers);

	for (int k = 0; k < ld->amountOfTiles; k++) {
	    printf("SourceX:%f\tSourceY:%f\tTargetX:%f\tTargetY:%f\n",(ld->tileData+k)->sourceX, (ld->tileData+k)->sourceY, (ld->tileData+k)->targetX, (ld->tileData+k)->targetY);
	}
    }
}

void renderLayer(TileMap* map, const char* layerName, Vector2 pos, float zoom) {
    struct LayerData* ld = NULL;
    for (int i = 0; i < map->numberLayers; i++) {
	if (!strcmp(layerName, (*(map->layerData+i))->name)) {
	    ld = *(map->layerData+i);
	    break;
	}
    }
    if (ld == NULL) {
	return;
    }

    struct TileData* td;
    for (int i = 0; i < ld->amountOfTiles; i++) {
	td = ld->tileData+i;
	//float shiftX = 
	DrawTexturePro(map->texture, 
		(Rectangle){td->sourceX,td->sourceY,ld->tileSize,ld->tileSize},
		(Rectangle){(td->targetX * zoom) + pos.x ,(td->targetY * zoom) + pos.y, ld->tileSize * zoom,ld->tileSize * zoom}, 
		(Vector2){0,0}, 
		0, 
		RAYWHITE);
    }
}

void unloadMap(TileMap* map) {
    for (int i = 0; i < map->numberLayers; i++) {
	unloadLayer(*(map->layerData+i));
    }
    free(map->layerData);
    UnloadTexture(map->texture);
    free(map);
}

static int readFromFile(char* buf, char* filename, int buflen) {
    FILE* file = fopen(filename,"r");
    if (file == NULL) {
	return 0;
    }
    //Read 1 byte BUF_LEN times or until EOF
    int bytesRead = fread(buf, 1, buflen, file);
    if (bytesRead == EOF) {
	fclose(file);
	return 0;
    }
    fclose(file);
    return 1;
}
