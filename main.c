#include <SDL.h>
#include <SDL_timer.h>
#include <SDL_image.h>
#include <stdbool.h>
#include <stdlib.h>
#include "vectors.h"
#include "bodies.h"

#define SCR_WIDTH 1500
#define SCR_HEIGHT 1000

#define PLANT_INIT_AMOUNT 5
#define HERB_INIT_AMOUNT 1000
#define CARN_INIT_AMOUNT 10

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
	
	field.species_heads[species] = new_body;
}

int init_amounts[SPECIES_AMOUNT] = {
		[SPECIES_PLANT] = PLANT_INIT_AMOUNT,
		[SPECIES_HERB] = HERB_INIT_AMOUNT,
		[SPECIES_CARN] = CARN_INIT_AMOUNT,
};

int init_forces[SPECIES_AMOUNT] = {
		[SPECIES_PLANT] = 1,
		[SPECIES_HERB] = 6,
		[SPECIES_CARN] = 4,
};


int init_speeds[SPECIES_AMOUNT] = {
		[SPECIES_PLANT] = 1,
		[SPECIES_HERB] = 3,
		[SPECIES_CARN] = 5,
};


void field_init(SDL_Renderer* rend) {
	for (species_t i = 0; i < SPECIES_AMOUNT; i++) {
		SDL_Texture* tex;
		
		switch (i) {
			case SPECIES_PLANT:
				tex = texture_from_img(rend, "C:\\repos\\SDL_playground\\assets\\sand_grain.png");
				break;
			case SPECIES_HERB:
				tex = texture_from_img(rend, "C:\\repos\\SDL_playground\\assets\\fish.png");
				break;
			case SPECIES_CARN:
				tex = texture_from_img(rend, "C:\\repos\\SDL_playground\\assets\\black.png");
				break;
			default:
				break;
		}
		
		for (int j = 0; j < init_amounts[i]; j++) {
			add_body_node(body_new(rand() % SCR_WIDTH, rand() % SCR_HEIGHT, 5, tex, init_forces[i], init_speeds[i]), i);
		}
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

void move_herbs() {
	body_list_t* current_herb = field.species_heads[SPECIES_HERB];
	int min_dist;
	int dist;
	Body* target_plant;
	
	while (current_herb != NULL) {
		min_dist = SDL_MAX_SINT32;
		target_plant = NULL;
		
		body_list_t* current_plant = field.species_heads[SPECIES_PLANT];
		while (current_plant != NULL) {
			dist = (int) vector_length(vector_sub(current_plant->this->position, current_herb->this->position));
			if ((dist < 1500) && dist < min_dist) {
				min_dist = dist;
				target_plant = current_plant->this;
			}
			
			current_plant = current_plant->next;
		}
		
		if (target_plant) {
			body_seek(current_herb->this, target_plant->position);
		}
		
		current_herb = current_herb->next;
	}
}

void move_plants() {
	body_list_t* current_plant = field.species_heads[SPECIES_PLANT];
	int min_dist;
	int dist;
	Body* target_herb;
	
	while (current_plant != NULL) {
		min_dist = SDL_MAX_SINT32;
		target_herb = NULL;
		
		body_list_t* current_herb = field.species_heads[SPECIES_HERB];
		while (current_herb != NULL) {
			dist = (int) vector_length(vector_sub(current_herb->this->position, current_plant->this->position));
			if ((dist < 20) && dist < min_dist) {
				min_dist = dist;
				target_herb = current_herb->this;
			}
			
			current_herb = current_herb->next;
		}
		
		if (target_herb) {
			body_evade(current_plant->this, target_herb->position);
			current_plant->this->health -= 1;
			if (current_plant->this->health == 0) {
			
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
	int mouse_x, mouse_y;
	
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);
	
	win = SDL_CreateWindow("Particles", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCR_WIDTH,
						   SCR_HEIGHT, 0);
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
		
		move_plants();
		move_herbs();
		
		draw_bodies(rend);
		SDL_RenderPresent(rend);
	}
	
	SDL_DestroyRenderer(rend);
	SDL_DestroyWindow(win);
	SDL_Quit();
	return EXIT_SUCCESS;
}