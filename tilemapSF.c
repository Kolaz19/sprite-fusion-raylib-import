#include "tilemapSF.h"
#include "include/raylib.h"
#include "loadMapData.h"
#include <stdio.h>
#include <stdlib.h>

static int readFromFile(char* buf, char* filename, int buflen);


TileMap* createMap(char* textureFileName, char* jsonFileName, errTileMap* err) {
    *err = OK;
    TileMap* map = malloc(sizeof(TileMap));
    map->texture = LoadTexture(textureFileName);
    if (map->texture.id <= 0) {
	*err = ERR_TEXTURE_LOAD;
	return NULL;
    }

    char jsonBuffer[JSON_LEN];
    map->numberLayers = getNumberOfLayers(jsonBuffer, err);
    if (!err) {
	UnloadTexture(map->texture);
	free(map);
	return NULL;
    }

    map->layerData = malloc(sizeof(struct LayerData) * map->numberLayers);

    
    return map;
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
