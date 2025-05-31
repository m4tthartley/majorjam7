//
//  Created by Matt Hartley on 31/05/2025.
//  Copyright 2025 GiantJelly. All rights reserved.
//


#include <core/sysvideo.h>
#include <core/time.h>
#include "game.h"


extern sys_window_t window;
frametimer_t timer;


void G_Init()
{
	timer = frametimer_init();

	for (int i=0; i<array_size(map); ++i) {
		float x = i%MAP_SIZE_X;
		float y = i/MAP_SIZE_Y;
		float height = fbm(vec2(x/15.0f, y/15.0f));

		map[i].type = TILE_SEA;
		if (height > 0.5f) {
			map[i].type = TILE_GRASS;
		}
	}
}

void G_Frame()
{
	frametimer_update(&timer);
	float dt = timer.dt;

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
}
