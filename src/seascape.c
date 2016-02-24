#include "seascape.h"

#include <malloc.h>
#include <stdio.h>

#include "mu.h"
#include "gxutils.h"
#include "mathutils.h"

sea_t* SEA_create(u32 width, u32 height) {
	sea_t* context = malloc(sizeof(sea_t));
	if (context == NULL) {
		printf("failed to alloc global context");
		return 0;
	}

	context->resolution.x = width;
	context->resolution.y = height;

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