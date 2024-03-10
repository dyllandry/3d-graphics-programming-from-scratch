#include "display.h"

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

int max(int x, int y) {
    if (x > y) {
        return x;
    } else {
        return y;
    }
}

int min(int x, int y) {
    if (x > y) {
        return y;
    } else {
        return x;
    }
}

int bound(int value, int min_value, int max_value) {
    return max(min(value, max_value), min_value);
}

void draw_pixel(int x, int y, uint32_t color) {
    if (x < window_width && x >= 0 && y < window_width && y >= 0) {
        int pixel_pos = (y * window_width) + x;
        color_buffer[pixel_pos] = color;
    }
}

void draw_rect(int x, int y, int width, int height, uint32_t color) {
    int x_start = bound(x, 0, window_width);
    int x_end = bound(x + width, 0, window_width);
    int y_start = bound(y, 0, window_height);
    int y_end = bound(y + height, 0, window_height);

    for (int y_current = y_start; y_current < y_end; y_current++) {
        for (int x_current = x_start; x_current < x_end; x_current++) {
            int pixel_pos = (y_current * window_width) + x_current;
            color_buffer[pixel_pos] = color;
        }
    }
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

void destroy_window(void) {
    // Opposite order of allocation.
    free(color_buffer);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

