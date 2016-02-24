/* SDK Libraries */
#include <gccore.h>
#include <stdio.h>

#include "gxutils.h"
#include "seascape.h"
#include "mathutils.h"

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

	sea_t* sea = SEA_create(renderWidth, renderHeight);

	isRunning = TRUE;
	while (isRunning) {
		SEA_draw(sea);

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