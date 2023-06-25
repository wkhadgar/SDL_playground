//
// Created by paulo on 25/06/2023.
//

#include "vectors.h"

#define MIN(a, b) ((a) > (b)) ? (b) : (a)

Vector2 vector_clamp(Vector2 vec, uint32_t mag) {
	
	double length = vector_length(vec);
	double factor = MIN(mag, length) / length;
	
	return vector_mult(vec, factor);
}

Vector2 vector_normalize(Vector2 vec) {
	return vector_mult(vec, vector_length(vec));
}