//
// Created by paulo on 25/06/2023.
//

#ifndef SDL_PLAYGROUND_BODIES_H
#define SDL_PLAYGROUND_BODIES_H

#include <stdbool.h>
#include <SDL.h>
#include "vectors.h"

typedef struct Body {
    int32_t health;

    double mass;
    double max_force;

    double max_speed;
    double wander_speed;
    double current_speed;

    SDL_Rect collision_rect;

    SDL_Rect rect;
    SDL_Texture* texture;

    Vector2_t position;
    Vector2_t velocity;

    Vector2_t obstacle_check_lines[3];

} Body;

typedef struct body_list {
    Body* this;
    struct body_list* next;
    struct body_list* prev;
} body_list_t;

Body* body_new(int x, int y, double mass, SDL_Texture* texture, double max_speed);

void body_seek(Body* body, Vector2_t target);

void body_arrive(Body* body, Vector2_t target);

void body_flee(Body* body, Vector2_t target);

void body_wander(Body* body);

void body_pursuit(Body* body, Body* target_body);

void body_obstacle_avoid(Body* body) {
    body->obstacle_check_lines[]
}

void body_grow(Body* body, double mass_growth);

bool body_keep_inside(Body* body, SDL_Rect boundary);

Body* body_get_closest_body_from(const Body* body, body_list_t* list_head, double detection_radius);

void body_list_delete_node(body_list_t** list_head, body_list_t* node_to_delete);

void body_draw(Body* body, SDL_Renderer* rend);

#endif //SDL_PLAYGROUND_BODIES_H