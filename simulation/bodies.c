//
// Created by paulo on 25/06/2023.
//

#include "bodies.h"

#define ROTATE_TEXTURE 1

Body* body_new(int x, int y, uint16_t mass, SDL_Texture* texture, uint16_t max_force, uint16_t max_speed) {
	Body* new = (Body*) malloc(sizeof(Body));
	
	new->health = 100;
	new->mass = mass;
	
	new->max_force = max_force;
	new->max_speed = max_speed;
	
	new->velocity.x = 0;
	new->velocity.y = 0;
	new->position.x = x;
	new->position.y = y;
	
	new->rect.x = x - (mass / 2);
	new->rect.y = y - (mass / 2);
	new->rect.w = mass * 6;
	new->rect.h = mass * 6;
	new->texture = texture;
	
	return new;
}


void body_seek(Body* body, Vector2_t target) {
	Vector2_t tgt = {target.x, target.y};
	Vector2_t desired_vel = vector_mult(vector_normalize(vector_sub(tgt, body->position)), body->max_force);
	body_steer_to(body, vector_sub(desired_vel, body->velocity));
}

void body_evade(Body* body, Vector2_t target) {
	Vector2_t tgt = {target.x, target.y};
	Vector2_t desired_vel = vector_mult(vector_normalize(vector_sub(body->position, tgt)), body->max_speed);
	body_steer_to(body, vector_sub(desired_vel, body->velocity));
}

void body_draw(Body* body, SDL_Renderer* rend) {
#if ROTATE_TEXTURE == 1
	double angle = RAD_TO_DEG(atan2(body->velocity.y, body->velocity.x));
	SDL_RenderCopyEx(rend, body->texture, NULL, &body->rect, angle, NULL,
					 ((angle > 90) && (angle < 270)) ? SDL_FLIP_VERTICAL : SDL_FLIP_NONE);
#else
	SDL_RenderCopy(rend, body->texture, NULL, &body->rect);
#endif
}