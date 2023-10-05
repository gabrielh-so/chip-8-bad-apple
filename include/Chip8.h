#ifndef __CHIP8_H
#define __CHIP8_H

#include <time.h>

#include <cstdint>
#include <algorithm>

#include <iostream>

#include <chrono>
#include <thread>

#include <memory>

#include <atomic>

#include <mutex>

#include <fstream>

#include <string>

#include <semaphore>

#include "./Display.h"

#define MEM_SIZE 1400000

//static int keyReadCount = 0;

class Chip8 {
private:

    uint32_t I;

    uint16_t PC;
    uint8_t SP;

    // delay and sound timers (should be atomic)
    std::mutex timerLock;
    std::atomic<uint8_t> DT;
    std::atomic<uint8_t> ST;

    // gp registers
    uint8_t V[16];

    // call stack
    uint16_t S[16];

    // input register (should be atomic)
    std::atomic<uint16_t> K;

    // emulator flag for input register update
    std::atomic<uint8_t> Kset;

    // emulator lock for input
    std::mutex KWriteLock;
    //std::binary_semaphore KWriteLock{1};

    // the address the program counter should be set to on reset
    uint16_t PCreset;

    // main memory
    uint8_t mem[MEM_SIZE];

    Display *display;

public:
    Chip8();

    ~Chip8();

    // write program to memory starting at PCreset
    void writeProgram(uint8_t *prog, unsigned int length) {
        std::copy(prog, prog + length, mem + PCreset);

    }

    void dumpMemory(unsigned int start, unsigned int length, std::string path) {

        std::ofstream os(path);
        //os.open(path);
        if (os.is_open()) {
            // file has been opened, so write memory
            const char* s = (char*)mem + start;

            os.write(s, length);
            
            os.close();

        } else {
            std::cout << "could not dump to memory" << std::endl;
        }

    }

    void setDefaultPC(uint16_t pcreset) {
        PCreset = pcreset;
    }

    void setDisplay(Display *d) {
        display = d;
    }

    void updateTimers() {
        std::lock_guard<std::mutex> tL(timerLock);
        DT = std::max(DT - 1, 0);
        ST = std::max(ST - 1, 0);
    }

    void updateInput(uint16_t k) {
        /*
        std::cout << "trying to lock" << std::endl;
        int tryCount = 0;
        while (!KWriteLock.try_lock()) {
            tryCount++;
            if (tryCount > 10000) {
                std::cout << "lock failed" << std::endl;
            }
        }
        */
        std::unique_lock<std::mutex> kwl(KWriteLock, std::defer_lock);
        //KWriteLock.acquire();

        /*
        while (!kwl.try_lock()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            std::cout << "main can't lock" << std::endl;
        }
        */
        //std::cout << "main trying to lock" << std::endl;
        //KWriteLock.lock();
        //std::cout << "main locked" << std::endl;
        //std::cout << "updating input k: " <<  k << std::endl;
        K = k;
        //std::cout << "main unlocked" << std::endl;
        Kset = 1;
        KWriteLock.unlock();
        
        //KWriteLock.release();


    }

    bool buzzerActive() {
        return ST > 0;
    }

