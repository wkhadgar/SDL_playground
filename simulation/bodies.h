//
// Created by paulo on 25/06/2023.
//

#ifndef SDL_PLAYGROUND_BODIES_H
#define SDL_PLAYGROUND_BODIES_H

#include <stdint.h>
#include <SDL.h>
#include "vectors.h"

typedef struct Body {
	uint8_t health;
	uint16_t mass;
	
	uint16_t max_force;
	uint16_t max_speed;
	
	SDL_Rect rect;
	SDL_Texture* texture;
	
	Vector2_t position;
	Vector2_t velocity;
} Body;

Body* body_new(int x, int y, uint16_t mass, SDL_Texture* texture, uint16_t max_force, uint16_t max_speed);

inline void body_steer_to(Body* body, Vector2_t target) {
	if (vector_length(vector_sub(target, body->position)) < 2) {
		return;
	}
	
	Vector2_t steer_force = vector_clamp(target, body->max_force);
	body->velocity = vector_clamp(vector_add(body->velocity, vector_div(steer_force, body->mass)), body->max_speed);
	body->position = vector_add(body->position, body->velocity);
	
	body->rect.x = (int) body->position.x - (body->rect.w / 2);
	body->rect.y = (int) body->position.y - (body->rect.h / 2);
}

void body_seek(Body* body, Vector2_t target);

void body_evade(Body* body, Vector2_t target);

void body_pursuit(Body* body, Body* target_body);

void body_draw(Body* body, SDL_Renderer* rend);

#endif //SDL_PLAYGROUND_BODIES_H