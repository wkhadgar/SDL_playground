//
// Created by paulo on 25/06/2023.
//

#ifndef SDL_PLAYGROUND_BODIES_H
#define SDL_PLAYGROUND_BODIES_H

#include <stdint.h>
#include "vectors.h"


/**
 * @brief Força máxima para cálculos de mudança de direção.
 */
#define MAX_FORCE 5

/**
 * @brief Velocidade máxima de movimentação.
 */
#define MAX_SPEED 5

typedef struct Body {
	uint8_t health;
	uint16_t mass;
	
	Vector2 pos;
	Vector2 velocity;
} Body;

void body_steer_to(Body* body, Vector2 target);

void body_seek(Body* body, Vector2 target);

#endif //SDL_PLAYGROUND_BODIES_H