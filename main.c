//
//  Created by Matt Hartley on 30/05/2025.
//  Copyright 2025 GiantJelly. All rights reserved.
//


#define GL_SILENCE_DEPRECATION 1
// #include <OpenGL/OpenGL.h>

#include "game.c"
#include "draw.c"

#include <core/sysvideo.h>
#include <core/sysaudio.h>
#define CORE_IMPL
#include <core/core.h>
#include <core/math.h>
// #include <core/glgfx.h>


sys_window_t window;
sysaudio_t audio;
allocator_t memory;
allocator_t tmpMemory;

vec3_t shipMesh[] = {
	{-0.4f, -0.5f},
	{ 0.0f, -0.25f},
	{ 0.4f, -0.5f},
	{ 0.0f,  0.5f},
	{-0.4f, -0.5f},
};

int main()
{
	sys_init_log("./output.log");
	sys_init_window(&window, "Asteroids", 1920, 1080, WINDOW_CENTERED);
	sys_init_opengl(&window);
	// sys_init_audio(&audio, (sysaudio_spec_t){
	// 	.sampleRate = SYSAUDIO_SAMPLE_RATE_44K,
	// 	.format = SYSAUDIO_FORMAT_INT16,
	// });
	// sys_init_audio(&audio, SYSAUDIO_DEFAULT_SPEC);

	// audio_buffer_t* shootSound = load

	memory = virtual_heap_allocator(MB(10), MB(10));
	tmpMemory = virtual_heap_allocator(MB(1), MB(1));
	str_set_allocator(&tmpMemory);

	G_Init();
	D_Init();

	for (;;) {
		sys_poll_events(&window);

		G_Frame();
		D_DrawFrame();

		sys_zero_memory(tmpMemory.address, tmpMemory.commit);
		clear_allocator(&tmpMemory);
	}
}
