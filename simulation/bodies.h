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
    double mass;

    double max_force;
    double max_speed;
    double wander_speed;

    double current_speed;

    SDL_Rect rect;
    SDL_Texture* texture;

    Vector2_t position;
    Vector2_t velocity;
} Body;

Body* body_new(int x, int y, double mass, SDL_Texture* texture, double max_speed);

void body_seek(Body* body, Vector2_t target);

void body_arrive(Body* body, Vector2_t target);

void body_evade(Body* body, Vector2_t target);

void body_wander(Body* body);

void body_pursuit(Body* body, Body* target_body);

void body_keep_inside(Body* body, SDL_Rect boundary);

void body_draw(Body* body, SDL_Renderer* rend);

#endif //SDL_PLAYGROUND_BODIES_H