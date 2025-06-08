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
		.blowAwayTimer = randfr(60.0f, 120.0f),
	};

	player.tile->plant = plant;
}

void G_ResetRainDrop(raindrop_t* drop, _Bool init)
{
	// if (init) {
		drop->timer = randfr(0.0f, 5.0f);
	// } else {
		// drop->timer = 0.0f;
	// }
	drop->pos = vec3(randfr(0.0f, mapSize.x) + 6.0f, randfr(0.0f, mapSize.y), randfr(25.0f, 25.0f));
}

void G_ResetAllRain()
{
	for (int i=0; i<array_size(rain); ++i) {
		G_ResetRainDrop(rain + i, _True);
	}
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
		if (mapIslandMask[i] == 1) {
			mapTiles[i].type = TILE_DIRT;
			if (height > 0.55f) {
				mapTiles[i].type = TILE_MARSH;
			}
		}
		if (mapIslandMask[i] == 2) {
			mapTiles[i].type = TILE_HOUSE_DIRT;
		}
		float r = randfr(0.0f, 4.0f);
		mapTiles[i].rotation = floorf(r);
	}

	G_ResetAllRain();

	player.pos = vec2((float)mapSize.x/2, (float)mapSize.y/2);
}

void G_Frame()
{
	frametimer_update(&timer);
	float dt = timer.dt;

	static _Bool returnPressed = _False;
	if (window.keyboard[KEY_RETURN].pressed) {
		returnPressed = _True;
	}

	// WEATHER
	weatherEventTimer -= dt;
	if (weatherEventTimer < 0.0f) {
		weatherEventTimer = randfr(60.0f, 120.0f);
		// weatherEventTimer = randfr(15.0f, 30.0f);
		print("A weather event is occuring.. \n");

		if (!(currentWeather==WEATHER_RAIN || currentWeather==WEATHER_HEAVY_RAIN || currentWeather==WEATHER_STORM) &&
			(queuedWeather==WEATHER_RAIN || queuedWeather==WEATHER_HEAVY_RAIN || queuedWeather==WEATHER_STORM)) {
			G_ResetAllRain();
		}

		weather_t weightedWeather[] = {
			WEATHER_CALM,
			WEATHER_CALM,
			WEATHER_CALM,
			WEATHER_CALM,
			WEATHER_CALM,
			WEATHER_CALM,
			WEATHER_RAIN,
			WEATHER_RAIN,
			WEATHER_RAIN,
			WEATHER_WIND,
			WEATHER_WIND,
			WEATHER_WIND,
			WEATHER_HEAVY_RAIN,
			WEATHER_HEAVY_RAIN,
			WEATHER_STORM,
		};

		currentWeather = queuedWeather;
		while ((queuedWeather = weightedWeather[randr(0, array_size(weightedWeather))]) == currentWeather);
	}

	if (menuShop) {
		// SHOP
		if (window.keyboard[KEY_ESC].released) {
			menuShop = _False;
		}

		if (window.keyboard[KEY_DOWN].released) {
			shopSelected = smin(shopSelected + 1, array_size(plantDefs)-1);
		}
		if (window.keyboard[KEY_UP].released) {
			shopSelected = smax(shopSelected - 1, 0);
		}
		if (window.keyboard[KEY_LEFT].released && shopSelected >= 5) {
			shopSelected -= 5;
		}
		if (window.keyboard[KEY_RIGHT].released && shopSelected < 4) {
			shopSelected += 5;
		}

		if (returnPressed && window.keyboard[KEY_RETURN].released && player.tile) {
			if (money >= plantDefs[shopSelected].cost) {
				AddPlant(plantDefs[shopSelected]);
				money -= plantDefs[shopSelected].cost;
				menuShop = _False;
				returnPressed = _False;
			}
		}
	} else {
		// PLAYER CONTROLS
		// if (window.keyboard[KEY_A].down) {
		// 	cameraPos.x -= 10.0f * dt;
		// }
		// if (window.keyboard[KEY_D].down) {
		// 	cameraPos.x += 10.0f * dt;
		// }
		// if (window.keyboard[KEY_S].down) {
		// 	cameraPos.y -= 10.0f * dt;
		// }
		// if (window.keyboard[KEY_W].down) {
		// 	cameraPos.y += 10.0f * dt;
		// }

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

		// if (window.keyboard[KEY_R].released) {
		// 	currentWeather = WEATHER_HEAVY_RAIN;
		// }

		// if (window.keyboard[KEY_P].released) {
		// 	weatherEventTimer = 0.0f;
		// }
	}

	// TILE SELECTION
	vec2_t selectionOffset = mul2f(add2(dirVectors[player.aniDir], vec2(0, 0.25f)), 1.0f);
	selectionPos = add2(player.pos, selectionOffset);
	int2_t pos = int2(selectionPos.x + 0.5f, selectionPos.y + 0.5f);
	player.tilePos = pos;
	pos.x = smax(smin(pos.x, mapSize.x-1), 0);
	pos.y = smax(smin(pos.y, mapSize.y-1), 0);
	player.tile = &mapTiles[pos.y*mapSize.x + pos.x];
	if (player.tile->type != TILE_SEA && player.tile->type != TILE_HOUSE_DIRT) {
		if (returnPressed && window.keyboard[KEY_RETURN].released) {
			if (player.tile->plant.alive) {
				if (player.tile->plant.stage==2) {
					money += player.tile->plant.def.profit;
					if (player.tile->plant.def.reharvestable) {
						player.tile->plant.stageTimer = 30.0f;
						player.tile->plant.stage = 1;
					} else {
						player.tile->plant = (plant_t){0};
					}
				}
			} else {
				menuShop = _True;
				returnPressed = _False;
			}
		}

		if (!player.tile->plant.windPosts && !(player.tile->plant.def.flags & PLANT_LIKES_WIND) && window.keyboard[KEY_1].released) {
			if (money >= 50) {
				player.tile->plant.windPosts = _True;
				money -= 50;
			}
		}
	}

	// TILES
	for (int i=0; i<mapTotalTiles; ++i) {
		tile_t* t = mapTiles + i;
		t->ani += dt * 1.0f;

		if (t->water > 0.0f) {
			if (currentWeather == WEATHER_RAIN || currentWeather == WEATHER_HEAVY_RAIN) {
				t->water -= 0.01f * dt;
			} else {
				t->water -= 0.05f * dt;
			}
		}

		if (t->plant.alive) {
			// t->water = max(t->water - 0.1f*dt, 0.0f);
			t->plant.stageTimer -= dt;
			if (t->plant.stageTimer <= 0.0f) {
				t->plant.stage = umin(t->plant.stage+1, 2);
				t->plant.stageTimer = 30.0f;
			}

			uint32_t likesWater = t->plant.def.flags & PLANT_LIKES_WATER;
			if (!likesWater && t->water > 1.0f) {
				t->plant.health -= 0.01f * dt;
			}

			if (t->plant.health <= 0.0f) {
				t->plant.alive = _False;
			}

			if (!(t->plant.def.flags & PLANT_LIKES_WIND) && currentWeather == WEATHER_WIND || currentWeather == WEATHER_STORM) {
				t->plant.blowAwayTimer -= dt;
			}
			if (t->plant.blowAwayTimer <= 0.0f) {
				// if (!windDebris.plantDef) {
				// 	windDebris = (wind_debris_t){
				// 		.plantDef = &plantDefs[t->plant.def.type],
				// 		.pos = vec2(i%mapSize.x, i/mapSize.x),
				// 	};
				// }
				t->plant.blowAway = _True;
			}

			if (t->plant.blowAway && !t->plant.windPosts) {
				t->plant.blowAwayPos.x -= 8.0f * dt;
				t->plant.blowAwayPos.y += 1.0f * dt;
				t->plant.blowAwayRotation += 0.5f * dt;

				if (t->plant.blowAwayPos.x < (float)-mapSize.x/2 - 2.0f) {
					t->plant.alive = _False;
				}
			}

			if (t->plant.burned) {
				t->plant.burnedTimer -= dt;
				if (t->plant.burnedTimer <= 0.0f) {
					t->plant.alive = _False;
				}
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

				// rain[i].timer = randfr(0.0f, 0.0f);
				// rain[i].pos = vec3(randfr(0.0f, mapSize.x), randfr(0.0f, mapSize.y), randfr(20.0f, 20.0f));

				G_CreateParticle(vec3f2(rain[i].pos.xy, 0.1f), vec3(-randfr(0.25f, 0.5f), -randfr(0.25f, 0.5f), randfr(2.0f, 3.0f)));
				G_CreateParticle(vec3f2(rain[i].pos.xy, 0.1f), vec3( randfr(0.25f, 0.5f), -randfr(0.25f, 0.5f), randfr(2.0f, 3.0f)));
				G_CreateParticle(vec3f2(rain[i].pos.xy, 0.1f), vec3( randfr(0.25f, 0.5f),  randfr(0.25f, 0.5f), randfr(2.0f, 3.0f)));
				G_CreateParticle(vec3f2(rain[i].pos.xy, 0.1f), vec3(-randfr(0.25f, 0.5f),  randfr(0.25f, 0.5f), randfr(2.0f, 3.0f)));

				G_ResetRainDrop(rain + i, _False);
			}
		}

		if (currentWeather == WEATHER_RAIN) {
			if (i <= (RAIN_COUNT/4)) {
				rain[i].timer -= dt;
			}
		}
		else if (currentWeather == WEATHER_HEAVY_RAIN || currentWeather == WEATHER_STORM) {
			rain[i].timer -= dt;
		}
	}

	// WIND
	for (int i=0; i<array_size(wind); ++i) {
		wind_t* w = wind + i;

		if (currentWeather == WEATHER_WIND || currentWeather == WEATHER_STORM) {
			w->timer -= dt;
		}
		if (w->timer <= 0.0f) {
			if (w->type == WIND_SWIRL) {
				w->pos.x -= 10.0f * dt;
			} else {
				w->pos.x -= 20.0f * dt;
				w->pos.y -= 2.0f * dt;
			}
			w->ani += dt * 10.0f;

			if (w->pos.x < -1.0f) {
				w->pos.x = (float)mapSize.x + 1.0f;
				w->pos.y = randfr(0.0f, (float)mapSize.y);
				w->timer = randfr(1.0f, 10.0f);
				w->ani = randfr(-1.0f, 0.0f) * 10.0f;
				w->type = randr(0, 4);
			}
		}
	}

	// WIND DEBRIS
	// if (windDebrisTimer <= 0.0f) {
	// 	windDebrisTimer = randfr(10.0f, 20.0f);

	// 	for (int i=0; i<mapTotalTiles; ++i) {
	// 		if ()
	// 	}
	// }
	// if (windDebris.plantDef) {

	// }

	// LIGHTNING
	if (currentWeather == WEATHER_STORM) {
		lightningStrikeTimer -= dt;
	}
	if (lightningStrikeTimer < 0.0f) {
		print("LIghtning STRIKE \n");
		tile_t* hitTile = &mapTiles[lightningStrikePos.y*mapSize.x + lightningStrikePos.x];
		if (!(hitTile->plant.def.flags & PLANT_LIGHTNING_RESISTANT)) {
			hitTile->plant.burned = _True;
			hitTile->plant.burnedTimer = 5.0f;
		}

		lightningStrikeTimer = randfr(5.0f, 10.0f);
		lightningStrikePos = int2(randr(1, mapSize.x-1), randfr(1, mapSize.y-1));

		for (int i=0; i<array_size(lightningVertices); ++i) {
			lightningVertices[i] = vec2(i%2==1 ? randfr(0.0f, 0.5f) : randfr(0.5f, 1.0f), randfr(-0.5f, 0.5f) + (float)i + 1.0f);
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
