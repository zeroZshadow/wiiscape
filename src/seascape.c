#include "seascape.h"

#include <malloc.h>
#include <stdio.h>

#include "mu.h"
#include "gxutils.h"

sea_t* SEA_create(u32 width, u32 height) {
	sea_t* context = malloc(sizeof(sea_t));
	if (context == NULL) {
		printf("failed to alloc global context");
		return 0;
	}

	context->resolution.x = width;
	context->resolution.y = height;

	context->NUM_STEPS = 8;
	context->PI = 3.1415;
	context->EPSILON = 1e-3;
	context->EPSILON_NRM = 0.1 / (float)height;

	context->ITER_FRAGMENT = 3;
	context->ITER_GEOMETRY = 5;
	context->SEA_HEIGHT = 0.6;
	context->SEA_CHOPPY = 4;
	context->SEA_SPEED = 0.8;
	context->SEA_FREQ = 0.16;
	context->SEA_BASE = (guVector){ 0.1, 0.19, 0.22 };
	context->SEA_WATER_COLOR = (guVector){ 0.8, 0.9, 0.6 };
	context->octave_m = (Mtx22) { 1.6, 1.6, -1.2, 1.6 };

	return context;
}

void SEA_draw(sea_t* sea) {
	GXColor datatile[TILESIZE*TILESIZE];

	// Since we do not have a massive amount of memory, we have to pathtrace is the tiling order so we only have to store 4*4 pixels of data
	const u32 width = sea->resolution.x;
	const u32 height = sea->resolution.y;
	const u32 width_blocks = width / TILESIZE;
	const u32 height_blocks = height / TILESIZE;

	u32 x, y, xb, yb;

	sea->time++;
	sea->SEA_TIME = (float)sea->time * sea->SEA_SPEED;

	for (yb = 0; yb < height_blocks; ++yb) {
		for (xb = 0; xb < width_blocks; ++xb) {
			//This is a single tile
			for (y = 0; y < TILESIZE; ++y) {
				for (x = 0; x < TILESIZE; ++x) {
					// Calculate UV coordinate
					// TODO Correct for aspect ratio
					guVector coord;
					coord.x = (xb * TILESIZE) + x;
					coord.y = (yb * TILESIZE) + y;

					//Get pixel color for tile
					guVector color = SEA_pixel(sea, coord);

					//Convert to u32
					//TODO: Clamp color?
					datatile[x + (y * TILESIZE)] = GXU_vectorToColorData(&color);
				}
			}

			//Entire tile rendered, write to texture
			GXU_copyTilePixelBuffer(datatile, xb, yb);
		}
	}
}

guVector SEA_pixel(sea_t* sea, guVector coord) {
	guVector uv;
	//TODO Optimize with PS ops?
	uv.x = coord.x / sea->resolution.x;
	uv.y = coord.y / sea->resolution.y;

	return uv;
}