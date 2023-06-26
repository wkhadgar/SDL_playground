//
// Created by paulo on 25/06/2023.
//

#include "vectors.h"

double vector_length(Vector2_t vec) {
    return sqrt((vec.x * vec.x) + (vec.y * vec.y));
}

Vector2_t vector_clamp(Vector2_t vec, double max_length) {

    double length = vector_length(vec);

    if (length <= max_length) {
        return vec;
    }

    return vector_scale(vec, max_length / length);
}

Vector2_t vector_scale(Vector2_t vec, double factor) {
    Vector2_t vec_scaled = {vec.x * factor, vec.y * factor};
    return vec_scaled;
}

Vector2_t vector_sub(Vector2_t vec_a, Vector2_t vec_b) {
    Vector2_t vec_sub = {vec_a.x - vec_b.x, vec_a.y - vec_b.y};
    return vec_sub;
}

Vector2_t vector_add(Vector2_t vec_a, Vector2_t vec_b) {
    Vector2_t vec_add = {vec_a.x + vec_b.x, vec_a.y + vec_b.y};
    return vec_add;
}

Vector2_t vector_normalize(Vector2_t vec) {
    return vector_scale(vec, 1 / vector_length(vec));
}