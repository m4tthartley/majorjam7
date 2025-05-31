//
//  Created by Matt Hartley on 31/05/2025.
//  Copyright 2025 GiantJelly. All rights reserved.
//


#ifdef __APPLE__
#	define GL_SILENCE_DEPRECATION 1
// #	include <OpenGL/gl.h>
#	include <OpenGL/glu.h>
#endif
#ifdef __WIN32__
#	include <gl/gl.h>
#	include <gl/glu.h>
#endif

#define CORE_IMPL
#include <core/glgfx.h>


#include "game.h"


extern sys_window_t window;
extern allocator_t memory;
bitmap_t* tileBitmap;
gfx_texture_t tileTex;

void CheckGLError()
{
	// char* err = gluErrorString(glGetError());
	GLenum err = glGetError();
	const GLubyte* errstr = gluErrorString(err);
	assert(err == GL_NO_ERROR);
}

void D_Init()
{
	tileBitmap = load_bitmap_file(&memory, "assets/land.bmp");
	tileTex = gfx_create_texture(tileBitmap);
}

void D_DrawFrame()
{
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT);
	CheckGLError();

	// glMatrixMode(GL_PROJECTION);
	// glLoadIdentity();
	// CheckGLError();
	float aspect = (float)window.fbWidth / (float)window.fbHeight;
	// glOrtho(-10*aspect, 10*aspect, -10, 10, -10, 10);
	// CheckGLError();
	// glMatrixMode(GL_MODELVIEW);
	// glLoadIdentity();

	gfx_ortho_projection_centered(window.width, window.height, 20*aspect, 20);

	glLoadIdentity();
	glTranslatef(-cameraPos.x, -cameraPos.y, 0);

	CheckGLError();

	// glColor4f(1, 1, 1, 1);
	// glBegin(GL_QUADS);
	// glVertex2f(-0.5f, -0.5f);
	// glVertex2f( 0.5f, -0.5f);
	// glVertex2f( 0.5f,  0.5f);
	// glVertex2f(-0.5f,  0.5f);
	// glEnd();

	// if (window.keyboard[KEY_LEFT].down) {
	// 	playerRot += 0.03f;
	// }
	// if (window.keyboard[KEY_RIGHT].down) {
	// 	playerRot -= 0.03f;
	// }
	// if (window.keyboard[KEY_UP].down) {
	// 	float len = len2(playerSpeed);
	// 	playerSpeed.x -= sinf(playerRot) * (0.001f);
	// 	playerSpeed.y += cosf(playerRot) * (0.001f);
	// }

	// playerPos = add2(playerPos, playerSpeed);
	// playerSpeed = mul2f(playerSpeed, 0.99f);
	// glTranslatef(playerPos.x, playerPos.y, 0);
	// glRotatef(todeg(playerRot), 0, 0, 1);
	// glColor4f(1, 1, 1, 1);
	// glBegin(GL_LINE_STRIP);
	// for (int i=0; i<array_size(shipMesh); ++i) {
	// 	glVertex2f(shipMesh[i].x, shipMesh[i].y);
	// }
	// glEnd();

	gfx_sprite_tile_size(32);
	gfx_sprite_scale(2);
	glEnable(GL_TEXTURE_2D);
	gfx_texture(&tileTex);
	// gfx_draw_sprite_tile(vec2(0, 0), 2);

	for (int y=0; y<MAP_SIZE_Y; ++y)
	for (int x=0; x<MAP_SIZE_X; ++x) {
		tile_t* tile = &map[y*MAP_SIZE_X + x];
		if (tile->type == TILE_GRASS) {
			gfx_draw_sprite_tile(vec2((float)-MAP_SIZE_X/2 + x, (float)-MAP_SIZE_Y/2 + y), 56);
		}
		if (tile->type == TILE_SEA) {
			gfx_draw_sprite_tile(vec2((float)-MAP_SIZE_X/2 + x, (float)-MAP_SIZE_Y/2 + y), 40);
		}
	}

	// glEnable(GL_TEXTURE_2D);
	// gfx_texture(&tileTex);
	// glBegin(GL_QUADS);
	// glTexCoord2f(0.0f, 0.0f);
	// glVertex2f(-5.0f, -5.0f);
	// glTexCoord2f(1.0f, 0.0f);
	// glVertex2f( 5.0f, -5.0f);
	// glTexCoord2f(1.0f, 1.0f);
	// glVertex2f( 5.0f,  5.0f);
	// glTexCoord2f(0.0f, 1.0f);
	// glVertex2f(-5.0f,  5.0f);
	// glEnd();
	// glDisable(GL_TEXTURE_2D);
	
	CheckGLError();

	sys_present_opengl(&window);
}