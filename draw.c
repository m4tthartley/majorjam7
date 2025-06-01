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
bitmap_t* playerBitmap;
gfx_texture_t tileTex;
gfx_texture_t playerTex;
gfx_texture_t fontTex;
gfx_framebuffer_t framebuffer;

char* tileVertexShader = "\
#version 120\n\
\
attribute vec2 aPos;\
attribute vec2 aTexCoord;\
\
uniform mat4 uProj;\
\
varying vec2 vTexCoord;\
\
void main() {\
	gl_Position = uProj * vec4(aPos, 0, 1);\
	vTexCoord = aTexCoord;\
}\
";
char* tileFragmentShader = "\
#version 120\n\
\
uniform sampler2D uTexture;\
\
varying vec2 vTexCoord;\
\
void main() {\
	gl_FragColor = texture2D(uTexture, vTexCoord);\
}\
";

void CheckGLError()
{
	// char* err = gluErrorString(glGetError());
	GLenum err = glGetError();
	const GLubyte* errstr = gluErrorString(err);
	assert(err == GL_NO_ERROR);
}

void D_Init()
{
	framebuffer = gfx_create_framebuffer(1280 / 2, 720 / 2, GL_RGBA, GL_NEAREST);

	tileBitmap = load_bitmap_file(&memory, "assets/land.bmp");
	playerBitmap = load_bitmap_file(&memory, "assets/character.bmp");
	tileTex = gfx_create_texture(tileBitmap);
	playerTex = gfx_create_texture(playerBitmap);
	fontTex = gfx_generate_font_texture(&memory, &FONT_DEFAULT);
}

void D_DrawSpriteRect(v2 pos, v2 sprite_offset, v2 sprite_size) {
	vec2_t vertex_offset = div2f(vec2(sprite_size.x/2, sprite_size.y/2), _gfx_sprite_tile_size);
	
	float twidth = _gfx_active_texture->width;
	float theight = _gfx_active_texture->height;

	glBegin(GL_QUADS);

	glTexCoord2f(sprite_offset.x / twidth, sprite_offset.y / theight);
	glVertex2f(pos.x - vertex_offset.x, pos.y - vertex_offset.y);

	glTexCoord2f((sprite_offset.x+sprite_size.x) / twidth, sprite_offset.y / theight);
	glVertex2f(pos.x + vertex_offset.x, pos.y - vertex_offset.y);

	glTexCoord2f((sprite_offset.x+sprite_size.x) / twidth, (sprite_offset.y+sprite_size.y) / theight);
	glVertex2f(pos.x + vertex_offset.x, pos.y + vertex_offset.y);

	glTexCoord2f(sprite_offset.x / twidth, (sprite_offset.y+sprite_size.y) / theight);
	glVertex2f(pos.x - vertex_offset.x, pos.y + vertex_offset.y);

	glEnd();
}

void D_DrawSpriteTile(v2 pos, int tile) {
	assert(_gfx_active_texture);
	int tiles_per_row = _gfx_active_texture->width / _gfx_sprite_tile_size;
	int tiles_per_column = _gfx_active_texture->height / _gfx_sprite_tile_size;
	tile %= (tiles_per_row * tiles_per_column);
	D_DrawSpriteRect(
		pos,
		vec2((tile%tiles_per_row)*_gfx_sprite_tile_size, (tile/tiles_per_row)*_gfx_sprite_tile_size),
		vec2(_gfx_sprite_tile_size, _gfx_sprite_tile_size)
	);
}

