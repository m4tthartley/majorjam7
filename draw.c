//
//  Created by Matt Hartley on 31/05/2025.
//  Copyright 2025 GiantJelly. All rights reserved.
//


#include <core/sys.h>

#include <core/targetconditionals.h>
#ifdef __APPLE__
#	define GL_SILENCE_DEPRECATION 1
// #	include <OpenGL/gl.h>
#	include <OpenGL/glu.h>
#endif
#ifdef __WIN32__
#	include <gl/gl.h>
#	include <gl/glu.h>
#endif

#include <core/glgfx.h>


#include "game.h"


extern sys_window_t window;
extern allocator_t memory;
int2_t framebufferSize;

bitmap_t* tileBitmap;
bitmap_t* playerBitmap;
gfx_texture_t tileTex;
gfx_texture_t playerTex;
gfx_texture_t fontTex;
gfx_texture_t plantTex;
gfx_texture_t buildingTex;
gfx_texture_t iconsTex;
gfx_texture_t windTex;
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

gfx_texture_t LoadBitmapAndCreateTexture(char* file)
{
	// char buffer[MAX_PATH_LENGTH] = {0};
	// sys_get_resource_path(buffer, MAX_PATH_LENGTH, file);

	// file_t f = sys_open(buffer);
	// if (!f) {
	// 	// char_copy(buffer, file, str_len(file));
	// 	strcpy(buffer, file);
	// }
	// sys_close(f);

	bitmap_t* bitmap = load_bitmap_file(&memory, file);
	gfx_texture_t texture = gfx_create_texture(bitmap);
	return texture;
}

void D_Init()
{
	framebufferSize = int2(1280 / 2, 720 / 2);
	framebuffer = gfx_create_framebuffer(framebufferSize.x, framebufferSize.y, GL_RGBA, GL_NEAREST);

	// tileBitmap = load_bitmap_file(&memory, "assets/land.bmp");
	// playerBitmap = load_bitmap_file(&memory, "assets/character.bmp");
	// tileTex = gfx_create_texture(tileBitmap);
	// playerTex = gfx_create_texture(playerBitmap);

	// plantTex = gfx_create_texture(load_bitmap_file(&memory, "assets/plants.bmp"));
	// buildingTex = gfx_create_texture(load_bitmap_file(&memory, "assets/buildings.bmp"));
	// iconsTex = gfx_create_texture(load_bitmap_file(&memory, "assets/icons.bmp"));
	// windTex = gfx_create_texture(load_bitmap_file(&memory, "assets/wind.bmp"));

	fontTex = gfx_generate_font_texture(&memory, &FONT_DEFAULT);

	tileTex = LoadBitmapAndCreateTexture("assets/land.bmp");
	playerTex = LoadBitmapAndCreateTexture("assets/character.bmp");
	plantTex = LoadBitmapAndCreateTexture("assets/plants.bmp");
	buildingTex = LoadBitmapAndCreateTexture("assets/buildings.bmp");
	iconsTex = LoadBitmapAndCreateTexture("assets/icons.bmp");
	windTex = LoadBitmapAndCreateTexture("assets/wind.bmp");
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

	// gfx_ortho_projection_centered(window.width, window.height, 20*aspect, 20);
	gfx_ortho_projection_centered(framebufferSize.x, framebufferSize.y, mapSize.x, mapSize.y);

	glLoadIdentity();
	glTranslatef(-cameraPos.x, -cameraPos.y, 0);

	glEnable(GL_BLEND);
	// glAlphaFunc(GLenum func, GLclampf ref)
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	gfx_bind_framebuffer(&framebuffer);
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT);

	glEnable(GL_TEXTURE_2D);
	gfx_sprite_tile_size(32);
	gfx_sprite_scale(1);
	
	// TILES
	gfx_texture(&tileTex);
	glColor4f(1, 1, 1, 1);
	for (int y=0; y<mapSize.y; ++y) {
		for (int x=0; x<mapSize.x; ++x) {
			// gfx_texture(&tileTex);
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
			if (tile->type == TILE_DIRT || tile->type == TILE_HOUSE_DIRT) {
				D_DrawSpriteTile(vec2f(0), tile->water>0.5f ? 49 : 48);
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
					D_DrawSpriteRect(vec2f(0), vec2(((int)tile->ani + x%2)%2 ? 32 : 0, 64), vec2(32, 32));
				}
			}

			glPopMatrix();
		}
	}

	gfx_texture(&buildingTex);
	gfx_draw_sprite_rect(vec2(2, 5), vec2(0, 0), vec2(112, 96));

