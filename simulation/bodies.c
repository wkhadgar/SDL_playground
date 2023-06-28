//
// Created by paulo on 25/06/2023.
//

#include "bodies.h"

#define ROTATE_TEXTURE 1
#define DEBUG_COLLISIONS 0

#define WANDER_RATE 10

#define WALL_CLOSENESS_FEAR 75

Body* body_new(int x, int y, double mass, SDL_Texture* texture, double max_speed) {
    Body* new = (Body*) malloc(sizeof(Body));

    new->health = 100;
    new->mass = mass;

    new->max_force = 100 / mass;
    new->max_speed = max_speed;
    new->wander_speed = max_speed / 5;
    new->current_speed = 0;

    new->velocity.x = 0;
    new->velocity.y = 0;
    new->position.x = x;
    new->position.y = y;

    new->rect.x = (int) (x - (mass / 2));
    new->rect.y = (int) (y - (mass / 2));
    new->rect.w = (int) mass;
    new->rect.h = (int) mass;

    new->collision_rect.x = new->position.x + ((new->rect.w / 4) * (new->velocity.x > 0 ? 1 : -1));
    new->collision_rect.y = new->position.y + ((new->rect.h / 4) * (new->velocity.y > 0 ? 1 : -1));
    new->collision_rect.w = new->rect.w / 2;
    new->collision_rect.h = new->rect.h / 2;


    new->texture = texture;

    return new;
}

static void body_steer_to(Body* body, Vector2_t steer_direction, double clamp_speed) {
    if (vector_length(vector_sub(steer_direction, body->position)) == 0) {
        return;
    }

    Vector2_t steer_force = vector_clamp(steer_direction, body->max_force);
    Vector2_t accel = vector_scale(steer_force, 1 / body->mass);
    Vector2_t result_vec = vector_add(body->velocity, accel);

    if (body->current_speed < clamp_speed) {
        body->current_speed += 0.06;
        clamp_speed = body->current_speed;
    } else if (body->current_speed > clamp_speed) {
        body->current_speed -= 0.06;
        clamp_speed = body->current_speed;
    }

    body->velocity = vector_clamp(result_vec, clamp_speed);
    body->position = vector_add(body->position, body->velocity);

    body->rect.x = (int) body->position.x - (body->rect.w / 2);
    body->rect.y = (int) body->position.y - (body->rect.h / 2);

    Vector2_t norm_vel = vector_normalize(body->velocity);
    body->collision_rect.x = body->position.x + ((body->rect.w / 4) * norm_vel.x) - (body->collision_rect.w / 2);
    body->collision_rect.y = body->position.y + ((body->rect.h / 4) * norm_vel.y) - (body->collision_rect.w / 2);

}

void body_seek(Body* body, Vector2_t target) {
    Vector2_t to_tgt_vec = vector_sub(target, body->position);
    Vector2_t desired_vel = vector_clamp(to_tgt_vec, body->max_force);
    body_steer_to(body, vector_sub(desired_vel, body->velocity), body->max_speed);
}

void body_arrive(Body* body, Vector2_t target) {
    Vector2_t to_tgt_vec = vector_sub(target, body->position);
    double dist = vector_length(to_tgt_vec);
    double ramped_speed = body->max_speed * (dist / 100);
    double clipped_speed = MIN(ramped_speed, body->max_speed);
    Vector2_t desired_vel = vector_scale(to_tgt_vec, clipped_speed / dist);
    body_steer_to(body, vector_sub(desired_vel, body->velocity), body->max_speed);
}

void body_wander(Body* body) {
    Vector2_t wander_change = {body->velocity.x + ((((rand() % (WANDER_RATE + 1)) - (WANDER_RATE / 2.0)) / 10)),
                               body->velocity.y + (((rand() % (WANDER_RATE + 1)) - (WANDER_RATE / 2.0)) / 10)};
    body_steer_to(body, vector_sub(wander_change, body->velocity), body->wander_speed);
}

void body_flee(Body* body, Vector2_t target) {
    Vector2_t tgt = {target.x, target.y};
    Vector2_t desired_vel = vector_scale(vector_normalize(vector_sub(body->position, tgt)), body->max_speed);
    body_steer_to(body, vector_sub(desired_vel, body->velocity), body->max_speed);
}

void body_grow(Body* body, double mass_growth) {
    body->mass += mass_growth;
    body->rect.h = 5 * sqrt(body->mass);
    body->rect.w = 5 * sqrt(body->mass);

    body->collision_rect.w = body->rect.w / 2;
    body->collision_rect.h = body->rect.h / 2;
}

bool body_keep_inside(Body* body, SDL_Rect boundary) {
    Vector2_t future_pos = vector_add(body->position, vector_scale(body->velocity, WALL_CLOSENESS_FEAR));

    if ((future_pos.x > boundary.x && future_pos.x < boundary.w) &&
        (future_pos.y > boundary.y && future_pos.y < boundary.h)) {
        return false;
    }

    Vector2_t desired_pos = {boundary.w / 2.0, boundary.h / 2.0};
    body_seek(body, desired_pos);
    return true;
}

Body* body_get_closest_body_from(const Body* body, body_list_t* list_head, double detection_radius) {
    uint32_t dist;
    uint32_t min_dist = SDL_MAX_SINT32;
    body_list_t* current_body = list_head;
    Body* closest_body = NULL;

    while (current_body != NULL) {
        dist = (uint32_t) vector_length(vector_sub(current_body->this->position, body->position));
        if ((dist < detection_radius) && dist < min_dist) {
            min_dist = dist;
            closest_body = current_body->this;
        }

        current_body = current_body->next;
    }

    return closest_body;
}

void body_list_delete_node(body_list_t** list_head, body_list_t* node_to_delete) {
    if (node_to_delete->next) {
        node_to_delete->next->prev = node_to_delete->prev;
    }

    if (node_to_delete->prev) {
        node_to_delete->prev->next = node_to_delete->next;
    } else {
        *list_head = node_to_delete->next;
    }

    free(node_to_delete);
}

void body_draw(Body* body, SDL_Renderer* rend) {
#if ROTATE_TEXTURE == 1
    double angle = RAD_TO_DEG(atan2(body->velocity.y, body->velocity.x));
    SDL_RenderCopyEx(rend, body->texture, NULL, &body->rect, angle, NULL,
                     ((angle > 90) && (angle < 270)) ? SDL_FLIP_VERTICAL : SDL_FLIP_NONE);
#else
    SDL_RenderCopy(rend, body->texture, NULL, &body->rect);
#endif

#if DEBUG_COLLISIONS == 1
    uint8_t r;
    uint8_t g;
    uint8_t b;
    SDL_GetRenderDrawColor(rend, &r, &g, &b, NULL);
    SDL_SetRenderDrawColor(rend, 255, 0, 0, 0);
    SDL_RenderDrawRect(rend, &body->rect);
    SDL_SetRenderDrawColor(rend, 0, 255, 255, 0);
    SDL_RenderDrawRect(rend, &body->collision_rect);
    SDL_SetRenderDrawColor(rend, r, g, b, 0);
#endif
}