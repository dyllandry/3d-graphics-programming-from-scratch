#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <SDL2/SDL.h>

bool is_running = false;
SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;

uint32_t* color_buffer = NULL;

int window_width = 800;
int window_height = 600;

// Returns bool if SDL initialization worked.
bool initialize_window(void) {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        fprintf(stderr, "Error initializing SDL.\n");
        return false;
    }

    window = SDL_CreateWindow(
        NULL,                   // window title
        SDL_WINDOWPOS_CENTERED, // window pos x
        SDL_WINDOWPOS_CENTERED, // window pos y
        window_width,                    // window width
        window_height,                    // window height
        SDL_WINDOW_BORDERLESS
    );

    if (!window) {
        fprintf(stderr, "Errror creating SDL window.\n");
        return false;
    }

    renderer = SDL_CreateRenderer(
        window,
        -1, // display device, -1 is default.
        0   // extra flags
    );

    if (!renderer) {
        fprintf(stderr, "Error creating SDL renderer.\n");
        return false;
    }

    return true;
}

void setup(void) {
    int num_pixels = window_width * window_height;
    color_buffer = malloc(sizeof *color_buffer * num_pixels);
}

void process_input(void) {
    SDL_Event event;
    SDL_PollEvent(&event);

    switch (event.type) {
        // When the window's X button is pressed.
        case SDL_QUIT:
            is_running = false;
            break;
        case SDL_KEYDOWN:
            if (event.key.keysym.sym == SDLK_ESCAPE) {
                is_running = false;
            }
            break;
    }
}

void update(void) {

}

void render(void) {
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);
}

void destroy_window(void) {
    // Opposite order of allocation.
    free(color_buffer);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

int main(void) {
    is_running = initialize_window();

    setup();

    while (is_running) {
        process_input();
        update();
        render();
    }

    destroy_window();

    return 0;
}