    void tick() {

        // get the next instruction
        //if (PC > 300) PC = 200;
        uint16_t cir = (mem[PC & 0xFFF] << 8) + mem[(PC + 1) & 0xFFF];

        // get the instruction type from the opcode
        uint8_t ins = (cir & 0xF000) >> 12;

        //std::cout << "SP " << (int)SP << std::endl;
        //std::cout << std::endl;
        //std::cout << "PC " << PC << std::endl;
        //std::cout << "cir " << cir << std::endl;
        //std::cout << std::endl;

        switch (ins) {
            case 0x00:
              {
                uint8_t op = (cir & 0x0FF);
                switch (op) {
                    case 0x0E0:
                        display->clearDisplay();
                        break;
                    case 0x0EE:
                        PC = S[SP--];
                        break;
                }
                break;
              }
            case 0x01:
                // jump to location nnn
                //uint16_t a = (cir && 0xFFF);
                PC = (cir & 0xFFF);
                break;
            case 0x02:
                // call subroutine at nnn
                S[++SP] = PC;
                PC = (cir & 0xFFF);
                break;
            case 0x03:
                // skip next instruction if Vx == kk
                PC += 2 * (V[(cir & 0xF00) >> 8] == (cir & 0x0FF));
                break;
            case 0x04:
                // skip next instruction if Vx != kk
                PC += 2 * (V[(cir & 0xF00) >> 8] != (cir & 0x0FF));
                break;
            case 0x05:
                // skip next instruction if Vx == Vy
                PC += 2 * (V[(cir & 0xF00) >> 8] == V[(cir & 0x0F0) >> 4]);
                break;
            case 0x06:
                V[(cir & 0xF00) >> 8] = (cir & 0x0FF);
                break;
            case 0x07:
                V[(cir & 0xF00) >> 8] += (cir & 0x0FF);
                break;
            case 0x08:
              {
                uint8_t op = (cir & 0x0F);
                uint8_t x = (cir & 0xF00) >> 8;
                uint8_t y = (cir & 0x0F0) >> 4;
                switch (op) {
                    case 0x00:
                        V[x] = V[y];
                        break;
                    case 0x01:
                        V[x] |= V[y];
                        break;
                    case 0x02:
                        V[x] &= V[y];
                        break;
                    case 0x03:
                        V[x] ^= V[y];
                        break;
                    case 0x04:

                        // preemptive check for overflow
                        // true if Vy > 255 - Vx
                        V[15] = V[y] > (0xFF - V[x]);

                        V[x] += V[y];
                        break;
                    case 0x05:
                        // preemptive check for underflow (check if this is right)
                        // true if Vx > Vy
                        V[15] = V[x] > V[y];
                        V[x] -= V[y];
                        break;
                    case 0x06:
                        V[15] = V[x] & 0x01;
                        V[x]  = V[x] >> 2;
                        break;
                    case 0x07:
                        V[15] = V[y] > V[x];
                        V[x] = V[y] - V[x];
                        break;
                    case 0x0E:
                        V[15] = V[x] & 0x80;
                        V[x]  = V[x] << 2;
                        break;
                }
                break;
              }
            case 0x09:
                PC += 2 * (V[(cir & 0xF00) >> 8] != V[(cir & 0x0F0) >> 4]);
                break;
            case 0x0A:
                I = (cir & 0xFFF);
                break;
            case 0x0B:
                PC = (cir & 0xFFF) + V[0];
                break;
            case 0x0C:
                V[(cir & 0xF00) >> 8] = (uint8_t)(std::rand() % 256) & (cir & 0x0FF);
                break;
            case 0x0D:
              {

                V[15] = 0x0;
                display->startWrite();
                for (uint8_t i = 0; i < (cir & 0x0F); i++) {
                    V[15] |= display->writeDisplayRow(V[((cir & 0xF00) >> 8)], V[(cir & 0x0F0) >> 4] + i, mem[3000]);
                }
                display->stopWrite();

                // display n-byte sprite stored at mem[I] to (Vx, Vy), V[f] = collision


                // make call to display, and VF to return collision value
                break;
              }
            case 0x0E:
              {
                uint8_t op = (cir & 0x0FF);
                switch (op) {
                    case 0x09E:
                        // shift k down by Vx, and get lsb
                        PC += 2 * (((K >> V[(cir & 0xF00) >> 8]) & 0x01) > 0);
                        break;
                    case 0x0A1:
                        PC += 2 * (((K >> V[(cir & 0xF00) >> 8]) & 0x01) == 0);
                        break;
                }
                break;
              }
            case 0x0F:
              {
                uint8_t op = (cir & 0x0FF);
                uint8_t x = (cir & 0xF00) >> 8;
                switch (op) {
                    case 0x07:
                        V[x] = DT;
                        break;
                    case 0x0A:
                      {

                        //std::cout << "listening for key press" << std::endl;

                        bool success = false;
                        uint16_t Kchange;
                        while (!success) {

                            // don't want write between reading K and changing flag
                            //std::cout << "emu try lock[1]" << std::endl;
                            std::unique_lock<std::mutex> kwl(KWriteLock, std::defer_lock);
                            
                            //if (keyReadCount > 38 || keyReadCount == 3 || keyReadCount == 25 || keyReadCount == 37 || keyReadCount == 38) {
                            /*
                            if (true) {
                                std::cout << "keyReadCount: " << keyReadCount << std::endl;
                                std::cout << "It's time" << std::endl;
                            }
                            
                            while (!kwl.try_lock()) {
                                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                                std::cout << "emu[1] can't lock" << std::endl;
                            }
                            */
                            kwl.lock();
                            //KWriteLock.acquire();

                            /*
                            if (keyReadCount > 38 || keyReadCount == 3 || keyReadCount == 25 || keyReadCount == 37 || keyReadCount == 38) {
                                std::cout << "keyReadCount: " << keyReadCount << std::endl;
                                std::cout << "It's time" << std::endl;
                            }
                            */

                            //std::cout << "emu locked[1]" << std::endl;
                            uint16_t oldK = K;
                            Kset = 0x0;
                            //std::cout << "emu unlock[1]" << std::endl;
                            kwl.unlock();
                            //KWriteLock.release();

                            /*
                            if (keyReadCount > 38 || keyReadCount == 3 || keyReadCount == 25 || keyReadCount == 37 || keyReadCount == 38) {
                                std::cout << "keyReadCount: " << keyReadCount << std::endl;
                                std::cout << "It's time" << std::endl;
                            }
                            */

                            // wait until flag is updated
                            while (!Kset);

                            /*
                            if (keyReadCount > 38 || keyReadCount == 3 || keyReadCount == 25 || keyReadCount == 37 || keyReadCount == 38) {
                                std::cout << "keyReadCount: " << keyReadCount << std::endl;
                                std::cout << "It's time" << std::endl;
                            }
                            */

                            //std::cout << "emu try lock[2]" << std::endl;
                            
                            //KWriteLock.acquire();
                            kwl.lock();

                            /*
                            while (!kwl.try_lock()) {
                                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                                std::cout << "emu[2] can't lock" << std::endl;
                            }

                            if (keyReadCount > 38 || keyReadCount == 3 || keyReadCount == 25 || keyReadCount == 37 || keyReadCount == 38) {
                                std::cout << "keyReadCount: " << keyReadCount << std::endl;
                                std::cout << "It's time" << std::endl;
                            }
                            */

                            //std::cout << "emu locked[2]" << std::endl;
                            Kchange = K ^ oldK;
                            success = Kchange & K;

                            // successful if the input has been changed to on
                            Kset = 0x0;
                            //KWriteLock.release();
                            //std::cout << "key read count: " << keyReadCount << std::endl;
                            //std::cout << "emu unlock[2]" << std::endl;
                            
                            /*
                            keyReadCount++;

                            if (keyReadCount > 38 || keyReadCount == 3 || keyReadCount == 25 || keyReadCount == 37 || keyReadCount == 38) {
                                std::cout << "keyReadCount: " << keyReadCount << std::endl;
                                std::cout << "It's time" << std::endl;
                            }
                            */
                            kwl.unlock();
                        }
                        
                        // find the changed key
                        V[x] = 0x00;
                        for (uint8_t i = 0; i < 16; i++) {
                            if (Kchange & 0x01) {
                                V[x] = i;
                                break;
                            }
                            Kchange = Kchange >> 1;
                        }
                        break;
                      }
                    case 0x15:
                        DT = V[x];
                        break;
                    case 0x18:
                        ST = V[x];
                        break;
                    case 0x1E:
                        I += V[x];
                        break;
                    case 0x29:
                        // I = location of sprite for digit V[x]

                        I = 5 * V[x];

                        break;
                    case 0x33:
                        mem[I] = V[x] / 100;
                        mem[I + 1] = (V[x] / 10) % 10;
                        mem[I + 2] =  V[x] % 10;
                        break;
                    case 0x55:
                      {
                        for (uint8_t i = 0; i <= x; i++) {
                            mem[I + i] = V[i];
                        }
                        break;
                      }
                    case 0x65:
                      {
                        for (uint8_t i = 0; i <= x; i++) {
                            V[i] = mem[I + i];
                        }
                        break;
                      }
                }
                break;
              }
        }

        // increment PC counter
        PC += 2; // moved here to avoid obvious conflict with subroutine returns

    }