#if 0
	gfx_texture(0);
	for (int y=0; y<mapSize.y; ++y) {
		for (int x=0; x<mapSize.x; ++x) {
			tile_t* tile = &mapTiles[y*mapSize.x + x];
			if (tile->plant.alive) {
				vec2_t pos = vec2((float)-mapSize.x/2 + x + 0.5f, (float)-mapSize.y/2 + y + 0.5f);
				gfx_color(vec4(0, 1, 0, 1));
				gfx_line_circle(add2(pos, vec2(0.4f, 0.25f)), tile->plant.health * 0.25f, 10);
				gfx_color(vec4(0, 0, 1, 1));
				gfx_line_circle(add2(pos, vec2(0.4f, -0.25f)), tile->water * 0.25f, 10);
				gfx_color(vec4(1, 0, 0, 1));
				gfx_line_circle(add2(pos, vec2(0.4f, -0.75f)), tile->plant.blowAwayTimer * 0.01f, 10);
			}
		}
	}
#endif

	// PLANTS
	gfx_color(vec4(1, 1, 1, 1));
	for (int y=0; y<mapSize.y; ++y) {
		for (int x=0; x<mapSize.x; ++x) {
			tile_t* tile = &mapTiles[y*mapSize.x + x];
			if (tile->plant.alive) {
				gfx_texture(&plantTex);
				int spriteStage = umin(tile->plant.stage, tile->plant.def.spriteStages-1);
				if (tile->plant.blowAway) {
					glPushMatrix();
					glTranslatef(tile->plant.blowAwayPos.x, tile->plant.blowAwayPos.y, 0);
					glRotatef(todeg(tile->plant.blowAwayRotation), 0, 0, 1);
					gfx_draw_sprite_rect(
						vec2f(0),
						add2(tile->plant.def.spriteOffset, vec2(tile->plant.def.spriteSize.x * spriteStage, 0)),
						tile->plant.def.spriteSize
					);
					glPopMatrix();
				} else {
					if (tile->plant.burned) {
						gfx_color(vec4(0, 0, 0, 1));
					} else {
						gfx_color(vec4(1, 1, 1, 1));
					}
					vec2_t pos = vec2((float)-mapSize.x/2 + x + 0.5f, (float)-mapSize.y/2 + y + 0.5f);
					// gfx_draw_text(&FONT_DEFAULT, pos, "Hi");
					tile->plant.blowAwayPos = add2(pos, vec2(0, tile->plant.def.spriteSize.y/64 + tile->plant.def.tileOffset));
					gfx_draw_sprite_rect(
						add2(pos, vec2(0, tile->plant.def.spriteSize.y/64 + tile->plant.def.tileOffset)),
						add2(tile->plant.def.spriteOffset, vec2(tile->plant.def.spriteSize.x * spriteStage, 0)),
						tile->plant.def.spriteSize
					);

					if (tile->plant.windPosts) {
						gfx_texture(&iconsTex);
						if (tile->plant.def.type == PLANT_BANANA || tile->plant.def.type == PLANT_MINT || tile->plant.def.type == PLANT_CRANBERRY) {
							gfx_draw_sprite_rect(add2(pos, vec2(0, 0.5f)), vec2(64, 128), vec2(64, 64));
						} else {
							gfx_draw_sprite_rect(add2(pos, vec2(0, 0.5f)), vec2(0, 128), vec2(64, 64));
						}
					}

					if (tile->plant.stage >= 2) {
						gfx_texture(&iconsTex);
						gfx_draw_sprite_rect(add2(pos, vec2(0.25f, 0.5f)), vec2(32, 192), vec2(32, 32));
					}
				}
			}
		}
	}

	// PLAYER
	gfx_color(vec4(1, 1, 1, 1));
	{
		vec2_t pos = vec2((float)-mapSize.x/2 + player.pos.x + 0.5f, (float)-mapSize.y/2 + player.pos.y + 0.5f);
		gfx_texture(&playerTex);
		glPushMatrix();
		glTranslatef(pos.x, pos.y, 0);

		int ani = player.ani;
		if (player.aniDir==1 || player.aniDir==3) {
			ani %= 2;
		} else {
			ani %= 4;
		}
		int aniTile = (48 - (player.aniDir*16)) + ani;
		if (player.aniDir == 3) {
			aniTile += 32;
			glRotatef(180, 0, 1, 0);
		}

		D_DrawSpriteTile(vec2(0, 0.5f), aniTile);
		D_DrawSpriteTile(vec2(0, 0.5f + 1), aniTile + 8);
		// D_DrawSpriteRect(vec2(0, 0.5f), vec2(0, 192), vec2(32, 32));
		// D_DrawSpriteRect(vec2(0, 0.5f + 1), vec2(0, 192+32), vec2(32, 30));

		// glDisable(GL_TEXTURE_2D);
		gfx_texture(0);
		// gfx_point(vec2f(0));
		glPopMatrix();

		vec2_t selectedPos = vec2((float)-mapSize.x/2 + selectionPos.x + 0.5f, (float)-mapSize.y/2 + selectionPos.y + 0.5f);
		glPushMatrix();
		glTranslatef(selectedPos.x, selectedPos.y, 0);
		// glDisable(GL_TEXTURE_2D);
		// glColor4f(1, 0, 0, 1);
		// gfx_point(vec2f(0));

		// gfx_point(selectedPos);
		glPopMatrix();

		if (player.tile && player.tile->type != TILE_SEA && player.tile->type != TILE_HOUSE_DIRT) {
			gfx_point(vec2((float)-mapSize.x/2 + player.tilePos.x + 0.5f, (float)-mapSize.y/2 + player.tilePos.y + 0.5f));
			gfx_line_quad(vec2((float)-mapSize.x/2 + player.tilePos.x + 0.5f, (float)-mapSize.y/2 + player.tilePos.y + 0.5f), vec2f(1.0f));
		}
	}

	gfx_texture(&plantTex);
	D_DrawSpriteRect(vec2(0, 0), vec2(0, 0), vec2(256, 256));

	// RAIN
	glDisable(GL_TEXTURE_2D);
	// glColor4f(39.0f/255.0f, 137.0f/255.0f, 205.0f/255.0f, 1);
	glColor4f(66.0f/255.0f, 191.0f/255.0f, 232.0f/255.0f, 1);
	// glColor4f(184.0f/255.0f, 204.0f/255.0f, 216.0f/255.0f, 1);
	glPointSize(1.0f);
	// glPushMatrix();
	glTranslatef((float)-mapSize.x/2, (float)-mapSize.y/2, 0);
	for (int i=0; i<array_size(rain); ++i) {
		vec2_t pos = vec2(rain[i].pos.x, rain[i].pos.y + (rain[i].pos.z*0.5f));
		// gfx_point(pos);
		gfx_line(pos, add2(pos, mul2f(normalize2(vec2(1, 2)), 0.125f)));
	}
	// glPopMatrix();
	
	// PARTICLES
	for (int i=0; i<array_size(particles); ++i) {
		if (particles[i].timer > 0) {
			vec2_t pos = vec2(particles[i].pos.x, particles[i].pos.y + (particles[i].pos.z*0.5f));
			// glColor4f(1, 0, 1, 1);
			gfx_point(pos);
			// gfx_line(pos, add2(pos, vec2(0, 0.25f)));

			// vec2_t ground = vec2(particles[i].pos.x, particles[i].pos.y);
			// glColor4f(0, 1, 0, 1);
			// gfx_line(pos, ground);
		}
	}

	// WIND
	gfx_texture(&windTex);
	gfx_color(vec4(1, 1, 1, 1));
	for (int i=0; i<array_size(wind); ++i) {
		wind_t* w = wind + i;

		if (w->type == WIND_SWIRL) {
			if (w->ani > 0.0f) {
				gfx_draw_sprite_tile(w->pos.xy, min((int)w->ani, 11));
			}
		} else {
			gfx_draw_sprite_tile(w->pos.xy, 12);
		}
	}

	// LIGHTNING
	if (lightningStrikeTimer > 0.0f && lightningStrikeTimer <= 0.125f) {
		gfx_texture(0);
		// gfx_color(vec4(1, 1, 1, 0.5f));
		glColor4f(201.0f/255.0f, 212.0f/255.0f, 253.0f/255.0f, (lightningStrikeTimer*8.0f));
		gfx_quad(vec2((float)mapSize.x/2, (float)mapSize.y/2), vec2(mapSize.x, mapSize.y));

		glColor4f(201.0f/255.0f, 212.0f/255.0f, 253.0f/255.0f, 1);
		vec2_t strikePos = vec2((float)lightningStrikePos.x + 0.5f, (float)lightningStrikePos.y + 0.5f);
		vec2_t start = strikePos;
		for (int i=0; i<array_size(lightningVertices); ++i) {
			vec2_t point = add2(strikePos, lightningVertices[i]);
			gfx_line(start, point);
			start = point;
		}
	}

	// GLOBAL TEXT
	glLoadIdentity();
	glDisable(GL_TEXTURE_2D);
	gfx_texture(0);

	char str[64];
	sprint(str, 64, "Weather: %s - Incoming weather warning: %s, %is", weatherNames[currentWeather], weatherNames[queuedWeather], (int)weatherEventTimer);
	vec2_t strSize = gfx_layout_text(&FONT_DEFAULT, str);

	char tileStr[64] = {0};
	if (player.tile) {
		sprint(tileStr, 64, tileNames[player.tile->type]);
		if (player.tile->plant.alive) {
			char plantStr[64];
			sprint(plantStr, 64, " / %s, stage %i", plantDefs[player.tile->plant.def.type].name, (int)player.tile->plant.stageTimer);
			// char_append(tileStr, " / ", 64);
			// char_append(tileStr, plantDefs[player.tile->plant.def.type].name, 64);
			char_append(tileStr, plantStr, 64);
		}
	}

	char moneyStr[64];
	sprint(moneyStr, 64, "Money: $%u", money);

	glBegin(GL_QUADS);
	glColor4f(0.0f, 0.0f, 0.0f, 0); glVertex3f((float)-mapSize.x/2, (float)mapSize.y/2 - 0.5f, 0);
	glColor4f(0.0f, 0.0f, 0.0f, 0); glVertex3f((float)mapSize.x/2,   (float)mapSize.y/2 - 0.5f, 0);
	glColor4f(0.0f, 0.0f, 0.0f, 0.5f); glVertex3f((float)mapSize.x/2,   (float)mapSize.y/2, 0); //glVertex3f((float)-mapSize.x/2 + strSize.x + 0.25f,   (float)mapSize.y/2, 0);
	glColor4f(0.0f, 0.0f, 0.0f, 0.5f); glVertex3f((float)-mapSize.x/2, (float)mapSize.y/2, 0); //glVertex3f((float)-mapSize.x/2, (float)mapSize.y/2, 0);
	glEnd();

	glBegin(GL_QUADS);
	glColor4f(0.0f, 0.0f, 0.0f, 0); glVertex3f((float)-mapSize.x/2, (float)-mapSize.y/2 + 0.5f, 0);
	glColor4f(0.0f, 0.0f, 0.0f, 0); glVertex3f((float)mapSize.x/2,   (float)-mapSize.y/2 + 0.5f, 0);
	glColor4f(0.0f, 0.0f, 0.0f, 0.5f); glVertex3f((float)mapSize.x/2,   (float)-mapSize.y/2, 0);
	glColor4f(0.0f, 0.0f, 0.0f, 0.5f); glVertex3f((float)-mapSize.x/2, (float)-mapSize.y/2, 0);
	glEnd();

	glEnable(GL_TEXTURE_2D);
	gfx_texture(&fontTex);
	glColor4f(1, 1, 1, 1);
	gfx_draw_text(&FONT_DEFAULT, vec2((float)-mapSize.x/2 + 0.125f, (float)mapSize.y/2 - 0.25f - 0.125f), str);
	vec2_t moneyStrSize = gfx_layout_text(&FONT_DEFAULT, moneyStr);
	gfx_draw_text(&FONT_DEFAULT, vec2((float)mapSize.x/2 - moneyStrSize.x - 0.125f, (float)mapSize.y/2 - 0.25f - 0.125f), moneyStr);
	gfx_draw_text(&FONT_DEFAULT, vec2((float)-mapSize.x/2 + 0.125f, (float)-mapSize.y/2 + 0.125f), tileStr);

	char* controlsStr = "(ENTER) to buy plants";
	if (player.tile && player.tile->plant.alive) {
		if (player.tile->plant.stage >= 2) {
			controlsStr = "(ENTER) to sell this crop";
		} else {
			if (!(player.tile->plant.def.flags & PLANT_LIKES_WIND)) {
				controlsStr = "(NUM1) to buy wind posts for $50";
			} else {
				controlsStr = "This plant is still growing";
			}
		}
	}
	vec2_t controlsStrSize = gfx_layout_text(&FONT_DEFAULT, controlsStr);
	gfx_draw_text(&FONT_DEFAULT, vec2((float)mapSize.x/2 - controlsStrSize.x - 0.125f, -(float)mapSize.y/2 + 0.125f), controlsStr);

	// MENU
	if (menuShop) {
		gfx_texture(0);
		glLoadIdentity();
		gfx_color(vec4(0.1f, 0.2f, 0.2f, 0.75f));
		gfx_quad(vec2f(0), vec2(mapSize.x, mapSize.y));
		gfx_color(vec4(0.2f, 0.3f, 0.3f, 1));
		gfx_quad(vec2f(0), vec2(12, 11));

		gfx_texture(&fontTex);
		gfx_color(vec4f(1));
		gfx_draw_text(&FONT_DEFAULT, vec2(-6.0f + 0.125f, (float)mapSize.y/2 - 0.25f - 0.125f), "SHOP");
		char* controlStr = "(ENTER) to buy a plant / (ESC) to leave shop";
		vec2_t controlStrSize = gfx_layout_text(&FONT_DEFAULT, controlStr);
		gfx_draw_text(&FONT_DEFAULT, vec2(6.0f - 0.125f - controlStrSize.x, (float)mapSize.y/2 - 0.25f - 0.125f), controlStr);

		vec2_t cursorStart = { -6.0f + 0.125f, 5.25f-0.25f - 0.125f - 1.0f};
		vec2_t cursor = cursorStart;
		// vec2_t textStep = {0.25f+0.125f, -(0.25f+0.125f)};
		vec2_t textStep = {0, -(0.25f+0.125f)};

		gfx_color(vec4f(1));
		for (int i=0; i<array_size(plantDefs); ++i) {
			plant_def_t* plant = plantDefs + i;

			// float col = i > 5 ? 6.0f : 0.0f;
			float col = 0;
			if (i == 5) {
				cursor.x += 6.0f;
				cursor.y = cursorStart.y;
			}

			gfx_texture(&fontTex);
			gfx_draw_text(&FONT_DEFAULT, add2(cursor, vec2(col + 1.75f, 0.4f)), plant->name);
			char priceStr[64];
			sprint(priceStr, 64, "$%u", plant->cost);
			if (plant->cost > money) {
				gfx_color(vec4(1, 0, 0, 1));
			}
			gfx_draw_text(&FONT_DEFAULT, add2(cursor, vec2(col + 1.75f + 3.25f, 0.4f)), priceStr);
			gfx_color(vec4(1, 1, 1, 1));

			uint32_t flags[] = {
				PLANT_LIKES_WATER,
				PLANT_LIKES_DRY,
				PLANT_LIKES_WIND,
				PLANT_LIGHTNING_RESISTANT,
			};
			char* flagNames[] = {
				"Flooding",
				"Dry",
				"Strong Wind",
				"Lightning",
			};
			for (int i=0; i<array_size(flags); ++i) {
				char str[64];
				sprint(str, 64, "%s", flagNames[i]);
				_Bool right = (i%2 == 1);
				vec2_t pos = add2(cursor, vec2(col + 1.75f + (right ? 2.5f : 0), 0.3f + -0.5f - (0.4f*(i/2))));
				gfx_texture(&iconsTex);
				if (plant->flags & flags[i]) {
					gfx_draw_sprite_rect(add2(pos, vec2(-0.25f, 0.125f)), vec2(0, 224), vec2(8, 8));
				} else {
					gfx_draw_sprite_rect(add2(pos, vec2(-0.25f, 0.125f)), vec2(8, 224), vec2(8, 8));
				}
				gfx_texture(&fontTex);
				gfx_draw_text(&FONT_DEFAULT, pos, str);
			}

			gfx_texture(&plantTex);
			gfx_draw_sprite_rect(add2(cursor, add2(div2f(plant->spriteSize, 64), vec2(col + 1.0f - plant->spriteSize.x/64 - 0.25f, plant->tileOffset))), plant->spriteOffset, plant->spriteSize);

			gfx_texture(0);
			if (shopSelected == i) {
				gfx_line_quad(add2(cursor, vec2(col + 3, 0)), vec2(6, 2));
			}

			cursor = add2(cursor, vec2(0, -2));
			// gfx_draw_text(&FONT_DEFAULT, cursor, plant->name);
			// cursor = add2(cursor, textStep);
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