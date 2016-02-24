#include "seascape.h"

#include <malloc.h>
#include <stdio.h>
#include <math.h>

#include "mu.h"
#include "gxutils.h"

sea_t* SEA_create(u32 width, u32 height) {
	sea_t* context = malloc(sizeof(sea_t));
	if (context == NULL) {
		printf("failed to alloc global context");
		return 0;
	}

	context->resolution = (guVec2){ width, height };

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
					guVec2 coord;
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

// math
void fromEuler(guVector ang, Mtx m) {
	guVec2 a1 = (guVec2) { sinf(ang.x), cos(ang.x) };
	guVec2 a2 = (guVec2) { sinf(ang.y), cos(ang.y) };
	guVec2 a3 = (guVec2) { sinf(ang.z), cos(ang.z) };
	m[0][0] = a1.y*a3.y + a1.x*a2.x*a3.x;
	m[0][1] = a1.y*a2.x*a3.x + a3.y*a1.x;
	m[0][2] = -a2.y*a3.x;
	m[1][0] = -a2.y*a1.x;
	m[1][1] = a1.y*a2.y;
	m[1][2] = a2.x;
	m[2][0] = a3.y*a1.x*a2.x + a1.y*a3.x;
	m[2][1] = a1.x*a3.x - a1.y*a3.y*a2.x;
	m[2][2] = a2.y*a3.y;
}

f32 hash(guVec2 p) {
	float h = guVec2Dot(p, (guVec2) { 127.1, 311.7 });
	float i;
	return modff(sinf(h)*43758.5453123f, &i);
}

f32 noise(guVec2 p) {
	guVec2 i, f;
	guVec2Modf(p, &f, &i);

	guVec2 tmp = (guVec2) { 3.0 - 2.0*f.x, 3.0 - 2.0*f.y };
	guVec2 u = guVec2Mul(f, guVec2Mul(f, tmp));

	f32 h0 = hash(i);
	f32 h1 = hash(guVec2Add(i, (guVec2) { 1.0, 0.0 }));
	f32 h2 = hash(guVec2Add(i, (guVec2) { 0.0, 1.0 }));
	f32 h3 = hash(guVec2Add(i, (guVec2) { 1.0, 1.0 }));

	return -1.0 + 2.0 * mix(mix(h0, h1, u.x), mix(h2, h3, u.x), u.y);
}

f32 diffuse(guVector n, guVector l, f32 p) {
	return powf(muVecDotProduct(&n, &l) * 0.4f + 0.6f, p);
}
f32 specular(guVector n, guVector l, guVector e, float s) {
	float nrm = (s + 8.0) / (3.1415 * 8.0);

	guVector reflect = guVecReflect(e, n);
	float dot = muVecDotProduct(&reflect, &l);

	return powf(fmaxf(dot, 0.0f), s) * nrm;
}

f32 sea_octave(guVec2 uv, f32 choppy) {
	const float n = noise(uv);
	uv.x += n;
	uv.y += n;

	guVec2 wv = guVec2Abs(guVec2Sin(uv));
	wv.x = 1.0f - wv.x; wv.y = 1.0f - wv.y;

	guVec2 swv = guVec2Abs(guVec2Cos(uv));
	wv = guVec2Mix(wv, swv, wv);
	return powf(1.0 - powf(wv.x * wv.y, 0.65), choppy);
}

// sky
guVector getSkyColor(guVector e) {
	e.y = fmaxf(e.y, 0);
	guVector ret;
	ret.x = powf(1 - e.y, 2);
	ret.y = 1 - e.y;
	ret.z = 0.6f + (1 - e.y)*0.4f;
	return ret;
}

// sea
float map(sea_t* sea, guVector p, u8 iterations) {
	f32 freq = sea->SEA_FREQ;
	f32 amp_ = sea->SEA_HEIGHT;
	f32 choppy = sea->SEA_CHOPPY;
	guVec2 uv = (guVec2) { p.x * 0.75f, p.z };

	f32 d = 0, h = 0;
	u8 i;
	for (i = 0; i < iterations; i++) {
		guVec2 octUV = (guVec2) {
			(uv.x + sea->SEA_TIME)*freq,
			(uv.y + sea->SEA_TIME)*freq,
		};
		d = sea_octave(octUV, choppy);
		guVec2 octUV2 = (guVec2) {
			(uv.x - sea->SEA_TIME)*freq,
			(uv.y - sea->SEA_TIME)*freq,
		};
		d += sea_octave(octUV2, choppy);
		h += d * amp_;
		uv = guVec2MatMul(sea->octave_m, uv);
		freq *= 1.9f;
		amp_ *= 0.22f;
		choppy = mix(choppy, 1, 0.2f);
	}
	return p.y - h;
}

guVector getSeaColor(sea_t* sea, guVector p, guVector n, guVector l, guVector eye, guVector dist) {
	guVector neye = (guVector) { -eye.x, -eye.y, -eye.z };
	f32 fresnel = 1 - fmaxf(guVecDotProduct(&n, &neye), 0);
	fresnel = powf(fresnel, 3) * 0.65f;

	guVector reflected = getSkyColor(guVecReflect(eye, n));

	f32 refractedDiffuse = diffuse(n, l, 80);	
	guVector refracted = (guVector) {
		sea->SEA_BASE.x + refractedDiffuse * sea->SEA_WATER_COLOR.x * 0.12f,
		sea->SEA_BASE.y + refractedDiffuse * sea->SEA_WATER_COLOR.y * 0.12f,
		sea->SEA_BASE.z + refractedDiffuse * sea->SEA_WATER_COLOR.z * 0.12f,
	};

	guVector color = (guVector) {
		mix(refracted.x, reflected.x, fresnel),
		mix(refracted.y, reflected.y, fresnel),
		mix(refracted.z, reflected.z, fresnel),
	};

	f32 atten = fmaxf(1 - guVecDotProduct(&dist, &dist) * 0.001f, 0);
	f32 spec = specular(n, l, eye, 60);
	color = (guVector) {
		color.x + (sea->SEA_WATER_COLOR.x * (p.y - sea->SEA_HEIGHT) * 0.18f * atten) + spec,
		color.y + (sea->SEA_WATER_COLOR.y * (p.y - sea->SEA_HEIGHT) * 0.18f * atten) + spec,
		color.z + (sea->SEA_WATER_COLOR.z * (p.y - sea->SEA_HEIGHT) * 0.18f * atten) + spec,
	};

	return color;
}

guVector SEA_pixel(sea_t* sea, guVec2 coord) {
	guVec2 uv;
	//TODO Optimize with PS ops?
	uv.x = ((coord.x / sea->resolution.x) * 2 - 1) * (sea->resolution.x / sea->resolution.y);
	uv.y = (coord.y / sea->resolution.y) * 2 - 1;

	f32 time = (f32)sea->time * 0.3;

	// ray
	//TODO Done once, do outside pixel loop
	guVector ang = (guVector) { sinf(time*3), sinf(time)*0.2f+0.3f, time };
	guVector ori = (guVector) { 0, 3.5f, time * 5 };

	guVector dir = (guVector) { uv.x, uv.y, -2 };
	guVecNormalize(&dir);
	dir.z += guVecMag(&dir) * 0.15f;
	guVecNormalize(&dir);

	Mtx dirMtx;
	fromEuler(ang, dirMtx);
	guVecMultiply(dirMtx, &dir, &dir);

	// tracing
	guVector p;
	heightMapTracing(ori, dir, &p);
	guVector dist;
	guVecSub(&p, &ori, &dist);
	guVector n = getNormal(p, guVecDotProduct(&dist, &dist) * sea->EPSILON_NRM);
	guVector light = (guVector){ 0, 1, 0.8f };
	guVecNormalize(&light);

	guVector color_sky = getSkyColor(dir);
	guVector color_sea = getSeaColor(sea, p, n, light, dir, dist);
	f32 mixfactor = pow(smoothstep(0, -0.05f, dir.y), 0.3f);

	// color
	guVector color = (guVector) {
		powf(mix(color_sky.x, color_sea.y, mixfactor), 0.75f),
		powf(mix(color_sky.y, color_sea.y, mixfactor), 0.75f),
		powf(mix(color_sky.z, color_sea.z, mixfactor), 0.75f)
	};

	return color;
}