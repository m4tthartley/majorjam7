//
//  Created by Matt Hartley on 31/05/2025.
//  Copyright 2025 GiantJelly. All rights reserved.
//

#ifndef __GAME_H__
#define __GAME_H__


#include <core/math.h>


typedef struct {
	enum {
		TILE_GRASS,
		TILE_MARSH,
		TILE_DIRT,
		TILE_WATER,
		TILE_SEA,
	} type;
} tile_t;

#define MAP_SIZE_X 32
#define MAP_SIZE_Y 18
tile_t map[MAP_SIZE_X*MAP_SIZE_Y];

vec2_t cameraPos = {0};


#endif
