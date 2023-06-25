//
// Created by paulo on 25/06/2023.
//

#ifndef SDL_PLAYGROUND_VECTORS_H
#define SDL_PLAYGROUND_VECTORS_H

#include <stdint.h>
#include <math.h>

typedef struct Vector2 {
	double x;
	double y;
} Vector2;

inline double vector_length(Vector2 vec) {
	return sqrt((vec.x * vec.x) + (vec.y * vec.y));
}

inline Vector2 vector_add(Vector2 vec_a, Vector2 vec_b) {
	Vector2 vec_add = {vec_a.x + vec_b.x, vec_a.y + vec_b.y};
	return vec_add;
}

inline Vector2 vector_sub(Vector2 vec_a, Vector2 vec_b) {
	Vector2 vec_sub = {vec_a.x - vec_b.x, vec_a.y - vec_b.y};
	return vec_sub;
}

inline Vector2 vector_div(Vector2 vec, double factor) {
	Vector2 vec_div = {vec.x / factor, vec.y / factor};
	return vec_div;
}

inline Vector2 vector_mult(Vector2 vec, double factor) {
	Vector2 vec_div = {vec.x * factor, vec.y * factor};
	return vec_div;
}

Vector2 vector_clamp(Vector2 vec, uint32_t mag);

Vector2 vector_normalize(Vector2 vec);

#endif //SDL_PLAYGROUND_VECTORS_H