//
//  Created by Matt Hartley on 01/06/2025.
//  Copyright 2025 GiantJelly. All rights reserved.
//

#ifndef __PLANT_H__
#define __PLANT_H__


#include <stdint.h>
#include <core/core.h>
#include <core/math.h>


typedef enum {
	PLANT_NONE = 0,
	PLANT_BANANA = 1,

	PLANT_CRANBERRY,

	PLANT_SWEET_POTATO,
} plant_type_t;
enum {
	PLANT_FLAG_NONE				= 0,
	PLANT_LIKES_WATER			= (1<<0),
	PLANT_LIKES_DRY				= (1<<1),
	PLANT_LIKES_WIND			= (1<<2),
	PLANT_LIKES_SHADE			= (1<<3),
	PLANT_LIGHTNING_RESISTANT	= (1<<4),
	// PLANT_WATER_RESISTANT,
	// PLANT_WIND_RESISTANT,
	// PLANT_SHADE_RESISTANT,
} plant_flags_t;

typedef struct {
	plant_type_t type;
	char* name;
	uint32_t flags;
	uint32_t spriteIndex;
	vec2_t spriteOffset;
	vec2_t spriteSize;
	float tileOffset;
	uint32_t numStages;
	uint32_t price;
} plant_def_t;

plant_def_t plantDefs[] = {
	{
		.type = PLANT_BANANA,
		.name = "Banana Plant",
		.flags = PLANT_LIKES_WATER | PLANT_LIKES_DRY,
		.spriteOffset = {0, 192},
		.spriteSize = {64, 64},
		.tileOffset = -0.3f,
		.numStages = 3,
	},
	{
		.type = PLANT_CRANBERRY,
		.name = "Cranberry Bush",
		.flags = PLANT_LIKES_WATER,
		.spriteOffset = {0, 32},
		.spriteSize = {64, 64},
		.tileOffset = -0.25f,
		.numStages = 3,
	},
	{
		.type = PLANT_SWEET_POTATO,
		.name = "Sweet Potato Plant",
		.flags = PLANT_LIKES_DRY | PLANT_LIKES_WIND,
		.spriteOffset = {192, 0},
		.spriteSize = {32, 32},
		.tileOffset = -0.4f,
		.numStages = 2,
	},
};

typedef struct {
	// plant_type_t type;
	// uint32_t flags;
	plant_def_t def;
	float health;
	// float water;
	_Bool alive;
} plant_t;


#endif
