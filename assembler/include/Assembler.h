#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include <string>
#include <regex>
#include <vector>
#include <stdexcept>
#include <fstream>
#include <algorithm>
#include <unordered_map>
#include <sstream>

#include <stdint.h>
#include <cstdlib>

#include "./InstructionParser.h"

enum LineToken {
    ALIAS,
    LABEL,
    SPRITE,
    COMMENT,
    INSTRUCTION,
    INVALID
};

class Assembler {
private:

    // regex for pattern recognition
    std::vector<std::regex> tokenExpressions;
    //std::vector<std::regex> instructionExpressions;

    std::vector<std::string> instructions;
    std::vector<std::string> aliasLines;
    std::vector<std::string> spriteLines;

    std::unordered_map<std::string, std::string> aliases;
    std::unordered_map<std::string, uint16_t> sprites;
    //std::vector<std::string> labelLines;

    std::vector<LineToken> tokens;

    std::unordered_map<std::string, uint16_t> labels;

    uint8_t program[4096];

    uint16_t programStart;

    uint16_t spriteBlockStart;

    LineToken getToken(std::string line) {

        // keywords
        /*
        std::regex aliasExp("^(#alias ([0-9A-Za-z]+) ([0-9A-Za-z]+))$");
        std::regex labelExp("^[A-Za-z][0-9A-Za-z]*:$");
        std::regex spriteExp("^#sprite [A-Za-z][0-9A-Za-z]*( (0x[\dA-Fa-f]+|0b[01]+|\d+))+$");
        std::regex startExp("^#start$");
        std::regex endExp("^#end$");
        std::regex commentExp("^;.*$");
        std::regex instructionExp("^[A-Z]+( .*)*$");
        */

        int matchIndex;
        for (matchIndex = 0; matchIndex < tokenExpressions.size(); matchIndex++) {
            if (std::regex_search(line, tokenExpressions[matchIndex])) {
                LineToken ret = static_cast<LineToken>(matchIndex);
                
                return static_cast<LineToken>(matchIndex);
            }
        }
        // return last token type (INVALID)
        
        return static_cast<LineToken>(matchIndex);
    }

public:

    enum Direction {
        FORWARDS,
        BACKWARDS
    };

private:
    Direction spriteBlockDirection;

public:

    void setProgramStart(int ps) {
        programStart = ps;
    }

    void setSpriteBlockStart(int sbs) {
        spriteBlockStart = sbs;
    }

    void setSpriteBlockDirection(Direction sbd) {
        spriteBlockDirection = sbd;
    }

    Assembler() {

        programStart = 200;
        spriteBlockStart = 4000;

        std::fill(std::begin(program),std::end(program),0);

        // generate regex expressions for line recognition

        tokenExpressions.push_back(std::regex("^#alias\\s+[A-Za-z_][0-9A-Za-z_]*\\s+[0-9A-Za-z_]*$"));
        tokenExpressions.push_back(std::regex("^[A-Za-z_][0-9A-Za-z_]*:$"));
        tokenExpressions.push_back(std::regex("^#sprite\\s+[A-Za-z_][0-9A-Za-z_]*(\\s+(0x[\\dA-Fa-f]+|0b[01]+|\\d+))+$"));
        //tokenExpressions.push_back(std::regex("^#start$"));
        //tokenExpressions.push_back(std::regex("^#end$"));
        tokenExpressions.push_back(std::regex("^;.*$"));
        tokenExpressions.push_back(std::regex("^[A-Z]+(\\s+.*)*$"));

        /*
        instructionExpressions.push_back(std::regex("^[Vv][0-9A-Fa-f]$"));
        instructionExpressions.push_back(std::regex("^0x[\\dA-Fa-f]+$"));
        instructionExpressions.push_back(std::regex("^0b[01]+$"));
        instructionExpressions.push_back(std::regex("^\\d+$"));
        */
    }

    void tokenise(std::ifstream &is) {

        int instructionCount = 0;
        std::regex whitespaceExp("^\\s*|\\s*$");
        std::string line;
        while (std::getline(is, line)) {

            std::string trimmedLine = std::regex_replace(line, whitespaceExp, "");

            if (trimmedLine == "") continue;

            LineToken lT = getToken(trimmedLine);
            switch (lT) {
                case INVALID:
                    std::cout << "parse error at: \"" << line << "\"" << std::endl;
                    break;
                case ALIAS:

                    aliasLines.push_back(trimmedLine);
                    break;
                case LABEL:
                    //labels.push_back(trimmedLine);
                    labels[trimmedLine.substr(0, trimmedLine.size() - 1)] = programStart + (instructionCount * 2) - 2;
                    break;
                case SPRITE:
                    spriteLines.push_back(trimmedLine);
                    break;
                case COMMENT:
                    // don't need to do anything
                    break;
                case INSTRUCTION:
                    instructions.push_back(trimmedLine);
                    instructionCount++;
                    break;
                default:
                    break;
            }
        }

    }

    /*
    void processInstruction(int index) {
        
        // check valid
        // check for aliases
        // add label addresses
        // add sprite addresses to the one instruction
        int index = 0;
        for (int i = 0; i < tokens.size(); i++) {
            if (tokens.at(i) == INSTRUCTION) {
                // process the instruction

                std::istringstream insStream();

                // split token by whitespace
                while (getline(insStream, tok, ';')) {
                    for (matchIndex = 0; matchIndex < instructionExpressions.size(); matchIndex++) {
                        if (std::regex_search(tok, instructionExpressions[matchIndex])) {
                            char** result;
                            switch (matchIndex) {
                                case 0:

                                    break;
                                case 1:
                                    break;
                                case 2:
                                    break;
                                case 3:
                                    strol(tok, result, 10);
                                    break;
                            }

                            delete *result;
                        }
                    }
                    std::cout << "invalid argument for instruction" << std::endl;
                }

                index++;
            }
        }

    }
    */

