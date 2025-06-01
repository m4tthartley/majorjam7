//
//  Created by Matt Hartley on 31/05/2025.
//  Copyright 2025 GiantJelly. All rights reserved.
//

#ifndef __GAME_H__
#define __GAME_H__


#include <stdbool.h>
#include <core/core.h>
#include <core/math.h>


typedef struct {
	enum {
		TILE_GRASS,
		TILE_MARSH,
		TILE_DIRT,
		TILE_WATER,
		TILE_SEA,
	} type;
	float rotation;
	float ani;
	float water;
} tile_t;

typedef struct {
	vec2_t pos;
	float ani;
	uint32_t aniDir;
	int2_t tilePos;
	tile_t* tile;
} player_t;

enum {
	DIR_DOWN = 0,
	DIR_UP = 1,
	DIR_RIGHT = 2,
	DIR_LEFT = 3,
} direction_t;
vec2_t dirVectors[] = {
	{ 0.0f, -1.0f},
	{ 0.0f,  1.0f},
	{ 1.0f,  0.0f},
	{-1.0f,  0.0f},
};

typedef enum {
	PLANT_NONE,
	PLANT_FLOWER,
} plant_type_t;
enum {
	PLANT_LIKES_WATER,
	PLANT_WATER_RESISTANT,
	PLANT_WIND_RESISTANT,
	PLANT_LIKES_SHADE,
	PLANT_SHADE_RESISTANT,
	PLANT_LIGHTNING_RESISTANT,
} plant_flags_t;

typedef struct {
	plant_type_t type;
	uint32_t flags;
	float health;
} plant_t;

_Bool mapIslandMask[20*11] = {
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,1,1,0,0,0,1,1,1,1,0,0,0,0,
	0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,
	0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,
	0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,
	0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,
	0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,
	0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,
	0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,
	0,1,1,1,1,0,0,0,1,1,1,1,1,1,1,0,1,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

// #define MAP_SIZE_X 32
// #define MAP_SIZE_Y 18
int2_t mapSize;
uint32_t mapTotalTiles;
tile_t* mapTiles;

typedef struct {
	vec3_t pos;
	float timer;
} raindrop_t;
raindrop_t rain[128];

typedef struct {
	vec3_t pos;
	vec3_t speed;
	float timer;
} particle_t;
particle_t particles[512];

vec2_t cameraPos = {0};

player_t player;
vec2_t selectionPos;

_Bool isRaining = _True;

#endif
