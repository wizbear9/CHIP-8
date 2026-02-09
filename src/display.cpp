#include <cstdio>
#include "display.h"

Display::Display()
{
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        printf("SDL_Init failed: %s\n", SDL_GetError());
        exit(1);
    }

    window = SDL_CreateWindow(
        "CHIP-8 Emulator",
        W, H, SDL_WINDOW_RESIZABLE);
    if (window == NULL) {
        printf("SDL_CreateWindow failed: %s\n",
            SDL_GetError());
        exit(2);
    }

    renderer = SDL_CreateRenderer(window, NULL);
    if (renderer == NULL) {
        printf("SDL_CreateRenderer failed: %s\n",
            SDL_GetError());
        exit(2);
    }

    SDL_SetRenderLogicalPresentation(renderer, W, H,
        SDL_LOGICAL_PRESENTATION_INTEGER_SCALE);

    texture = SDL_CreateTexture(renderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        64, 32);

    SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST);
}

Display::~Display()
{
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void Display::render_frame(uint8_t *gfx)
{
    uint32_t pixels[2048];
    for (int i = 0; i < 2048; ++i) {
        if (gfx[i] != 0)
            pixels[i] = 0xFFFFFFFF;
        else
            pixels[i] = 0x00000000;
    }
    SDL_UpdateTexture(texture, NULL, pixels, 64 * sizeof(uint32_t));
    SDL_RenderClear(renderer);
    SDL_RenderTexture(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);
}