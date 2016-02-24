#include "seascape.h"
#include <malloc.h>
#include <stdio.h>

#include "mu.h"
#include "gxutils.h"

sea_t* SEA_create() {
	sea_t* context = malloc(sizeof(sea_t));
	if (context == NULL) {
		printf("failed to alloc global context");
		return 0;
	}

	return context;
}

void SEA_draw(sea_t* sea) {
	GXColor datatile[TILESIZE*TILESIZE];

	// Since we do not have a massive amount of memory, we have to pathtrace is the tiling order so we only have to store 4*4 pixels of data
	const u32 width = sea->resolution.x;
	const u32 height = sea->resolution.y;

	u16 x, y, ix, iy;

	for (y = 0; y < height; y += TILESIZE) {
		for (x = 0; x < width; x += TILESIZE) {
			//This is a single tile
			for (iy = 0; iy < TILESIZE; ++iy) {
				for (ix = 0; ix < TILESIZE; ++ix) {
					// Calculate UV coordinate
					// TODO Correct for aspect ratio
					guVector uv;
					uv.x = (x + ix) / (float) width;
					uv.y = (y + iy) / (float) height;

					//Get pixel color for tile
					guVector color = SEA_pixel(sea, uv);

					//Convert to u32
					//TODO: WRITE CLAMP IN PSQ
					guVecMax(&color, 1.0f);
					guVecMin(&color, 0.0f);
					datatile[ix + (iy * TILESIZE)] = GXU_vectorToColorData(&color);
				}
			}

			//Entire tile rendered, write to texture
			GXU_copyTilePixelBuffer(datatile, x / TILESIZE, y / TILESIZE);
		}
	}
}

guVector SEA_pixel(sea_t* ctx, guVector coord) {
	guVector uv;
	//TODO Optimize with PS ops?
	uv.x = coord.x / ctx->resolution.x;
	uv.x = coord.y / ctx->resolution.y;
}