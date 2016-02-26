#include "mathutils.h"
#include "mu.h"
#include <ogc/gu.h>

/* This should be in gu.h or something */

f32 guVecMag(guVector* vec) {
	return sqrt(muVecDotProduct(vec, vec));
}

inline f32 vecDistance(guVector* point1, guVector* point2) {
	guVector sub;
	muVecSub(point2, point1, &sub);
	return guVecMag(&sub);
}

inline u8 vecMinAxis(guVector* vec) {
	guVector abs;
	muVecAbs(vec, &abs);
	return (abs.x <= abs.y ? (abs.x <= abs.z ? 0 : 2) : (abs.y <= abs.z) ? 1 : 2);
}

void vecPerpendicular(guVector* vec, guVector* out) {
	const u8 ma = vecMinAxis(vec);
	if (ma == 0) {
		const guVector val = { 0, vec->z, -vec->y };
		*out = val;
	} else if (ma == 1) {
		const guVector val = { vec->z, 0, -vec->x };
		*out = val;
	} else {
		const guVector val = { vec->y, -vec->x, 0 };
		*out = val;
	}
}

guVector Vector(f32 x, f32 y, f32 z) {
	guVector res = { x, y, z };
	return res;
}

void guVecMax(guVector* vector, f32 max) {
	vector->x = vector->x > max ? max : vector->x;
	vector->y = vector->y > max ? max : vector->y;
	vector->z = vector->z > max ? max : vector->z;
}

void guVecMin(guVector* vector, f32 min) {
	vector->x = vector->x < min ? min : vector->x;
	vector->y = vector->y < min ? min : vector->y;
	vector->z = vector->z < min ? min : vector->z;
}

void guVec2Modf(guVec2 v, guVec2* f, guVec2* i) {
	i->x = floorf(v.x);
	i->y = floorf(v.y);
	f->x = v.x - i->x;
	f->y = v.y - i->y;
}

f32 guVec2Dot(guVec2 a, guVec2 b) {
	return  (a.x * b.x) + (a.y * b.y);
}

guVec2 guVec2Mul(guVec2 a, guVec2 b) {
	return (guVec2) { 
		a.x * b.x,
		a.y * b.y
	};
}

guVec2 guVec2Add(guVec2 a, guVec2 b) {
	return (guVec2) {
		a.x + b.x,
		a.y + b.y
	};
}

guVec2 guVec2MatMul(Mtx22 m, guVec2 v) {
	return (guVec2) {
		m.a1 * v.x + m.a2 * v.y,
		m.b1 * v.x + m.b2 * v.y
	};
}

guVec2 guVec2Abs(guVec2 a) {
	return (guVec2) {
		fabs(a.x),
		fabs(a.y)
	};
}

guVec2 guVec2Mix(guVec2 a, guVec2 b, guVec2 f) {
	return (guVec2) {
		mix(a.x, b.x, f.x),
		mix(a.y, b.y, f.y)
	};
}

guVector guVecReflect(guVector p, guVector n) {
	guVector r;
	float dot = muVecDotProduct(&p, &n) * 2;
	muVecScale(&n, &r, dot);
	guVecSub(&r, &p, &r);
	return r;
}

f32 smoothstep(f32 a, f32 b, f32 t) {
	t = fminf(fmaxf(muFastDiv(t-a, b-a), 0), 1);
	return t*t*(3-2*t);
}

f32 guVec2Mag(guVec2* uv) {
	return sqrtf(uv->x*uv->x + uv->y * uv->y);
}