void D_DrawFrame()
{
	glClearColor(1, 0, 0, 1);
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

	// gfx_ortho_projection_centered(window.width, window.height, 20*aspect, 20);
	gfx_ortho_projection_centered(window.width, window.height, mapSize.x, mapSize.y);

	glLoadIdentity();
	glTranslatef(-cameraPos.x, -cameraPos.y, 0);

	glEnable(GL_BLEND);
	// glAlphaFunc(GLenum func, GLclampf ref)
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	gfx_bind_framebuffer(&framebuffer);
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT);

	gfx_sprite_tile_size(32);
	gfx_sprite_scale(1);
	glEnable(GL_TEXTURE_2D);
	gfx_texture(&tileTex);
	glColor4f(1, 1, 1, 1);

	// TILES
	for (int y=0; y<mapSize.y; ++y) {
		for (int x=0; x<mapSize.x; ++x) {
			tile_t* tile = &mapTiles[y*mapSize.x + x];
			vec2_t pos = vec2((float)-mapSize.x/2 + x + 0.5f, (float)-mapSize.y/2 + y + 0.5f);
			glPushMatrix();

			glTranslatef(pos.x, pos.y, 0);
			if (tile->type == TILE_GRASS) {
				// glRotatef(tile->rotation*90, 0, 0, 1);
				D_DrawSpriteTile(vec2f(0), tile->water>0.5f ? 25 : 24);

				// glDisable(GL_TEXTURE_2D);
				// glColor4f(0, 0, 1, 1);
				// gfx_line(vec2f(0), vec2(tile->water, 0));
				// glEnable(GL_TEXTURE_2D);
				// glColor4f(1, 1, 1, 1);
			}
			if (tile->type == TILE_MARSH) {
				D_DrawSpriteTile(vec2f(0), tile->water>0.5f ? 57 : 56);
			}
			if (tile->type == TILE_SEA) {
				D_DrawSpriteTile(vec2f(0), 32 + (int)tile->ani % 3);
			}

			if (tile->type != TILE_SEA && (y-1)*mapSize.x + x < mapTotalTiles) {
				if (mapTiles[(y-1)*mapSize.x + x].type == TILE_SEA) {
					// glScalef(0.9f, 0.9f, 1);
					glTranslatef(0, -0.75f, 1);
					// D_DrawSpriteTile(vec2f(0), 48);
					D_DrawSpriteRect(vec2f(0), vec2(64, 192), vec2(32, 16));
				}
			}

			glPopMatrix();
		}
	}

	gfx_texture(&fontTex);
	for (int y=0; y<mapSize.y; ++y) {
		for (int x=0; x<mapSize.x; ++x) {
			tile_t* tile = &mapTiles[y*mapSize.x + x];
			vec2_t pos = vec2((float)-mapSize.x/2 + x + 0.5f, (float)-mapSize.y/2 + y + 0.5f);
			// gfx_draw_text(&FONT_DEFAULT, pos, "Hi");
		}
	}

	// PLAYER
	{
		vec2_t pos = vec2((float)-mapSize.x/2 + player.pos.x + 0.5f, (float)-mapSize.y/2 + player.pos.y + 0.5f);
		gfx_texture(&playerTex);
		glPushMatrix();
		glTranslatef(pos.x, pos.y, 0);
		D_DrawSpriteTile(vec2(0, 0.5f), 48 + (int)player.ani % 3);
		D_DrawSpriteTile(vec2(0, 0.5f + 1), 48 + 8 + (int)player.ani % 3);
		glDisable(GL_TEXTURE_2D);
		gfx_point(vec2f(0));
		glPopMatrix();

		vec2_t selectedPos = vec2((float)-mapSize.x/2 + selectionPos.x + 0.5f, (float)-mapSize.y/2 + selectionPos.y + 0.5f);
		glPushMatrix();
		glTranslatef(selectedPos.x, selectedPos.y, 0);
		glDisable(GL_TEXTURE_2D);
		glColor4f(1, 0, 0, 1);
		gfx_point(vec2f(0));

		// gfx_point(selectedPos);
		glPopMatrix();

		if (player.tile && player.tile->type != TILE_SEA) {
			gfx_point(vec2((float)-mapSize.x/2 + player.tilePos.x + 0.5f, (float)-mapSize.y/2 + player.tilePos.y + 0.5f));
			gfx_line_quad(vec2((float)-mapSize.x/2 + player.tilePos.x + 0.5f, (float)-mapSize.y/2 + player.tilePos.y + 0.5f), vec2f(1.0f));
		}
	}

	// RAIN
	glDisable(GL_TEXTURE_2D);
	glPointSize(2.0f);
	// glPushMatrix();
	glTranslatef((float)-mapSize.x/2, (float)-mapSize.y/2, 0);
	glColor4f(0, 0, 1, 1);
	for (int i=0; i<array_size(rain); ++i) {
		vec2_t pos = vec2(rain[i].pos.x, rain[i].pos.y + (rain[i].pos.z*0.5f));
		gfx_point(pos);
	}
	// glPopMatrix();
	
	// PARTICLES
	for (int i=0; i<array_size(particles); ++i) {
		if (particles[i].timer > 0) {
			vec2_t pos = vec2(particles[i].pos.x, particles[i].pos.y + (particles[i].pos.z*0.5f));
			// glColor4f(1, 0, 1, 1);
			gfx_point(pos);

			// vec2_t ground = vec2(particles[i].pos.x, particles[i].pos.y);
			// glColor4f(0, 1, 0, 1);
			// gfx_line(pos, ground);
		}
	}


	// PRESENT FRAMEBUFFER
	gfx_bind_window_framebuffer(&window);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	// gfx_texture(&framebuffer.textures[0]);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, framebuffer.textures[0]);
	glColor4f(1, 1, 1, 1);
	// glDisable(GL_TEXTURE_2D);

	// gfx_draw_sprite_rect(vec2(0,0), vec2f(0), vec2(framebuffer.width, framebuffer.height));
	// glScalef(0.95f, 0.95f, 1);
	// glTranslatef(0, -1.0f, 0);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f); glVertex2f(-1.0f, -1.0f);
	glTexCoord2f(1.0f, 0.0f); glVertex2f( 1.0f, -1.0f);
	glTexCoord2f(1.0f, 1.0f); glVertex2f( 1.0f,  1.0f);
	glTexCoord2f(0.0f, 1.0f); glVertex2f(-1.0f,  1.0f);
	glEnd();

	sys_present_opengl(&window);
}