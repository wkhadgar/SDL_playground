#include <SDL.h>
#include <SDL_timer.h>
#include <SDL_image.h>
#include <stdbool.h>
#include "vectors.h"
#include "bodies.h"

#define INIT_CREATION_WIDTH 1000
#define INIT_CREATION_HEIGHT 1000

typedef enum {
    SPECIES_PLANT = 0,
    SPECIES_PREY,
    SPECIES_PREDATOR,

    SPECIES_AMOUNT,
} species_t;

SDL_Texture* texture_from_img(SDL_Renderer* rend, char* path) {
	SDL_Surface* surf = IMG_Load(path);
	if (surf == NULL) {
		printf("%s\n", SDL_GetError());
	}
	SDL_Texture* tex = SDL_CreateTextureFromSurface(rend, surf);
	if (tex == NULL) {
		printf("%s\n", SDL_GetError());
	}
	SDL_FreeSurface(surf);
	return tex;
}

struct field {
	body_list_t* species_heads[SPECIES_AMOUNT];
	
} field = {
		.species_heads = {NULL},
};

void add_body_node(Body* body, species_t species) {
    body_list_t* new_body = (body_list_t*) malloc(sizeof(body_list_t));
    new_body->this = body;
    new_body->next = field.species_heads[species];
    new_body->prev = NULL;

    if (field.species_heads[species]) {
        field.species_heads[species]->prev = new_body;
    }
    field.species_heads[species] = new_body;
}

int init_amounts[SPECIES_AMOUNT] = {
        [SPECIES_PLANT] = 2,
        [SPECIES_PREY] = 1000,
        [SPECIES_PREDATOR] = 1,
};

double init_masses[SPECIES_AMOUNT] = {
        [SPECIES_PLANT] = 30,
        [SPECIES_PREY] = 20,
        [SPECIES_PREDATOR] = 30,
};

double init_speeds[SPECIES_AMOUNT] = {
        [SPECIES_PLANT] = 0.1,
        [SPECIES_PREY] = 4,
        [SPECIES_PREDATOR] = 20,
};


SDL_Rect window_boundary = {0, 0, 0, 0};

void create_bodies(species_t species, SDL_Texture* tex) {
    double mass_variance;
    double speed_variance;
    double mass_bias;
    double speed_bias;

    mass_bias = init_masses[species] * 50;
    speed_bias = init_speeds[species] * 50;


    for (int i = 0; i < init_amounts[species]; i++) {

        mass_variance = ((rand() % (int) mass_bias) - (mass_bias / 2)) / 100;
        speed_variance = ((rand() % (int) speed_bias) - (speed_bias / 2)) / 100;

        add_body_node(body_new((rand() % INIT_CREATION_WIDTH) + 400, (rand() % INIT_CREATION_HEIGHT),
                               init_masses[species] + mass_variance, tex,
                               init_speeds[species] + speed_variance), species);
    }
}

void field_init(SDL_Renderer* rend) {
    for (species_t i = 0; i < SPECIES_AMOUNT; i++) {
        SDL_Texture* tex;

        switch (i) {
            case SPECIES_PLANT:
                tex = texture_from_img(rend, "assets\\seaweed.png");
                break;
            case SPECIES_PREY:
                tex = texture_from_img(rend, "assets\\fish.png");
                break;
            case SPECIES_PREDATOR:
                tex = texture_from_img(rend, "assets\\shark.png");
                break;
            default:
                break;
        }

        create_bodies(i, tex);

    }
}

void draw_bodies(SDL_Renderer* rend) {
	
	for (species_t i = 0; i < SPECIES_AMOUNT; i++) {
		body_list_t* current_body = field.species_heads[i];
		
		while (current_body != NULL) {
			body_draw(current_body->this, rend);
			current_body = current_body->next;
		}
	}
}

void handle_predators(bool to_mouse, Vector2_t m_pos) {
    Body const* tgt_prey;


    body_list_t* current_predator = field.species_heads[SPECIES_PREDATOR];
    while (current_predator != NULL) {

        if (to_mouse) {
            body_seek(current_predator->this, m_pos);
        } else if (!body_keep_inside(current_predator->this, window_boundary)) {
            tgt_prey = body_get_closest_body_from(current_predator->this, field.species_heads[SPECIES_PREY], 500);

            if (tgt_prey) {
                body_seek(current_predator->this, tgt_prey->position);
            } else {
                body_wander(current_predator->this);
            }
        }

        current_predator = current_predator->next;
    }
}

