/* SDK Libraries */
#include <gccore.h>
#include <stdio.h>

#include "mtrand.h"
#include "gxutils.h"
#include "seascape.h"
#include "mathutils.h"

BOOL isRunning;
void OnResetCalled();

int main() { //int argc, char **argv) {
	SYS_SetResetCallback(OnResetCalled);

	FncMtSrand(time(NULL));

	// Initialize graphics
	GXU_init();

	// Frame buffer
	u16 renderWidth = vmode->viWidth >> 2;
	u16 renderHeight = vmode->viHeight >> 2;

	GXU_createPixelBuffer(renderWidth, renderHeight);
	GXU_clearPixelBuffer(0xFF000000);

	// Render first frame, so there is no corruption
	GXU_renderPixelBuffer();
	GXU_done();

	isRunning = TRUE;
	while (isRunning) {
		SEA_draw();

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