    void reset() {
        I = 0;
        PC = PCreset;
        SP = -1; // one less than 0 unsigned
        DT = 0;
        ST = 0;

        // check if this works

        // reset registers
        std::fill(std::begin(V),std::end(V),0);

        // reset stack
        std::fill(std::begin(S),std::end(S),0);

        // reset memory
        std::fill(std::begin(mem),std::end(mem),0);

        K = 0;

        /*
        display->startWrite();
        display->writeDisplayRow(0,0,0xFF);
        display->writeDisplayRow(1,0,0xFF);
        display->writeDisplayRow(2,0,0xFF);
        display->writeDisplayRow(3,0,0xFF);
        display->stopWrite();
        */

        /* WRITE PROGRAM HERE*/

        /*
        // copy
        mem[200] = 16 * 6 + 4;
        mem[201] = 120;
        
        // start: get sprite address
        mem[202] = 16 * 15 + 0;
        mem[203] = 16 * 2 + 9;
        
        // draw
        mem[204] = 16 * 13 + 2;
        mem[205] = 16 * 3 + 5;
        
        // increment
        mem[206] = 16 * 7 + 2;
        mem[207] = 5;
        
        // b: move
        mem[208] = 16 * 15 + 1;
        mem[209] = 16 * 0 + 7;
        
        // skip
        mem[210] = 16 * 3 + 1;
        mem[211] = 0;
        
        // jump b
        mem[212] = 16 * 1 + 0;
        mem[213] = 206;
        
        // skip
        mem[214] = 16 * 14 + 5;
        mem[215] = 16 * 9 + 14;
        
        // jump a
        mem[216] = 16 * 1 + 0;
        mem[217] = 220;
        
        // set
        mem[218] = 16 * 6 + 0;
        mem[219] = 1;
        
        // jump end
        mem[220] = 16 * 1 + 0;
        mem[221] = 222;
        
        // a: set
        mem[222] = 16 * 6 + 0;
        mem[223] = 0;
        
        // end: set
        mem[224] = 16 * 15 + 4;
        mem[225] = 16 * 1 + 5;
        
        // jump start
        mem[226] = 16 * 1 + 0;
        mem[227] = 200;

        dumpMemory(200, 28, "./written program.c8");
        */

        /*
        // listen for key
        mem[200] = 16 * 15 + 0;
        mem[201] = 16 * 0 + 10;

        // clear
        mem[202] = 16 * 0 + 0;
        mem[203] = 16 * 14 + 0;

        // get address
        mem[204] = 16 * 15 + 0;
        mem[205] = 16 * 2 + 9;
        
        // draw sprite
        mem[206] = 16 * 13 + 1;
        mem[207] = 16 * 2 + 5;

        // loop
        mem[208] = 16 * 1 + 0;
        mem[209] = 198;
        */
        

        /* Program ends here */

        // rewrite fonts in memory

        // 0
        mem[0] = 0xF0; mem[1] = 0x90; mem[2] = 0x90; mem[3] = 0x90; mem[4] = 0xF0;
        // 1
        mem[5] = 0x20; mem[6] = 0x60; mem[7] = 0x20; mem[8] = 0x20; mem[9] = 0x70;
        mem[10] = 0xF0; mem[11] = 0x10; mem[12] = 0xF0; mem[13] = 0x80; mem[14] = 0xF0;
        mem[15] = 0xF0; mem[16] = 0x10; mem[17] = 0xF0; mem[18] = 0x10; mem[19] = 0xF0;
        mem[20] = 0x90; mem[21] = 0x90; mem[22] = 0xF0; mem[23] = 0x10; mem[24] = 0x10;
        mem[25] = 0xF0; mem[26] = 0x80; mem[27] = 0xF0; mem[28] = 0x10; mem[29] = 0xF0;
        mem[30] = 0xF0; mem[31] = 0x80; mem[32] = 0xF0; mem[33] = 0x90; mem[34] = 0xF0;
        mem[35] = 0xF0; mem[36] = 0x10; mem[37] = 0x20; mem[38] = 0x40; mem[39] = 0x40;
        mem[40] = 0xF0; mem[41] = 0x90; mem[42] = 0xF0; mem[43] = 0x90; mem[44] = 0xF0;
        mem[45] = 0xF0; mem[46] = 0x90; mem[47] = 0xF0; mem[48] = 0x10; mem[49] = 0xF0;
        mem[50] = 0xF0; mem[51] = 0x90; mem[52] = 0xF0; mem[53] = 0x90; mem[54] = 0x90;
        mem[55] = 0xE0; mem[56] = 0x90; mem[57] = 0xE0; mem[58] = 0x90; mem[59] = 0xE0;
        mem[60] = 0xF0; mem[61] = 0x80; mem[62] = 0x80; mem[63] = 0x80; mem[64] = 0xF0;
        mem[65] = 0xE0; mem[66] = 0x90; mem[67] = 0x90; mem[68] = 0x90; mem[69] = 0xE0;
        mem[70] = 0xF0; mem[71] = 0x80; mem[72] = 0xF0; mem[73] = 0x80; mem[74] = 0xF0;
        mem[75] = 0xF0; mem[76] = 0x80; mem[77] = 0xF0; mem[78] = 0x80; mem[79] = 0x80;

        std::cout << "reset complete" << std::endl;
    }

private:

};

#endif
