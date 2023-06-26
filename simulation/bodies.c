//
// Created by paulo on 25/06/2023.
//

#include "bodies.h"

#define ROTATE_TEXTURE 1

Body* body_new(int x, int y, double mass, SDL_Texture* texture, double max_force, double max_speed) {
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
    new->rect.w = mass;
    new->rect.h = mass;
    new->texture = texture;

    return new;
}

void body_steer_to(Body* body, Vector2_t steer_direction) {
    if (vector_length(vector_sub(steer_direction, body->position)) == 0) {
        return;
    }

    Vector2_t steer_force = vector_clamp(steer_direction, body->max_force);
    Vector2_t accel = vector_scale(steer_force, 1 / body->mass);
    Vector2_t result_vec = vector_add(body->velocity, accel);

    body->velocity = vector_clamp(result_vec, body->max_speed);
    body->position = vector_add(body->position, body->velocity);

    body->rect.x = (int) body->position.x - (body->rect.w / 2);
    body->rect.y = (int) body->position.y - (body->rect.h / 2);
}

void body_seek(Body* body, Vector2_t target) {
    Vector2_t to_tgt_vec = vector_sub(target, body->position);
    Vector2_t desired_vel = vector_clamp(to_tgt_vec, body->max_force);
    body_steer_to(body, vector_sub(desired_vel, body->velocity));
}

void body_arrive(Body* body, Vector2_t target) {
    Vector2_t to_tgt_vec = vector_sub(target, body->position);
    double dist = vector_length(to_tgt_vec);
    double ramped_speed = body->max_speed * (dist / 100);
    double clipped_speed = MIN(ramped_speed, body->max_speed);
    Vector2_t desired_vel = vector_scale(to_tgt_vec, clipped_speed / dist);
    body_steer_to(body, vector_sub(desired_vel, body->velocity));
}

void body_evade(Body* body, Vector2_t target) {
    Vector2_t tgt = {target.x, target.y};
    Vector2_t desired_vel = vector_scale(vector_normalize(vector_sub(body->position, tgt)), body->max_speed);
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