//
//  Created by Matt Hartley on 31/05/2025.
//  Copyright 2025 GiantJelly. All rights reserved.
//

#ifndef __GAME_H__
#define __GAME_H__


#include <stdbool.h>
#include <core/core.h>
#include <core/math.h>

#include "plant.h"


typedef struct {
	enum {
		TILE_GRASS,
		TILE_MARSH,
		TILE_DIRT,
		TILE_WATER,
		TILE_SEA,
		TILE_HOUSE_DIRT,
	} type;
	float rotation;
	float ani;
	float water;
	plant_t plant;
} tile_t;
char* tileNames[] = {
	"Grass",
	"Marsh",
	"Dirt",
	"Water",
	"Sea",
	"Dirt",
};

typedef struct {
	vec2_t pos;
	float ani;
	uint32_t aniDir;
	int2_t tilePos;
	tile_t* tile;
} player_t;

enum {
	DIR_DOWN = 0,
	DIR_UP = 2,
	DIR_RIGHT = 1,
	DIR_LEFT = 3,
} direction_t;
vec2_t dirVectors[] = {
	{ 0.0f, -1.0f},
	{ 1.0f,  0.0f},
	{ 0.0f,  1.0f},
	{-1.0f,  0.0f},
};

typedef enum {
	WEATHER_CALM = 0,
	WEATHER_RAIN,
	WEATHER_HEAVY_RAIN,
	WEATHER_WIND,
	WEATHER_STORM,
	// WEATHER_HEAT,
	WEATHER_COUNT,
} weather_t;
char* weatherNames[] = {
	"Calm",
	"Rain",
	"Heavy Rain",
	"Wind",
	"Storm",
	// "Heat Wave",
};

uint8_t mapIslandMask[20*11] = {
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,1,1,0,0,0,1,1,1,1,0,0,0,0,
	0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,
	0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,
	0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,
	0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,
	0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,
	0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,
	0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,
	0,1,1,1,1,0,0,0,1,1,2,2,2,2,1,0,1,0,0,0,
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
#define RAIN_COUNT (2048)
raindrop_t rain[RAIN_COUNT];

typedef struct {
	enum {
		WIND_SWIRL = 0,
		WIND_DUST = 1,
	} type;
	vec3_t pos;
	float timer;
	float ani;
} wind_t;
wind_t wind[32];

float lightningStrikeTimer;
int2_t lightningStrikePos;
vec2_t lightningVertices[8];

typedef struct {
	vec3_t pos;
	vec3_t speed;
	float timer;
} particle_t;
particle_t particles[RAIN_COUNT*2];

typedef struct {
	plant_def_t* plantDef;
	vec2_t pos;
	float rotation;
} wind_debris_t;
wind_debris_t windDebris;
float windDebrisTimer;

vec2_t cameraPos = {0};

player_t player;
vec2_t selectionPos;
uint32_t money = 100;

_Bool menuShop = _False;
int32_t shopSelected = 0;

// _Bool isRaining = _True;
weather_t currentWeather;
weather_t queuedWeather;
float weatherEventTimer;

#endif
