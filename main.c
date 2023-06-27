#include <SDL.h>
#include <SDL_timer.h>
#include <SDL_image.h>
#include <stdbool.h>
#include "vectors.h"
#include "bodies.h"

#define SCR_WIDTH 1500
#define SCR_HEIGHT 1000

#define PLANT_INIT_AMOUNT 1
#define HERB_INIT_AMOUNT 1000
#define CARN_INIT_AMOUNT 0

typedef enum {
    SPECIES_PLANT = 0,
    SPECIES_HERB,
    SPECIES_CARN,

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

typedef struct body_list {
	Body* this;
	struct body_list* next;
	struct body_list* prev;
} body_list_t;

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
		[SPECIES_PLANT] = PLANT_INIT_AMOUNT,
		[SPECIES_HERB] = HERB_INIT_AMOUNT,
		[SPECIES_CARN] = CARN_INIT_AMOUNT,
};

double init_masses[SPECIES_AMOUNT] = {
        [SPECIES_PLANT] = 30,
        [SPECIES_HERB] = 20,
        [SPECIES_CARN] = 100,
};

double init_speeds[SPECIES_AMOUNT] = {
        [SPECIES_PLANT] = 0.1,
        [SPECIES_HERB] = 4,
        [SPECIES_CARN] = 5,
};


SDL_Rect window_boundary;

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

        add_body_node(body_new(rand() % SCR_WIDTH, rand() % SCR_HEIGHT, init_masses[species] + mass_variance, tex,
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
            case SPECIES_HERB:
                tex = texture_from_img(rend, "assets\\fish.png");
                break;
            case SPECIES_CARN:
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

void handle_herbs(bool to_mouse, Vector2_t m_pos) {
    body_list_t* current_herb = field.species_heads[SPECIES_HERB];
    int min_dist;
    int dist;
    Body const* target_plant;

    while (current_herb != NULL) {
        min_dist = SDL_MAX_SINT32;
        target_plant = NULL;

        body_list_t* current_plant = field.species_heads[SPECIES_PLANT];
        while (current_plant != NULL) {
            dist = (int) vector_length(vector_sub(current_plant->this->position, current_herb->this->position));
            if ((dist < 500) && dist < min_dist) {
                min_dist = dist;
                target_plant = current_plant->this;
            }

            current_plant = current_plant->next;
        }

        if (to_mouse) {
            body_seek(current_herb->this, m_pos);
        } else if (target_plant) {
            body_arrive(current_herb->this, target_plant->position);
        } else {
            body_wander(current_herb->this);
        }

        body_keep_inside(current_herb->this, window_boundary);
        current_herb = current_herb->next;
    }
}

void handle_plants() {
	body_list_t* current_plant = field.species_heads[SPECIES_PLANT];
	int min_dist;
	int dist;
	Body* target_herb;

    int debug = 0;
	
	while (current_plant != NULL) {
        debug++;
		min_dist = SDL_MAX_SINT32;
		target_herb = NULL;
		
		body_list_t* current_herb = field.species_heads[SPECIES_HERB];
		while (current_herb != NULL) {
			dist = (int) vector_length(vector_sub(current_herb->this->position, current_plant->this->position));
			if ((dist < (current_plant->this->rect.w/2)) && dist < min_dist) {
				min_dist = dist;
				target_herb = current_herb->this;
			}
			
			current_herb = current_herb->next;
		}
		
		if (target_herb) {
            body_arrive(current_plant->this, target_herb->position);
            current_plant->this->health -= 1;
            target_herb->health += 1;
            if (current_plant->this->health == 0) {
                body_list_t* tmp = current_plant;
                if (current_plant->prev) {
                    current_plant->prev->next = current_plant->next;
                } else {
                    field.species_heads[SPECIES_PLANT] = current_plant->next;
                }
                if (current_plant->next) {
                    current_plant->next->prev = current_plant->prev;
                }
                current_plant = current_plant->next;
                free(tmp);
                continue;
            }
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

    window_boundary.x = 0;
    window_boundary.y = 0;
    win = SDL_CreateWindow("Particles", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCR_WIDTH,
                           SCR_HEIGHT, 0);

    window_boundary.w = SCR_WIDTH;
    window_boundary.h = SCR_HEIGHT;
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

        handle_herbs((mouse_buttons & SDL_BUTTON(SDL_BUTTON_LEFT)), mouse_pos);
        handle_plants();

        draw_bodies(rend);
        SDL_RenderPresent(rend);

        SDL_Delay(1000 / 60);
    }

    SDL_DestroyRenderer(rend);
    SDL_DestroyWindow(win);
    SDL_Quit();
    return EXIT_SUCCESS;
}