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
	// PLANT_NONE = 0,
	PLANT_BANANA = 0,
	PLANT_SUGAR_CANE,
	PLANT_MINT,
	PLANT_CRANBERRY,
	PLANT_WATER_CHESTNUT,
	PLANT_STRAWBERRY,
	PLANT_SWEET_POTATO,
	PLANT_CARROT,
	PLANT_SPINACH,
	PLANT_COUNT,
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
	uint32_t spriteStages;
	float tileOffset;
	uint32_t cost;
	uint32_t profit;
	_Bool reharvestable;
} plant_def_t;

plant_def_t plantDefs[] = {
	{
		.type = PLANT_BANANA,
		.name = "Banana Plant",
		.flags = PLANT_LIKES_WATER | PLANT_LIKES_DRY,
		.spriteOffset = {0, 192},
		.spriteSize = {64, 64},
		.tileOffset = -0.3f,
		.spriteStages = 3,
		.cost = 30,
		.profit = 60,
		.reharvestable = _True,
	},
	{
		.type = PLANT_SUGAR_CANE,
		.name = "Sugar Cane",
		.flags = PLANT_LIKES_WATER | PLANT_LIKES_DRY,
		.spriteOffset = {0, 128},
		.spriteSize = {32, 64},
		.tileOffset = -0.3f,
		.spriteStages = 3,
		.cost = 30,
		.profit = 60,
		.reharvestable = _False,
	},
	{
		.type = PLANT_MINT,
		.name = "Mint Bush",
		.flags = PLANT_LIKES_WATER | PLANT_LIKES_DRY | PLANT_LIGHTNING_RESISTANT,
		.spriteOffset = {0, 96},
		.spriteSize = {64, 32},
		.tileOffset = 0.0f,
		.spriteStages = 2,
		.cost = 30,
		.profit = 60,
		.reharvestable = _False,
	},
	{
		.type = PLANT_CRANBERRY,
		.name = "Cranberry Bush",
		.flags = PLANT_LIKES_WATER,
		.spriteOffset = {0, 32},
		.spriteSize = {64, 64},
		.tileOffset = -0.1f,
		.spriteStages = 3,
		.cost = 30,
		.profit = 60,
		.reharvestable = _True,
	},
	{
		.type = PLANT_WATER_CHESTNUT,
		.name = "Water Chestnut",
		.flags = PLANT_LIKES_WATER,
		.spriteOffset = {0, 0},
		.spriteSize = {16, 32},
		.tileOffset = -0.1f,
		.spriteStages = 2,
		.cost = 30,
		.profit = 60,
		.reharvestable = _False,
	},
	{
		.type = PLANT_STRAWBERRY,
		.name = "Strawberries",
		.flags = PLANT_LIKES_DRY | PLANT_LIGHTNING_RESISTANT,
		.spriteOffset = {64, 0},
		.spriteSize = {32, 32},
		.tileOffset = -0.25f,
		.spriteStages = 3,
		.cost = 30,
		.profit = 60,
		.reharvestable = _True,
	},
	{
		.type = PLANT_SWEET_POTATO,
		.name = "Sweet Potato Plant",
		.flags = PLANT_LIKES_DRY | PLANT_LIKES_WIND,
		.spriteOffset = {192, 0},
		.spriteSize = {32, 32},
		.tileOffset = -0.2f,
		.spriteStages = 2,
		.cost = 30,
		.profit = 60,
		.reharvestable = _False,
	},
	{
		.type = PLANT_CARROT,
		.name = "Carrot",
		.flags = PLANT_LIKES_DRY | PLANT_LIKES_WIND,
		.spriteOffset = {192, 32},
		.spriteSize = {32, 32},
		.tileOffset = -0.2f,
		.spriteStages = 2,
		.cost = 30,
		.profit = 60,
		.reharvestable = _False,
	},
	{
		.type = PLANT_SPINACH,
		.name = "Spinach",
		.flags = PLANT_LIKES_DRY | PLANT_LIKES_WIND | PLANT_LIGHTNING_RESISTANT,
		.spriteOffset = {192, 64},
		.spriteSize = {32, 32},
		.tileOffset = -0.2f,
		.spriteStages = 2,
		.cost = 30,
		.profit = 60,
		.reharvestable = _False,
	},
};

typedef struct {
	// plant_type_t type;
	// uint32_t flags;
	plant_def_t def;
	float health;
	// float water;
	_Bool alive;
	float stageTimer;
	uint32_t stage;
} plant_t;


#endif
