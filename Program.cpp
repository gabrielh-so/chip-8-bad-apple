#include <iostream>

#include "include/Chip8.h"
#include "include/UpdateKeys.h"
#include <SDL.h>

#include <thread>
#include <mutex>

#include <fstream>

#include <filesystem>

#include <semaphore>

#include <chrono>

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 320
//#define SCREEN_WIDTH 720
//#define SCREEN_HEIGHT 360

//std::mutex threadLock;
std::binary_semaphore threadLock{1};

void startEmulator(Chip8 *c8) {

    while (!threadLock.try_acquire()) {

        try {
            c8->tick();
            //std::cout << "1" << std::endl;
        } catch (std::exception e) {
            std::cout << e.what() << std::endl;
        }
    }

    threadLock.release();
}

auto getAwakeTime() {
    // it's good enough(TM)
    return std::chrono::system_clock::now() + std::chrono::microseconds(1000000/60);
}

int main(int argc, char *argv[]) {

    SDL_Window* window = NULL;
  
    SDL_Surface* screenSurface = NULL;

    bool quit = false;

    SDL_Event e;

    Display d;

    Chip8 c8;

    c8.reset();


    if (argc > 1) {
        // load memory from file location given in argv[]
        if (argc > 2) {
            std::cout << "USAGE: ./chip-8 [program_file_path]" << std::endl;
            return 1;
        }

        //std::string cwd = getcwd();

        std::ifstream is;
        is.open(argv[1]);
        //is.open(cwd + argv[1]);
        //std::cout << cwd << argv[1] << std::endl;

        //is.open(std::filesystem::current_path() + argv[1]);
        //is.open(argv[1]);
        //std::cout << argv[1] << std::endl;

        //std::cout << getcwd() << std::endl;
        /*
        if (is.is_open()) {
            std::cout << "error opening program file" << std::endl;
            return 1;
        }
        */

        char program[MEM_SIZE];
        is.seekg(0, is.end);
        int length = is.tellg();
        std::cout << "length: " << length << std::endl;
        is.seekg(0, is.beg);

        is.read(program, length);

        is.close();

        c8.writeProgram((uint8_t*)program, length);
    }

    uint16_t k = 0;

    c8.setDisplay(&d);

    threadLock.acquire();
    
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("sdl could not be initialised - error: %s", SDL_GetError());
    }
    else {
        // create the window
        window = SDL_CreateWindow("chip-8", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

        if (window == NULL ) {
            printf("window could not be created - error: %s", SDL_GetError());
        }
        else {
            screenSurface = SDL_GetWindowSurface(window);

            // start the emulation thread
            std::thread emulatorThread(startEmulator, &c8);

            auto awakeTime = getAwakeTime();
            
            while (!quit) {

                //std::cout << "2" << std::endl;

                while( SDL_PollEvent( &e ) != 0 ) {
                    // quit event
                    if( e.type == SDL_QUIT ) {
                        quit = true;
                        //std::cout << "closing threads and quitting" << std::endl;

                        // allows emulator thread to exit
                        threadLock.release();

                        // input update forces emulator out of input wait
                        c8.updateInput(0);
                        c8.updateInput(1);

                        emulatorThread.join();

                        //std::cout << "emulator thread stopped" << std::endl;
                    }
                    // keydown event
                    else if( e.type == SDL_KEYDOWN) {

                        //std::cout << "3" << std::endl;
                        uint16_t oldK = k;
                        //std::cout << "key down" << std::endl;
                        //std::cout << "old k: " << oldK << std::endl;
                        k = updateKeyDown(e, k);
                        //std::cout << "new k: " << k << std::endl;

                        if (k != oldK) {
                            c8.updateInput(k);
                        }
                        
                    }
                    // keyup event
                    else if (e.type == SDL_KEYUP) {

                        //std::cout << "4" << std::endl;

                        uint16_t oldK = k;
                        //std::cout << "key up" << std::endl;
                        k = updateKeyUp(e, k);
                        //std::cout << "new k: " << k << std::endl;

                        if (k != oldK) {
                            c8.updateInput(k);
                        }

                        //std::cout << "5" << std::endl;
                    }
                }

                if (quit) break;

                d.drawDisplay(screenSurface);
                
                SDL_UpdateWindowSurface(window);

                // check for buzzer

                if (c8.buzzerActive()) {
                    // do a buzzer noise here
                }
                
                // wait for delay before updating timers
                //SDL_Delay(1000/60);

                std::this_thread::sleep_until(awakeTime);
                awakeTime = getAwakeTime();

                c8.updateTimers();
            }
        
        }
    }
    
    SDL_DestroyWindow(window);
    
    SDL_Quit();
    
    return 0;
}
