//
//  Created by Matt Hartley on 31/05/2025.
//  Copyright 2025 GiantJelly. All rights reserved.
//


// #include <core/core.h>
#include <core/sysvideo.h>
#include <core/time.h>
#include "game.h"


extern sys_window_t window;
frametimer_t timer;


void G_CreateParticle(vec3_t pos, vec3_t speed)
{
	for (int i=0; i<array_size(particles); ++i) {
		if (particles[i].timer <= 0.1f) {
			particles[i].timer = randfr(1.0f, 2.0f);
			particles[i].pos = pos;
			particles[i].speed = speed;
			return;
		}
	}

	print("No available particle slots \n");
}

void AddPlant(plant_def_t plantDef)
{
	plant_t plant = {
		.def = plantDef,
		.health = 1.0f,
		.alive = _True,
		.stageTimer = 30.0f,
	};

	player.tile->plant = plant;
}

void G_Init()
{
	timer = frametimer_init();
	queuedWeather = WEATHER_CALM;

	// mapSize.x = window.width / 32 / 2;
	// mapSize.y = window.height / 32 / 2;
	mapSize.x = 1280 / 32 / 2;
	mapSize.y = 720 / 32 / 2;
	mapTotalTiles = mapSize.x * mapSize.y;
	mapTiles = sys_alloc_memory(sizeof(tile_t) * mapTotalTiles);

	for (int i=0; i<mapTotalTiles; ++i) {
		float x = i%mapSize.x;
		float y = i/mapSize.y;
		float height = fbm(vec2(x/5.0f, y/5.0f));

		mapTiles[i].type = TILE_SEA;
		// if (height > 0.5f) {
		if (mapIslandMask[i]) {
			mapTiles[i].type = TILE_DIRT;
			if (height > 0.55f) {
				mapTiles[i].type = TILE_MARSH;
			}
		}
		float r = randfr(0.0f, 4.0f);
		mapTiles[i].rotation = floorf(r);
	}

	for (int i=0; i<array_size(rain); ++i) {
		rain[i].timer = randfr(0.0f, 5.0f);
		rain[i].pos = vec3(randfr(0.0f, mapSize.x), randfr(0.0f, mapSize.y), randfr(20.0f, 30.0f));
	}

	player.pos = vec2((float)mapSize.x/2, (float)mapSize.y/2);
}