    /*
    void findLabels() {
        int index = 0;
        for (int i = 0; i < tokens.size(); i++) {
            if (tokens.at(i) == LABEL) {
                std::string l = lines.at(i);
                labelPositions[l.substr(0,l.size() - 1)] = index * 2 - 2;
            } else if (tokens.at(i) == INSTRUCTION) {
                index++;
            }
        }
    }
    */


    /*
    void aliasInsertion() {
        // go through each instruction between start and end

        std::vector<LineToken>::const_iterator startIT = std::find(tokens.begin(), tokens.end(), START);
        if (startIT == tokens.end()) {
            std::cout << "no start keyword" << std::endl;
            return;
        }
        std::vector<LineToken>::const_iterator endIT = std::find(tokens.begin(), tokens.end(), END);
        if (endIT == tokens.end()) {
            std::cout << "no end keyword" << std::endl;
            return;
        }

        int startIndex = startIT - tokens.begin();
        int endIndex = endIT - tokens.begin();

        for (int i = startIndex; i < length; i++) {
            processInstruction();
        }

    }
    */

    int assemble(std::string path) {

        std::fill(std::begin(program), std::end(program), 0);

        std::ifstream is;
        is.open(path);

        if (!is.is_open()) {
            std::cout << "source file could not be opened" << std::endl;
            return -1;
        }

        // sort lines into collections
        tokenise(is);

        is.close();

        // convert sprite lines to sprites and addresses
        processAliases();
        processSprites();

        // add assembler aliases
        aliases["#program_start"] = std::to_string(programStart - 2);
        aliases["#program_end"] = std::to_string(programStart + 2 * instructions.size());
        aliases["#sprite_length"] = std::string("5");

        // convert instructions to numbers
        InstructionParser iP;
        iP.setProgramStart(programStart);
        iP.parse(instructions, labels, aliases, sprites);
        
        // copy instructions to memory
        iP.copyProgram(program);
    }

    void processAliases() {
        for (std::string aliasString : aliasLines) {

            // get each full word, skippping whitespace
            std::regex delimExp("[^\\s]+");
            std::regex_token_iterator<std::string::iterator> end;
            std::regex_token_iterator<std::string::iterator> insStrPtr(aliasString.begin(), aliasString.end(), delimExp);

            // skip first token (#alias)
            insStrPtr++;

            // save sprite name
            std::string key = *insStrPtr++;

            aliases[key] = *insStrPtr++;
        }
    }

    void processSprites() {
        
        // put sprites at the end of the program
        int spriteOffset = 0;
        // lines of sprites in array
        for (std::string spriteString : spriteLines) {

            // get each full word, skippping whitespace
            std::regex delimExp("[^\\s]+");
            std::regex_token_iterator<std::string::iterator> end;
            std::regex_token_iterator<std::string::iterator> insStrPtr(spriteString.begin(), spriteString.end(), delimExp);

            // skip first token (#sprite)
            insStrPtr++;

            // save sprite name
            std::string spriteIdentifier = *insStrPtr++;

            std::vector<uint8_t> spriteValues;

            while (insStrPtr != end) {
                
                // this has already been matched as a literal, so just convert
                spriteValues.push_back(parseNumber(*insStrPtr++));
            }
            
            if (spriteBlockDirection == BACKWARDS)
                spriteOffset += spriteValues.size();

            // add to memory at location
            for (int i = 0; i < spriteValues.size(); i++) {
                program[spriteBlockStart + spriteOffset * ((spriteBlockDirection == FORWARDS) ? 1 : -1) + i] = spriteValues.at(i);
            }

            aliases["#" + spriteIdentifier + "_address"] = std::to_string(spriteBlockStart + spriteOffset * ((spriteBlockDirection == FORWARDS) ? 1 : -1));
            aliases["#" + spriteIdentifier + "_length"] = std::to_string(spriteValues.size());

            if (spriteBlockDirection == FORWARDS)
                spriteOffset += spriteValues.size();
        }
    }

    // should be moved to own file
    uint16_t parseNumber(std::string n) {

        // given a string of hex, decimal or binary number of the form 0xFF, 123
        // or 0b10101 respectively, return the decimal value
        if (n.substr(0,2) == "0x") {
            uint16_t ret = 0;
            std::string hexString = n.substr(2,n.size() - 2);
            for (int i = 0; i < hexString.size(); i++){
                //const char c = *it++;
                ret <<= 4;
                
                uint8_t v = (uint8_t)(std::toupper(hexString[i]) - '0');
                ret += (v < 10) ? v : v - 7;
            }
            return ret;
        } else if (n.substr(0,2) == "0b") {
            uint16_t ret = 0;
            std::string binString = n.substr(2,n.size() - 2);
            for (int i = 0; i < binString.size(); i++) {
                //const char c = *it++;
                ret <<= 1;
                ret += atoi(&(binString[i]));
            }
            return ret;
        }
        uint16_t ret = std::stoi(n);
        return ret;
        
    }

    /*
    void printTokens() {
        for (LineToken &lT : tokens) {
            std::cout << lT << std::endl;
        }
        std::cout << std::endl;
    }

    void printLines() {
        for (std::string &s : lines) {
            std::cout << s << std::endl;
        }
        std::cout << std::endl;
    }
    */

    void copyProgram(uint8_t prog[]) {
        std::copy(std::begin(program), std::end(program), prog);
    }

private:

};

#endif