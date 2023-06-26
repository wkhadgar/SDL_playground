//
// Created by paulo on 25/06/2023.
//

#ifndef SDL_PLAYGROUND_VECTORS_H
#define SDL_PLAYGROUND_VECTORS_H

#include <stdint.h>
#include <math.h>

#define RAD_TO_DEG(x) ((x) > 0 ? (x) : (2*M_PI + (x))) * 360 / (2*M_PI)
#define MIN(a, b) ((a) > (b)) ? (b) : (a)

typedef struct Vector2 {
	double x;
	double y;
} Vector2_t;

double vector_length(Vector2_t vec);

Vector2_t vector_add(Vector2_t vec_a, Vector2_t vec_b);

Vector2_t vector_sub(Vector2_t vec_a, Vector2_t vec_b);

Vector2_t vector_scale(Vector2_t vec, double factor);

Vector2_t vector_clamp(Vector2_t vec, double max_length);

Vector2_t vector_normalize(Vector2_t vec);

#endif //SDL_PLAYGROUND_VECTORS_H