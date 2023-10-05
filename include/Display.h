#ifndef __DISPLAY_H
#define __DISPLAY_H

#include <SDL.h>

#include <cstdint>
#include <algorithm>
#include <iostream>
#include <mutex>

#define WIDTH 64
#define HEIGHT 32

class Display {
private:

    std::mutex writeLock;
    unsigned char framebuffer[WIDTH + HEIGHT * WIDTH];

public:

    Display() {
        clearDisplay();
    }

    void startWrite() {
        writeLock.lock();
    }

    void stopWrite() {
        writeLock.unlock();
    }

    uint8_t writeDisplayRow(uint8_t x, uint8_t y, uint8_t val) {

        uint8_t collision = 0;

        x %= WIDTH;
        y %= HEIGHT;

        int index = y * WIDTH + x;

        for (int i = 0; i < 8; i++) {

            uint8_t v = (val >> (7 - i)) & 0x01;
            collision |= v & framebuffer[index + i];

            framebuffer[index] = v ^ framebuffer[index];
            index = ((y * WIDTH) + ((index + 1) % WIDTH)) % (WIDTH * HEIGHT);
        }
        return collision;
    }

    void printDisplay() {

        writeLock.lock();
        for (int y = 0; y < HEIGHT; y++) {

            for (int x = 0; x < WIDTH; x++) {
                if (framebuffer[y * HEIGHT + x])
                    std::cout << "#";
                else
                    std::cout << " ";
            }
            std::cout << std::endl;

        }
        std::cout << std::endl;

        writeLock.unlock();
    }

    void clearDisplay() {
        writeLock.lock();
        //std::cout << "clearing the display" << std::endl;
        std::fill(framebuffer,framebuffer + sizeof(framebuffer),0);
        writeLock.unlock();
    }

    void drawDisplay(SDL_Surface *screenSurface) {

        int pixelSizeX = screenSurface->w/WIDTH;
        int pixelSizeY = screenSurface->h/HEIGHT;


        SDL_Rect pixelTarget;
        pixelTarget.w = pixelSizeX;
        pixelTarget.h = pixelSizeY;

        // clear display
        SDL_FillRect(screenSurface, NULL, SDL_MapRGB(screenSurface->format, 0x0, 0x0, 0x0));

        writeLock.lock();

        for (int y = 0; y < HEIGHT; y++) {

            for (int x = 0; x < WIDTH; x++) {

                pixelTarget.x = x*pixelSizeX;
                pixelTarget.y = y*pixelSizeY;
                
                if (framebuffer[y * WIDTH + x])
                    SDL_FillRect(screenSurface, &pixelTarget, SDL_MapRGB(screenSurface->format, 0xFF, 0xFF, 0xFF));
            }
        }

        writeLock.unlock();

    }



private:

};

#endif