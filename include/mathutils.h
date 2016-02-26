/*! \file mathutil.h
 *  \brief Math utility functions
 */

#ifndef _MATHUTIL_H
#define _MATHUTIL_H

#include <ogc/gu.h>
#include <math.h>

typedef struct {
	f32 a1, b1, a2, b2;
} Mtx22;

typedef struct {
	f32 x, y;
} guVec2;

/*! \brief Vector magnitude 
 *  \param vec Vector to calculate magnitude of
 *  \return Magnitude as a f32
 */
f32 guVecMag(guVector* vec);

/*! \brief Squared vector magnitude
 *  \param vec Vector to calculate squared magnitude of
 *  \return Squared magnitude as a f32
 */
f32 guVecSquareMag(guVector* vec);

/*! \brief Get distance between two 3d points
 *  \param point1 First 3d point
 *  \param point2 Second 3d point
 *  \return Distance between point1 and point2
 */
f32 vecDistance(guVector* point1, guVector* point2);

u8 vecMinAxis(guVector* vec);

void vecPerpendicular(guVector* vec, guVector* out);

f32 smoothstep(f32 a, f32 b, f32 t);

f32 fioraRand();

guVector Vector(f32 x, f32 y, f32 z);

guVector guVecReflect(guVector p, guVector n);

void guVecMax(guVector* vector, f32 max);

void guVecMin(guVector* vector, f32 min);

void guVec2Modf(guVec2 v, guVec2* f, guVec2* i);

f32 guVec2Dot(guVec2 a, guVec2 b);

guVec2 guVec2Mul(guVec2 a, guVec2 b);

guVec2 guVec2Add(guVec2 a, guVec2 b);

guVec2 guVec2MatMul(Mtx22 m, guVec2 v);

static inline guVec2 guVec2Sin(guVec2 a) {
	return (guVec2) {
		sinf(a.x),
		sinf(a.y)
	};
}

static inline guVec2 guVec2Cos(guVec2 a) {
	return (guVec2) {
		cosf(a.x),
		cosf(a.y)
	};
}

guVec2 guVec2Abs(guVec2 a);

guVec2 guVec2Mix(guVec2 a, guVec2 b, guVec2 f);

f32 guVec2Mag(guVec2* uv);

static inline f32 mix(f32 a, f32 b, f32 f)
{
	return f * (b - a) + a;
}

#endif


