#include <iostream>

#include <thread>
#include <mutex>

#include <fstream>

#include <filesystem>

#include <vector>
#include <unordered_map>

#include <string>
#include <stdint.h>

#include "include/Assembler.h"
#include "include/InstructionParser.h"

int main(int argc, char *argv[]) {
    
    std::cout << "hello world" << std::endl;

    if (argc > 1) {
        // load memory from file location given in argv[]
        if (argc > 2) {
            std::cout << "USAGE: ./chip-8-assembler [source_file_path]" << std::endl;
            return 1;
        }

        std::string path(argv[1]);

        Assembler a;
        a.setProgramStart(200);
        a.setSpriteBlockDirection(Assembler::FORWARDS);
        a.setSpriteBlockStart(3000);
        //a.setOutputOffset(200);
        a.assemble(path);

        //a.printLines();
        //a.printTokens();

        //std::vector<std::string> instructions;
        //std::unordered_map<std::string, uint16_t> labels = {};
        //std::unordered_map<std::string, std::string> aliases = {};
        //std::unordered_map<std::string, std::string> sprites = {};

        //instructions.push_back("LD V5 6");
        //instructions.push_back("DRW V3 V4 5");
        //instructions.push_back("JP myLabel");

        //labels["myLabel"] = 198;


        //InstructionParser iP;
        //iP.setProgramStart(200);
        //iP.parse(instructions, labels, aliases, sprites);

        //std::cout << "aaaa" << std::endl;

        uint8_t program[1400000]; // this will contain the video information too
        std::fill(std::begin(program),std::end(program),0);
        a.copyProgram(program);
        
        // do not print out 1.4MB please and thank you
        //std::cout << program << std::endl;

        std::ifstream is;
        is.open("output-diff.bin");

        if (!is.is_open()) {
            std::cout << "source file could not be opened" << std::endl;
            return -1;
        }

        // copy video information to program starting from 4000
        is.read((char*)(program + 4000), 1400000 - 4000);

        is.close();

        std::ofstream os("bad_apple_delta.ch8");
        //os.open(path);
        if (os.is_open()) {
            // file has been opened, so write memory
            const char* s = (char*)program + 200;

            os.write(s, 1400000 - 200);
            
            os.close();

        } else {
            std::cout << "could not output to file" << std::endl;
        }

    }
    return 0;
}
