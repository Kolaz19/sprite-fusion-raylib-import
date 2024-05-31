#include <stdio.h>
#include "tilemapSF.h"
#include "include/raylib.h"

#define NAME_JSON "map.json"
#define NAME_PNG "spritesheet.png"

#define keyU IsKeyDown(KEY_UP)
#define keyD IsKeyDown(KEY_DOWN)
#define keyL IsKeyDown(KEY_LEFT)
#define keyR IsKeyDown(KEY_RIGHT)
#define keyZoom IsKeyDown(KEY_R)
#define keyToggle IsKeyPressed(KEY_T)

#define SECOND_LAYER 2

int main() {

    errTileMap errL;
    int colAmount = 0;
    int colAmountLayer = 0;
    float zoom = 1;
    Vector2 pos = {0,0};
    bool toggleColData = false;

    InitWindow(1920*0.8, 1080*0.8, "Map Example");
    SetTargetFPS(60);

    //Create map
    TM map = createMap(NAME_PNG, NAME_JSON, &errL);
    if (errL != OK) {
	printf("Error creating map %d\n", errL);
    }

    //Create collision rectangles from map
    Rectangle* col = createCollisionData(map, pos,zoom,&colAmount, &errL);
    if (errL != OK) {
	printf("Error creating collision data of map %d\n", errL);
    }

    //Create collision rectangles from specific layer
    Rectangle* colLayer = createCollisionDataLayer(map,SECOND_LAYER, pos,zoom,&colAmountLayer, &errL);
    if (errL != OK) {
	printf("Error creating collision data of layer %d\n", errL);
    }

    while (!WindowShouldClose()) {
	//Change map properties
	//and create new collision data matching the new map
	//Once created, we can not change collision data
	if (keyZoom) zoom *= 1.01f;
	

	if (keyL) pos.x -= 5;
	if (keyR) pos.x += 5;
	if (keyD) pos.y += 5;
	if (keyU) pos.y -= 5;
	

	if (keyR || keyU || keyD || keyL || keyZoom) {
	    unloadCollisionData(col);
	    col = createCollisionData(map, pos, zoom, &colAmount, &errL);
	    unloadCollisionData(colLayer);
	    colLayer = createCollisionDataLayer(map,SECOND_LAYER, pos, zoom, &colAmountLayer, &errL);
	}

	if (keyToggle) toggleColData = !toggleColData;
	

	BeginDrawing();
	ClearBackground(RAYWHITE);

	renderLayer(map, "Ground", pos,zoom);
	renderLayer(map, "Collision", pos,zoom);
	renderLayer(map, "House", pos,zoom);

	//Draw collision rectangles over map
	//Normally you would use it to check for collision
	
	if (toggleColData) {
	    for (int i = 0; i < colAmount; i++) {
		DrawRectangleRec(*(col+i), RED);
	    }
	    for (int i = 0; i < colAmountLayer; i++) {
		DrawRectangleRec(*(colLayer+i), BLUE);
	    }
	}
	EndDrawing();

    }
    unloadMap(map);
    unloadCollisionData(col);
    unloadCollisionData(colLayer);
    CloseWindow();

    return 0;

}
