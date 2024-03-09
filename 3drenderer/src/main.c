#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <SDL2/SDL.h>

bool is_running = false;

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;

uint32_t* color_buffer = NULL;
SDL_Texture* color_buffer_texture = NULL;

int window_width = 800;
int window_height = 600;

// Returns bool if SDL initialization worked.
bool initialize_window(void) {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        fprintf(stderr, "Error initializing SDL.\n");
        return false;
    }

    // Use SDL to query what is the fullscreen width and height.
    SDL_DisplayMode display_mode;
    SDL_GetCurrentDisplayMode(
        0, // 0 is the default display
        &display_mode
    );
    window_width = display_mode.w;
    window_height = display_mode.h;

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

    SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);

    return true;
}

void setup(void) {
    int num_pixels = window_width * window_height;
    color_buffer = malloc(sizeof *color_buffer * num_pixels);

    color_buffer_texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        window_width,
        window_height
    );
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

void draw_grid(void) {
    // Draw a background grid that fills the entire window.
    // Lines should be rendered at every row/col multiple of 10.
    for (int y = 0; y < window_height; y++) {
        for (int x = 0; x < window_width; x++) {
            uint32_t grid_color = 0xFF888888;
            uint32_t background_color = 0xFF000000;

            uint32_t pixel_color = background_color;
            bool is_pixel_on_grid_line = y % 10 == 0 || x % 10 == 0;
            if (is_pixel_on_grid_line) {
                pixel_color = grid_color;
            }

            int pixel_pos = (y * window_width) + x;
            color_buffer[pixel_pos] = pixel_color;
        }
    }
}

void render_color_buffer(void) {
    SDL_UpdateTexture(
        color_buffer_texture,
        NULL,
        color_buffer,
        // row size
        (int)(sizeof *color_buffer * window_width)
    );
    SDL_RenderCopy(
        renderer,
        color_buffer_texture,
        NULL,
        NULL
    );
}

void clear_color_buffer(uint32_t color) {
    for (int y = 0; y < window_height; y++) {
        for (int x = 0; x < window_width; x++) {
            color_buffer[(window_width * y) + x] = color;
        }
    }
}

void render(void) {
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderClear(renderer);

    draw_grid();

    render_color_buffer();
    clear_color_buffer(0xFFFFFF00);

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
