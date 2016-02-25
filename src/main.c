/* SDK Libraries */
#include <gccore.h>
#include <debug.h>
#include <stdio.h>
#include <string.h>

#include "gxutils.h"
#include "seascape.h"
#include "mathutils.h"
#include "timer.h"

BOOL isRunning;
void OnResetCalled();

int main() { //int argc, char **argv) {
	SYS_SetResetCallback(OnResetCalled);

	// Initialize graphics
	GXU_init();

	// Frame buffer
	u16 renderWidth = vmode->viWidth >> 3;
	u16 renderHeight = vmode->viHeight >> 3;

	GXU_createPixelBuffer(renderWidth, renderHeight);
	GXU_clearPixelBuffer(0xFF000000);

	// Render first frame, so there is no corruption
	GXU_renderPixelBuffer();
	GXU_done();

	CON_EnableGecko(EXI_CHANNEL_1, FALSE);

	sea_t* sea = SEA_create(renderWidth, renderHeight);

	isRunning = TRUE;
	while (isRunning) {
		u64 start = timer_gettime();
		SEA_draw(sea);
		u64 time = timer_gettime() - start;

		printf("Frame time: %llu\n", time);

		// Render buffer to screen
		//TODO: Throw this on a thread
		GXU_renderPixelBuffer();
		GXU_done();
	}

	return 0;
}

void OnResetCalled() {
	isRunning = FALSE;
}