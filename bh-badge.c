#include <stdio.h>
#include "bh-badge.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>


SDL_Event event;

//SDL2 variables
void* nullptr;
SDL_Window *win;
SDL_Renderer *ren;

//These values are specific to the SDL2 emulator
#define PIXELRADIUS     8      
#define PIXELSPACING    5


void initDisplay(void) {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        printf("SDL_Init Error: %s\n", SDL_GetError());
    }

    win = SDL_CreateWindow(
        "sdlTest",
        100,
        100,
        (TOTPIXELX*(PIXELSPACING+PIXELRADIUS+PIXELRADIUS))+PIXELSPACING,
        (TOTPIXELY*(PIXELSPACING+PIXELRADIUS+PIXELRADIUS))+PIXELSPACING,
        SDL_WINDOW_SHOWN);

    if (win == nullptr) {
        printf("SDL_CreateWindow Error: %s\n", SDL_GetError());
    }

    ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (ren == nullptr) {
        printf("SDL_CreateRenderer Error: %s\n", SDL_GetError());
    }
    
    displayClear(BLACK);
    
    //Write grey cirles (LEDs in off state)
    for (uint8_t i=0; i<TOTPIXELX; i++) {
        for (uint8_t j=0; j<TOTPIXELY; j++) {
            displayPixel(i,j,GREY);
        }
    }
}

void displayClear(uint8_t color) {
    SDL_SetRenderDrawColor(ren, colors[color][0], colors[color][1], colors[color][2], 255);
    SDL_RenderClear(ren);
}

void displayPixel(uint8_t x, uint8_t y, uint8_t color) {
    /*
    SDL_Rect rect;
    rect.x = (x*(PIXELMULTIPLE+PIXELSPACING))+PIXELSPACING;
    rect.y = (y*(PIXELMULTIPLE+PIXELSPACING))+PIXELSPACING;
    rect.w = PIXELMULTIPLE;
    rect.h = PIXELMULTIPLE;

    SDL_SetRenderDrawColor(ren, 0, 0, 255, 255);
    SDL_RenderFillRect(ren, &rect);
    */
    filledCircleRGBA(
        ren,
        (x*(PIXELRADIUS+PIXELRADIUS+PIXELSPACING))+PIXELRADIUS+PIXELSPACING,
        (y*(PIXELRADIUS+PIXELRADIUS+PIXELSPACING))+PIXELRADIUS+PIXELSPACING,
        PIXELRADIUS,
        colors[color][0],
        colors[color][1],
        colors[color][2],
        255
        );
}

void displayLatch(void) {
    SDL_RenderPresent(ren);
}

void displayClose(void) {
    SDL_DestroyWindow(win);
    SDL_Quit();
}

uint8_t getControl(void) {
    if (SDL_PollEvent(&event)) {
        /*
        if (event.type == SDL_QUIT) {
            gameRunning = 0;
        }
        */
        if (event.type == SDL_KEYDOWN) {
            SDL_Keycode keyPressed = event.key.keysym.sym;

            switch (keyPressed)
            {
                case SDLK_ESCAPE:
                    return ESCAPE;
                case SDLK_UP:
                    return UP;
                case SDLK_DOWN:
                    return DOWN;
                case SDLK_LEFT:
                    return LEFT;
                case SDLK_RIGHT:
                    return RIGHT;
                case SDLK_SPACE:
                    return BUTTON;
            }
        }
    }
    return NOINPUT;
}

uint32_t getTime(void) {
    return SDL_GetTicks();
}

void controlDelayMs(uint16_t ms) {
    SDL_Delay(ms);
}

int main(void) {
    initDisplay();
    displayLatch();
    
    animateBadge();
    return 0;
}
