//
// Created by paulo on 25/06/2023.
//

#include "bodies.h"

void body_steer_to(Body* body, Vector2 target) {
	if (((target.x - (int) body->pos.x) == 0) && ((target.y - (int) body->pos.y) == 0)) {
		return;
	}
	
	Vector2 steer_force = vector_clamp(target, MAX_FORCE);
	body->velocity = vector_clamp(vector_add(body->velocity, vector_div(steer_force, body->mass)), MAX_SPEED);
	body->pos = vector_add(body->pos, body->velocity);
}

void body_seek(Body* body, Vector2 target) {
	Vector2 desired_vel = vector_mult(vector_normalize(vector_sub(target, body->pos)), MAX_SPEED);
	body_steer_to(body, vector_sub(desired_vel, body->velocity));
}