void G_Frame()
{
	frametimer_update(&timer);
	float dt = timer.dt;

	// WEATHER
	weatherEventTimer -= dt;
	if (weatherEventTimer < 0.0f) {
		weatherEventTimer = randfr(60.0f, 120.0f);
		print("A weather event is occuring.. \n");

		currentWeather = queuedWeather;
		queuedWeather = randr(0, WEATHER_COUNT);
	}

	if (menuShop) {
		// SHOP
		if (window.keyboard[KEY_ESC].released) {
			menuShop = _False;
		}

		if (window.keyboard[KEY_DOWN].released) {
			shopSelected = min(shopSelected + 1, array_size(plantDefs)-1);
		}
		if (window.keyboard[KEY_UP].released) {
			shopSelected = max(shopSelected - 1, 0);
		}

		if (window.keyboard[KEY_RETURN].released && player.tile) {
			AddPlant(plantDefs[shopSelected]);
			menuShop = _False;
		}
	} else {
		// PLAYER CONTROLS
		if (window.keyboard[KEY_A].down) {
			cameraPos.x -= 10.0f * dt;
		}
		if (window.keyboard[KEY_D].down) {
			cameraPos.x += 10.0f * dt;
		}
		if (window.keyboard[KEY_S].down) {
			cameraPos.y -= 10.0f * dt;
		}
		if (window.keyboard[KEY_W].down) {
			cameraPos.y += 10.0f * dt;
		}

		float playerSpeed = 3.0f * dt;
		_Bool running = _False;
		if (window.keyboard[KEY_LEFT].down) {
			running = _True;
			vec2_t nextTilePos = vec2(player.pos.x - 0.4f + 0.5f, player.pos.y + 0.5f);
			if (mapTiles[(int)nextTilePos.y*mapSize.x + (int)nextTilePos.x].type != TILE_SEA) {
				player.pos.x -= playerSpeed;
				player.aniDir = DIR_LEFT;
			}
		}
		if (window.keyboard[KEY_RIGHT].down) {
			running = _True;
			vec2_t nextTilePos = vec2(player.pos.x + 0.4f + 0.5f, player.pos.y + 0.5f);
			if (mapTiles[(int)nextTilePos.y*mapSize.x + (int)nextTilePos.x].type != TILE_SEA) {
				player.pos.x += playerSpeed;
				player.aniDir = DIR_RIGHT;
			}
		}
		if (window.keyboard[KEY_DOWN].down) {
			running = _True;
			vec2_t nextTilePos = vec2(player.pos.x + 0.5f, player.pos.y - 0.25f + 0.5f);
			if (mapTiles[(int)nextTilePos.y*mapSize.x + (int)nextTilePos.x].type != TILE_SEA) {
				player.pos.y -= playerSpeed;
				player.aniDir = DIR_DOWN;
			}
		}
		if (window.keyboard[KEY_UP].down) {
			running = _True;
			vec2_t nextTilePos = vec2(player.pos.x + 0.5f, player.pos.y + 0.4f + 0.5f);
			if (mapTiles[(int)nextTilePos.y*mapSize.x + (int)nextTilePos.x].type != TILE_SEA) {
				player.pos.y += playerSpeed;
				player.aniDir = DIR_UP;
			}
		}
		if (running) {
			player.ani += 4.0f * dt;
		}

		if (window.keyboard[KEY_R].released) {
			isRaining = !isRaining;
		}
	}

	// TILE SELECTION
	vec2_t selectionOffset = mul2f(add2(dirVectors[player.aniDir], vec2(0, 0.25f)), 1.0f);
	selectionPos = add2(player.pos, selectionOffset);
	int2_t pos = int2(selectionPos.x + 0.5f, selectionPos.y + 0.5f);
	player.tilePos = pos;
	pos.x = max(min(pos.x, mapSize.x-1), 0);
	pos.y = max(min(pos.y, mapSize.y-1), 0);
	player.tile = &mapTiles[pos.y*mapSize.x + pos.x];
	if (player.tile->type != TILE_SEA && window.keyboard[KEY_SPACE].released) {
		menuShop = _True;
	}

	// TILES
	for (int i=0; i<mapTotalTiles; ++i) {
		tile_t* t = mapTiles + i;
		t->ani += dt * 1.0f;

		if (t->water > 0.0f) {
			if (isRaining) {
				t->water -= 0.01f * dt;
			} else {
				t->water -= 0.05f * dt;
			}
		}

		if (t->plant.alive) {
			// t->water = max(t->water - 0.1f*dt, 0.0f);
			t->plant.stageTimer -= dt;
			if (t->plant.stageTimer <= 0.0f) {
				t->plant.stage = min(t->plant.stage+1, 2);
				t->plant.stageTimer = 30.0f;
			}

			uint32_t likesWater = t->plant.def.flags & PLANT_LIKES_WATER;
			if (!likesWater && t->water > 1.0f) {
				t->plant.health -= 0.01f * dt;
			}

			if (t->plant.health <= 0.0f) {
				t->plant.alive = _False;
			}
		}
	}

	// RAIN
	for (int i=0; i<array_size(rain); ++i) {
		if (rain[i].timer < 0.0f) {
			rain[i].pos.z -= 20.0f * dt;
			rain[i].pos.x -= 5.0f * dt;
			if (rain[i].pos.z < 0.0f) {
				int2_t tilePos = int2(rain[i].pos.x + 0.5f, rain[i].pos.y + 0.5f);
				int tileIndex = tilePos.y*mapSize.x + tilePos.x;
				if (tileIndex >= 0 && tileIndex < mapTotalTiles) {
					mapTiles[tileIndex].water += 0.3f;
					// if (mapTiles[tileIndex].plant.alive) {
					// 	mapTiles[tileIndex].plant.water = min(mapTiles[tileIndex].plant.water + 0.3f, 2.0f);
					// }
					if (mapTiles[tileIndex].water > 2.0f) {
						mapTiles[tileIndex].water = 2.0f;
					}
				}

				rain[i].timer = randfr(0.0f, 2.0f);
				rain[i].pos = vec3(randfr(0.0f, mapSize.x), randfr(0.0f, mapSize.y), randfr(20.0f, 30.0f));

				G_CreateParticle(vec3f2(rain[i].pos.xy, 0.1f), vec3(-randfr(0.25f, 0.5f), -randfr(0.25f, 0.5f), randfr(2.0f, 3.0f)));
				G_CreateParticle(vec3f2(rain[i].pos.xy, 0.1f), vec3( randfr(0.25f, 0.5f), -randfr(0.25f, 0.5f), randfr(2.0f, 3.0f)));
				G_CreateParticle(vec3f2(rain[i].pos.xy, 0.1f), vec3( randfr(0.25f, 0.5f),  randfr(0.25f, 0.5f), randfr(2.0f, 3.0f)));
				G_CreateParticle(vec3f2(rain[i].pos.xy, 0.1f), vec3(-randfr(0.25f, 0.5f),  randfr(0.25f, 0.5f), randfr(2.0f, 3.0f)));
			}
		}

		if (isRaining) {
			rain[i].timer -= dt;
		}
	}

	// PARTICLES
	for (int i=0; i<array_size(particles); ++i) {
		particle_t* p = particles + i;
		if (p->timer > 0) {
			if (p->pos.z > 0.0f) {
				p->pos = add3(p->pos, mul3f(p->speed, dt));
				p->speed.z -= dt*10.0f;
				if (p->pos.z < 0.0f) {
					p->timer = randfr(0.25f, 0.5f);
				}
			}

			p->timer -= dt;
		}
	}
}
