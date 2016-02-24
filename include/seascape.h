#ifndef _SEASCAPE_H
#define _SEASCAPE_H

#include <gccore.h>

typedef struct {
	guVector resolution;
	u8 NUM_STEPS;
	float PI;
	float EPSILON;
	float EPISON_NRM;
	u8 ITER_GEOMETRY;
	u8 ITER_FRAGMENT;
	float SEA_HEIGHT;
	float SEA_CHOPPY;
	float SEA_SPEED;
	float SEA_FREQ;
	guVector SEA_BASE;
	guVector SEA_WATER_COLOR;
	float SEA_TIME;
	Mtx33 octave_m;
} sea_t;

void SEA_draw();

guVector SEA_pixel(sea_t* ctx, guVector coord);

#endif