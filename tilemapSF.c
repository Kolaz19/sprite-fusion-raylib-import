#include "tilemapSF.h"
#include "include/raylib.h"
#include "loadMapData.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct TileMap {
    Texture2D texture;
    int numberLayers;
    LD layerData;
} TileMap;

errTileMap readFromFile(char* buf, char* filename, int buflen);

TileMap* createMap(char* textureFileName, char* jsonFileName, errTileMap* err) {
    *err = OK;
    TileMap* map = malloc(sizeof(TileMap));
    //Load texture
    map->texture = LoadTexture(textureFileName);
    if (map->texture.id <= 0) {
	*err = ERR_TEXTURE_LOAD;
	return NULL;
    }
    //Load JSON
    char jsonBuffer[JSON_LEN];
    *err = readFromFile(jsonBuffer, jsonFileName, JSON_LEN);
    if (*err != OK) {
	UnloadTexture(map->texture);
	free(map);
	return NULL;
    }

    //Load number of layers
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
	*curLayer = createLayer(jsonBuffer, i+1, map->texture.width, err);
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

Rectangle* createCollisionData(TileMap* map, Vector2 pos, float zoom, int* amount, errTileMap* err) {
    struct LayerData* ld;
    *amount = 0;
    //Get number of tiles that are collision relevant
    for (int i = 0; i < map->numberLayers; i++) {
	ld = *(map->layerData+i);
	if (ld->isCollisionLayer == true) {
	    *amount += ld->amountOfTiles;
	}
    }
    if (*amount == 0) {
	*err = ERR_NO_COLLISION_DATA;
	return NULL;
    }

    Rectangle* colData = malloc(*amount * sizeof(Rectangle));
    int index = 0;

    //Create collision rectangles based on tile data
    for (int i = 0; i < map->numberLayers; i++) {
	ld = *(map->layerData+i);
	if (ld->isCollisionLayer == true) {
	    for (int k = 0; k < ld->amountOfTiles; k++, index++) {
		(colData+index)->x = ((ld->tileData+k)->targetX * zoom) + pos.x;
		(colData+index)->y = ((ld->tileData+k)->targetY * zoom) + pos.y;
		(colData+index)->width = (colData+index)->height = ld->tileSize * zoom;
	    }
	}
    }
    *err = OK;
    return colData;
}

Rectangle* createCollisionDataLayer(TileMap* map, int layer, Vector2 pos, float zoom, int* amount, errTileMap* err) {
    struct LayerData* ld;
    *amount = 0;

    if (layer > map->numberLayers || layer < 0) {
	*err = ERR_LAYER_NOT_FOUND;
	return NULL;
    }

    ld = *(map->layerData+(layer-1));
    //Get number of tiles on layer
    *amount += ld->amountOfTiles;

    Rectangle* colData = malloc(*amount * sizeof(Rectangle));
    int index = 0;

    //Create collision rectangles based on tile data
    for (int k = 0; k < ld->amountOfTiles; k++, index++) {
	(colData+index)->x = ((ld->tileData+k)->targetX * zoom) + pos.x;
	(colData+index)->y = ((ld->tileData+k)->targetY * zoom) + pos.y;
	(colData+index)->width = (colData+index)->height = ld->tileSize * zoom;
    }
    *err = OK;
    return colData;
}

void unloadCollisionData(Rectangle* col) {
    free(col);
}

errTileMap readFromFile(char* buf, char* filename, int buflen) {
    FILE* file = fopen(filename,"r");
    if (file == NULL) {
	return ERR_FILE_OPEN;
    }
    //Read 1 byte BUF_LEN times or until EOF
    int bytesRead = fread(buf, 1, buflen, file);
    if (bytesRead == EOF) {
	fclose(file);
	return ERR_FILE_READ;
    }
    buf[bytesRead] = '\0';
    fclose(file);
    return OK;
}
