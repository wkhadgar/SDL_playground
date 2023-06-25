#include <SDL.h>
#include "SDL_image.h"
#include "SDL2_gfxPrimitives.h"
#include <stdbool.h>
#include "simulation/bodies.h"

#define SCR_WIDTH 1000
#define SCR_HEIGHT 800

#define ELLIPSE_PRECISION 5

typedef enum __attribute__((packed)) {
	SPECIES_DEFAULT = 0,
	
	SPECIES_AMOUNT,
} species_t;

typedef enum __attribute__((packed)) {
	COLORS_BLACK,
	COLORS_WHITE,
	COLORS_YELLOW,
	
	COLORS_AMOUNT,
} colors_t;

typedef struct {
	uint8_t r;
	uint8_t g;
	uint8_t b;
	uint8_t a;
} color_t;

const color_t colors[COLORS_AMOUNT] = {
		[COLORS_BLACK] =  {0, 0, 0, 0},
		[COLORS_WHITE] =  {255, 255, 255, 0},
		[COLORS_YELLOW] = {255, 255, 0, 0},
};

void set_render_color(SDL_Renderer* render, color_t color) {
	SDL_SetRenderDrawColor(render, color.r, color.g, color.b, color.a);
}

void fill_screen(SDL_Renderer* renderer, color_t color) {
	set_render_color(renderer, color);
	SDL_RenderClear(renderer);
}

void sdl_ellipse(SDL_Renderer* r, int x0, int y0, int radiusX, int radiusY) {
	
	float pih = M_PI / 2.0; //half of pi
	float theta = 0;     // angle that will be increased each loop
	
	//starting point
	int x = (int) (radiusX * cos(theta));//start point
	int y = (int) (radiusY * sin(theta));//start point
	int x1 = x;
	int y1 = y;
	
	//repeat until theta >= 90;
	float step = pih / (float) ELLIPSE_PRECISION; // amount to add to theta each time (degrees)
	for (theta = step; theta <= pih; theta += step)//step through only a 90 arc (1 quadrant)
	{
		//get new point location
		x1 = (int) (((float) radiusX * cosf(theta)) + 0.5); //new point (+.5 is a quick rounding method)
		y1 = (int) (((float) radiusY * sinf(theta)) + 0.5); //new point (+.5 is a quick rounding method)
		
		//draw line from previous point to new point, ONLY if point incremented
		if ((x != x1) || (y != y1))//only draw if coordinate changed
		{
			SDL_RenderDrawLine(r, x0 + x, y0 - y, x0 + x1, y0 - y1);//quadrant TR
			SDL_RenderDrawLine(r, x0 - x, y0 - y, x0 - x1, y0 - y1);//quadrant TL
			SDL_RenderDrawLine(r, x0 - x, y0 + y, x0 - x1, y0 + y1);//quadrant BL
			SDL_RenderDrawLine(r, x0 + x, y0 + y, x0 + x1, y0 + y1);//quadrant BR
		}
		//save previous points
		x = x1;//save new previous point
		y = y1;//save new previous point
	}
	//arc did not finish because of rounding, so finish the arc
	if (x != 0) {
		x = 0;
		SDL_RenderDrawLine(r, x0 + x, y0 - y, x0 + x1, y0 - y1);//quadrant TR
		SDL_RenderDrawLine(r, x0 - x, y0 - y, x0 - x1, y0 - y1);//quadrant TL
		SDL_RenderDrawLine(r, x0 - x, y0 + y, x0 - x1, y0 + y1);//quadrant BL
		SDL_RenderDrawLine(r, x0 + x, y0 + y, x0 + x1, y0 + y1);//quadrant BR
	}
}

void draw_body(SDL_Renderer* renderer, Body body) {
	set_render_color(renderer, colors[COLORS_YELLOW]);
	sdl_ellipse(renderer, body.pos.x, body.pos.y, body.mass, body.mass);
}

int main(int argc, char* argv[]) {
	SDL_Event event;
	SDL_Renderer* renderer;
	SDL_Window* window;
	
	Vector2 mouse_pos;
	int mouse_x, mouse_y;
	bool mouse_pressed = false;
	
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);
	SDL_CreateWindowAndRenderer(SCR_WIDTH, SCR_HEIGHT, 0, &window, &renderer);
	
	
	Body seeker = {
			.velocity = {0},
			.pos = {SCR_WIDTH / 2, SCR_HEIGHT / 2},
			.mass = 5,
			.health = 100,
	};
	
	while (1) {
		fill_screen(renderer, colors[COLORS_BLACK]);
		
		if (SDL_PollEvent(&event) && event.type == SDL_QUIT) {
			break;
		}
		
		SDL_GetMouseState(&mouse_x, &mouse_y);
		mouse_pos.x = mouse_x;
		mouse_pos.y = mouse_y;
		
		switch (event.type) {
			case SDL_MOUSEBUTTONDOWN:
				mouse_pressed = !mouse_pressed;
				break;
			case SDL_MOUSEBUTTONUP:
			default:
				break;
		}
		
		if (mouse_pressed) {
			body_seek(&seeker, mouse_pos);
		}
		
		draw_body(renderer, seeker);
		SDL_RenderPresent(renderer);
		SDL_Delay(10);
	}
	
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return EXIT_SUCCESS;
}