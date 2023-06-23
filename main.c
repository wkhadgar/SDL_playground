#include <SDL.h>

#define V_PAGES 20
#define PAGE_TYPE uint32_t

#define PAGE_SIZE (sizeof(PAGE_TYPE) << 3)
#define SCR_SIZE (V_PAGES * PAGE_SIZE)

typedef enum {
    UP = -1,
    DOWN = 1,
} vertical_dir_t;

typedef enum {
    LEFT = -1,
    RIGHT = 1,
} horizontal_dir_t;

typedef struct particle {
    struct {
        uint8_t r;
        uint8_t g;
        uint8_t b;
        uint8_t a;
    } color;
    struct {
        uint16_t x;
        uint16_t y;
    } scr;
} particle_t;

struct screen_field {
    PAGE_TYPE field[V_PAGES][SCR_SIZE];
} screen_field = {
        .field = {0},
};

void move_particle_vert(particle_t* particle, vertical_dir_t dir) {

    if ((particle->scr.y + dir < 0) || (particle->scr.y + dir > SCR_SIZE)) {
        return;
    }

    uint32_t* next_page = &screen_field.field[(particle->scr.y + dir) >> 5][particle->scr.x];
    uint32_t page_mask = 1 << ((particle->scr.y + dir) % PAGE_SIZE);

    if (*next_page & page_mask) {
        return;
    }

    screen_field.field[particle->scr.y >> 5][particle->scr.x] &= ~(1 << (particle->scr.y % 32));
    *next_page |= page_mask;
    particle->scr.y += dir;
}

void move_particle_hor(particle_t* particle, horizontal_dir_t dir) {

    if ((particle->scr.x + dir < 0) || (particle->scr.x + dir > V_PAGES * sizeof(uint32_t))) {
        return;
    }


    uint32_t* next_page = &screen_field.field[(particle->scr.y) >> 5][particle->scr.x + dir];
    uint32_t page_mask = 1 << (particle->scr.y % PAGE_SIZE);

    if (*next_page & page_mask) {
        return;
    }

    particle->scr.x += dir;
    *next_page |= page_mask;
    screen_field.field[(particle->scr.y) >> 5][particle->scr.x] &= ~page_mask;
}

void fill_screen(SDL_Renderer* renderer, uint8_t r, uint8_t g, uint8_t b) {
    SDL_SetRenderDrawColor(renderer, r, g, b, 0);
    SDL_RenderClear(renderer);
}

int main(int argc, char* argv[]) {
    SDL_Event event;
    SDL_Renderer* renderer;
    SDL_Window* window;

    particle_t p = {
            .color = {255, 255, 0, 255},
            .scr = {100, 0},
    };

    SDL_Init(SDL_INIT_VIDEO);
    SDL_CreateWindowAndRenderer(SCR_SIZE, SCR_SIZE, 0, &window, &renderer);
    while (1) {
        if (SDL_PollEvent(&event) && event.type == SDL_QUIT) {
            break;
        }

        fill_screen(renderer, 0, 0, 0);

        SDL_SetRenderDrawColor(renderer, p.color.r, p.color.g, p.color.b, p.color.a);
        SDL_RenderDrawPoint(renderer, p.scr.x, p.scr.y);
        move_particle_vert(&p, DOWN);
        SDL_RenderPresent(renderer);


    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return EXIT_SUCCESS;
}