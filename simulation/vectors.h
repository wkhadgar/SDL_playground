//
// Created by paulo on 25/06/2023.
//

#ifndef SDL_PLAYGROUND_VECTORS_H
#define SDL_PLAYGROUND_VECTORS_H

#include <stdint.h>
#include <math.h>

#define RAD_TO_DEG(x) ((x) > 0 ? (x) : (2*M_PI + (x))) * 360 / (2*M_PI)

typedef struct Vector2 {
	double x;
	double y;
} Vector2_t;

inline double vector_length(Vector2_t vec) {
	return sqrt((vec.x * vec.x) + (vec.y * vec.y));
}

inline Vector2_t vector_add(Vector2_t vec_a, Vector2_t vec_b) {
	Vector2_t vec_add = {vec_a.x + vec_b.x, vec_a.y + vec_b.y};
	return vec_add;
}

inline Vector2_t vector_sub(Vector2_t vec_a, Vector2_t vec_b) {
	Vector2_t vec_sub = {vec_a.x - vec_b.x, vec_a.y - vec_b.y};
	return vec_sub;
}

inline Vector2_t vector_div(Vector2_t vec, double factor) {
	Vector2_t vec_div = {vec.x / factor, vec.y / factor};
	return vec_div;
}

inline Vector2_t vector_mult(Vector2_t vec, double factor) {
	Vector2_t vec_div = {vec.x * factor, vec.y * factor};
	return vec_div;
}

Vector2_t vector_clamp(Vector2_t vec, uint32_t mag);

inline Vector2_t vector_normalize(Vector2_t vec) {
	return vector_div(vec, vector_length(vec));
}

#endif //SDL_PLAYGROUND_VECTORS_H