void handle_preys(bool to_mouse, Vector2_t m_pos) {
    Body* tgt_predator;
    Body const* tgt_plant;


    body_list_t* current_prey = field.species_heads[SPECIES_PREY];
    while (current_prey != NULL) {

        if (current_prey->this->health <= 0) {
            body_list_t* tmp = current_prey;
            current_prey = current_prey->next;
            body_list_delete_node(&field.species_heads[SPECIES_PREY], tmp);
            continue;
        }

        if (to_mouse) {
            body_seek(current_prey->this, m_pos);
        } else if (!body_keep_inside(current_prey->this, window_boundary)) {
            tgt_predator = body_get_closest_body_from(current_prey->this, field.species_heads[SPECIES_PREDATOR], 250);
            tgt_plant = body_get_closest_body_from(current_prey->this, field.species_heads[SPECIES_PLANT], 350);

            if (tgt_predator) {
                if (SDL_HasIntersection(&tgt_predator->collision_rect, &current_prey->this->rect) == SDL_TRUE) {
                    current_prey->this->health -= 5;
                    tgt_predator->health += 1;
                    body_grow(tgt_predator, current_prey->this->mass / 200);
                }
                body_flee(current_prey->this, tgt_predator->position);
            } else if (tgt_plant) {
                body_arrive(current_prey->this, tgt_plant->position);
            } else {
                body_wander(current_prey->this);
            }
        }

        current_prey = current_prey->next;
    }
}

void handle_plants() {
    Body* tgt_herb;

    body_list_t* current_plant = field.species_heads[SPECIES_PLANT];
    while (current_plant != NULL) {

        if (current_plant->this->health <= 0) {
            body_list_t* tmp = current_plant;
            current_plant = current_plant->next;
            body_list_delete_node(&field.species_heads[SPECIES_PLANT], tmp);
            continue;
        }

        tgt_herb = body_get_closest_body_from(current_plant->this, field.species_heads[SPECIES_PREY],
                                              current_plant->this->rect.w);

        if (tgt_herb) {
            body_arrive(current_plant->this, tgt_herb->position);
            current_plant->this->health -= 1;
            tgt_herb->health += 1;
            body_grow(tgt_herb, current_plant->this->mass / 1000);
        }

        current_plant = current_plant->next;
    }
}

int main(int argc, char* argv[]) {
    SDL_Event event;
    SDL_Window* win;
    SDL_Renderer* rend;
    Vector2_t mouse_pos;
    uint32_t mouse_buttons;
    int mouse_x;
    int mouse_y;

    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);

    win = SDL_CreateWindow("Particles", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, INIT_CREATION_WIDTH,
                           INIT_CREATION_HEIGHT, SDL_WINDOW_FULLSCREEN_DESKTOP);
    SDL_Metal_GetDrawableSize(win, &window_boundary.w, &window_boundary.h);

    rend = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    field_init(rend);
    bool run = true;
    while (run) {
        SDL_RenderClear(rend);

        SDL_PollEvent(&event);

        mouse_buttons = SDL_GetMouseState(&mouse_x, &mouse_y);
        mouse_pos.x = mouse_x;
        mouse_pos.y = mouse_y;

        switch (event.type) {
            case SDL_QUIT:
                run = false;
                break;
            case SDL_KEYDOWN:
                switch (event.key.keysym.scancode) {
                    case SDL_SCANCODE_ESCAPE:
                        run = false;
                        break;
                    default:
                        break;
                }
                break;
            default:
                break;
        }

        draw_bodies(rend);
        SDL_RenderPresent(rend);

        handle_plants();
        handle_preys((mouse_buttons & SDL_BUTTON(SDL_BUTTON_LEFT)), mouse_pos);
        handle_predators((mouse_buttons & SDL_BUTTON(SDL_BUTTON_RIGHT)), mouse_pos);

    }

    SDL_DestroyRenderer(rend);
    SDL_DestroyWindow(win);
    SDL_Quit();
    return EXIT_SUCCESS;
}