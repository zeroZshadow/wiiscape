#ifndef _SEASCAPE_H
#define _SEASCAPE_H

#include <gccore.h>

#include "mathutils.h"

typedef struct {
	guVec2 resolution;
	f32 time;

	u8 NUM_STEPS;
	float PI;
	float EPSILON;
	float EPSILON_NRM;
	u8 ITER_GEOMETRY;
	u8 ITER_FRAGMENT;
	float SEA_HEIGHT;
	float SEA_CHOPPY;
	float SEA_SPEED;
	float SEA_FREQ;
	guVector SEA_BASE;
	guVector SEA_WATER_COLOR;
	float SEA_TIME;
	Mtx22 octave_m;
} sea_t;

sea_t* SEA_create();

void SEA_draw(sea_t* sea);

guVector SEA_pixel(sea_t* sea, guVec2 coord);

#endif