//
// Created by paulo on 25/06/2023.
//

#include "vectors.h"

#define MIN(a, b) ((a) > (b)) ? (b) : (a)

Vector2_t vector_clamp(Vector2_t vec, uint32_t mag) {
	
	double length = vector_length(vec);
	double factor = MIN(mag, length) / length;
	
	return vector_mult(vec, factor);
}