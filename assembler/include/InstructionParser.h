#ifndef INSTRUCTIONPARSER_H
#define INSTRUCTIONPARSER_H

#include <unordered_map>
#include <unordered_set>
#include <string>
#include <regex>

#include <sstream>
#include <iostream>
#include <iterator>

enum InstructionToken {
    V_REGISTER,
    LITERAL,
    I_REGISTER,
    DELAY_TIMER,
    SOUND_TIMER,
    K_REGISTER,
    F,
    B,
    NO_INSTRUCTION

};

enum OperationToken {
    //SYS,
    CLS,
    RET,
    JP,
    CALL,
    SE,
    SNE,
    LD,
    OR,
    AND,
    XOR,
    ADD,
    SUB,
    SHR,
    SUBN,
    SHL,
    RND,
    DRW,
    SKP,
    SKNP,
    NO_OPERATION
};

class InstructionParser {

private:

    std::vector<std::string> instructions;
    std::vector<std::vector<std::string>> instructionStrings;
    std::vector<std::vector<InstructionToken>> instructionTokens;

    std::vector<std::regex> instructionExpressions;

    std::unordered_map<std::string, OperationToken> stringOpMap = {};

    std::unordered_map<OperationToken, std::tuple<InstructionToken, InstructionToken, InstructionToken>> validInstructions = {};

    uint8_t program[4096];

    int programStart;

    int instructionTotal;

public:

    void setProgramStart(int ps) {
        programStart = ps;
    }

    InstructionParser() {

        programStart = 200;

        std::fill(std::begin(program),std::end(program),0);

        // V_REGISTERS
        instructionExpressions.push_back(std::regex("^[Vv][0-9A-Fa-f]$"));


        // LITERALS
        instructionExpressions.push_back(std::regex("^0x[\\dA-Fa-f]+$|^0b[01]+$|^\\d+$"));
        //instructionExpressions.push_back(std::regex("^0b[01]+$"));
        //instructionExpressions.push_back(std::regex("^\\d+$"));

        // special ones
        instructionExpressions.push_back(std::regex("^I$"));
        instructionExpressions.push_back(std::regex("^DT$"));
        instructionExpressions.push_back(std::regex("^ST$"));
        instructionExpressions.push_back(std::regex("^K$"));
        instructionExpressions.push_back(std::regex("^F$"));
        instructionExpressions.push_back(std::regex("^B$"));

        //stringOpMap["SYS"] = SYS;
        stringOpMap["CLS"] = CLS;
        stringOpMap["RET"] = RET;
        stringOpMap["JP"] = JP;
        stringOpMap["CALL"] = CALL;
        stringOpMap["SE"] = SE;
        stringOpMap["SNE"] = SNE;
        stringOpMap["LD"] = LD;
        stringOpMap["OR"] = OR;
        stringOpMap["AND"] = AND;
        stringOpMap["XOR"] = XOR;
        stringOpMap["ADD"] = ADD;
        stringOpMap["SUB"] = SUB;
        stringOpMap["SHR"] = SHR;
        stringOpMap["SUBN"] = SUBN;
        stringOpMap["SHL"] = SHL;
        stringOpMap["SNE"] = SNE;
        stringOpMap["RND"] = RND;
        stringOpMap["DRW"] = DRW;
        stringOpMap["SKP"] = SKP;
        stringOpMap["SKNP"] = SKNP;

        /*

        validInstructions[CLS] = std::make_tuple(NO_INSTRUCTION, NO_INSTRUCTION, NO_INSTRUCTION);
        validInstructions[RET] = std::make_tuple(NO_INSTRUCTION, NO_INSTRUCTION, NO_INSTRUCTION);

        validInstructions[JP].insert(std::make_tuple(JP, LITERAL, NO_INSTRUCTION, NO_INSTRUCTION));

        validInstructions[CALL]insert(std::make_tuple(CALL, LITERAL, NO_INSTRUCTION, NO_INSTRUCTION));

        validInstructions[SE]insert(std::make_tuple(SE, V_REGISTER, LITERAL, NO_INSTRUCTION));

        validInstructions[SNE]insert(std::make_tuple(SNE, V_REGISTER, LITERAL, NO_INSTRUCTION));

        validInstructions[SE]insert(std::make_tuple(SE, V_REGISTER, V_REGISTER, NO_INSTRUCTION));

        validInstructions.insert(std::make_tuple(LD, V_REGISTER, LITERAL, NO_INSTRUCTION));

        validInstructions.insert(std::make_tuple(ADD, V_REGISTER, LITERAL, NO_INSTRUCTION));

        validInstructions.insert(std::make_tuple(LD, V_REGISTER, V_REGISTER, NO_INSTRUCTION));
        validInstructions.insert(std::make_tuple(OR, V_REGISTER, V_REGISTER, NO_INSTRUCTION));
        validInstructions.insert(std::make_tuple(AND, V_REGISTER, V_REGISTER, NO_INSTRUCTION));
        validInstructions.insert(std::make_tuple(XOR, V_REGISTER, V_REGISTER, NO_INSTRUCTION));
        validInstructions.insert(std::make_tuple(ADD, V_REGISTER, V_REGISTER, NO_INSTRUCTION));
        validInstructions.insert(std::make_tuple(SUB, V_REGISTER, V_REGISTER, NO_INSTRUCTION));
        validInstructions.insert(std::make_tuple(SHR, V_REGISTER, V_REGISTER, NO_INSTRUCTION));
        validInstructions.insert(std::make_tuple(SUBN, V_REGISTER, V_REGISTER, NO_INSTRUCTION));
        validInstructions.insert(std::make_tuple(SHL, V_REGISTER, V_REGISTER, NO_INSTRUCTION));
        validInstructions.insert(std::make_tuple(SNE, V_REGISTER, V_REGISTER, NO_INSTRUCTION));

        validInstructions.insert(std::make_tuple(LD, I_REGISTER, LITERAL, NO_INSTRUCTION));

        validInstructions.insert(std::make_tuple(JP, V_REGISTER, LITERAL, NO_INSTRUCTION));

        validInstructions.insert(std::make_tuple(RND, V_REGISTER, LITERAL, NO_INSTRUCTION));

        validInstructions.insert(std::make_tuple(DRW, V_REGISTER, V_REGISTER, LITERAL));
        validInstructions.insert(std::make_tuple(SKP, V_REGISTER, NO_INSTRUCTION, NO_INSTRUCTION));
        validInstructions.insert(std::make_tuple(SKNP, V_REGISTER, NO_INSTRUCTION, NO_INSTRUCTION));
        validInstructions.insert(std::make_tuple(LD, V_REGISTER, DELAY_TIMER, NO_INSTRUCTION));
        validInstructions.insert(std::make_tuple(LD, V_REGISTER, K_REGISTER, NO_INSTRUCTION));
        validInstructions.insert(std::make_tuple(LD, DELAY_TIMER, V_REGISTER, NO_INSTRUCTION));
        validInstructions.insert(std::make_tuple(LD, SOUND_TIMER, V_REGISTER, NO_INSTRUCTION));
        validInstructions.insert(std::make_tuple(ADD, I_REGISTER, V_REGISTER, NO_INSTRUCTION));
        validInstructions.insert(std::make_tuple(LD, F, V_REGISTER, NO_INSTRUCTION));
        validInstructions.insert(std::make_tuple(LD, B, V_REGISTER, NO_INSTRUCTION));
        validInstructions.insert(std::make_tuple(LD, I_REGISTER, V_REGISTER, NO_INSTRUCTION));
        validInstructions.insert(std::make_tuple(LD, V_REGISTER, I_REGISTER, NO_INSTRUCTION));
        */

    }

    // given aliases, sprites, and instructions produce machine code

    void parse(std::vector<std::string> &instructions, std::unordered_map<std::string, uint16_t> &labels, std::unordered_map<std::string, std::string> &aliases, std::unordered_map<std::string, uint16_t> &sprites) {

        int instructionCount = 0;
        for (std::string insStr : instructions) {
            std::cout << "now parsing: \"" << insStr << "\"" << std::endl;
            std::regex delimExp("[^\\s]+");
            std::regex_token_iterator<std::string::iterator> end;
            std::regex_token_iterator<std::string::iterator> insStrPtr(insStr.begin(), insStr.end(), delimExp);

            // first, map instruction
            InstructionToken insTok[3] {NO_INSTRUCTION, NO_INSTRUCTION, NO_INSTRUCTION};

            OperationToken opTok = getOperationType(*insStrPtr++);

            instructionStrings.push_back(std::vector<std::string>());
            instructionTokens.push_back(std::vector<InstructionToken>());
            //instructionStrings[instructionCount].push_back(*insStrPtr++);

            int expressionCount = 0;
            while (insStrPtr != end) {

                std::string expStr = *insStrPtr++;

                InstructionToken currentType = getExpressionType(expStr);

                // making this a while could cause cycles, but I think it's worth it
                while (currentType == NO_INSTRUCTION) {
                    // look up aliases, sprites, labels
                    if (aliases.find(expStr) != aliases.end()) {
                        expStr = aliases[expStr];
                    } else if (sprites.find(expStr) != sprites.end()) {
                        expStr = sprites[expStr];
                    } else if (labels.find(expStr) != labels.end()) {
                        expStr = std::to_string(labels[expStr]);
                    } else {
                        return;
                    }
                    currentType = getExpressionType(expStr);
                }
                insTok[expressionCount] = currentType;
                instructionStrings.at(instructionCount).push_back(expStr);
                expressionCount++;
            }

            std::tuple<OperationToken, InstructionToken,InstructionToken,InstructionToken> instruction = std::make_tuple(
                opTok,
                insTok[0],
                insTok[1],
                insTok[2]
            );

            //auto validInstructionSearch = validInstructions.find(instruction);

            //if (validInstructionSearch != validInstructions.end()) {
            if (true) {

                // convert instructions to machine code

                uint16_t instructionValue;

                switch (opTok) {
                    case CLS:
                        instructionValue = 0x00E0;
                        break;
                    case RET:
                        instructionValue = 0x00EE;
                        break;
                    case JP:
                        switch (insTok[0]) {
                            case V_REGISTER:
                                if (getVRegisterNumber(instructionStrings.at(instructionCount).at(0)) == 0) {
                                    instructionValue = 0xB000;
                                    instructionValue += parseNumber(instructionStrings.at(instructionCount).at(1));
                                }
                                break;
                            case LITERAL:
                                instructionValue = 0x1000;
                                instructionValue += parseNumber(instructionStrings.at(instructionCount).at(0));
                                break;
                        }
                        break;
                    case CALL:
                        instructionValue = 0x2000;
                        instructionValue += parseNumber(instructionStrings.at(instructionCount).at(0));
                        break;
                    case SE:
                        switch (insTok[1]) {
                            case V_REGISTER:
                                instructionValue = 0x5000;
                                instructionValue += getVRegisterNumber(instructionStrings.at(instructionCount).at(0)) << 8;
                                instructionValue += getVRegisterNumber(instructionStrings.at(instructionCount).at(1)) << 4;
                                break;
                            case LITERAL:
                                instructionValue = 0x3000;
                                instructionValue += getVRegisterNumber(instructionStrings.at(instructionCount).at(0)) << 8;
                                instructionValue += parseNumber(instructionStrings.at(instructionCount).at(1));
                                break;
                        }
                        break;
                    case SNE:
                        switch (insTok[1]) {
                            case V_REGISTER:
                                instructionValue = 0x9000;
                                instructionValue += getVRegisterNumber(instructionStrings.at(instructionCount).at(0)) << 8;
                                instructionValue += getVRegisterNumber(instructionStrings.at(instructionCount).at(1)) << 4;
                                break;
                            case LITERAL:
                                instructionValue = 0x4000;
                                instructionValue += getVRegisterNumber(instructionStrings.at(instructionCount).at(0)) << 8;
                                instructionValue += parseNumber(instructionStrings.at(instructionCount).at(1));
                                break;
                        }
                        break;
                    case LD:
                        switch (insTok[0]) {
                            case I_REGISTER:
                                switch (insTok[1]) {
                                    case LITERAL:
                                        instructionValue = 0xA000;
                                        instructionValue += parseNumber(instructionStrings.at(instructionCount).at(1));
                                        break;
                                    case V_REGISTER:
                                        instructionValue = 0xF055;
                                        instructionValue += getVRegisterNumber(instructionStrings.at(instructionCount).at(1)) << 8;
                                        break;
                                }
                                break;
                            case DELAY_TIMER:
                                instructionValue = 0xF015;
                                instructionValue += getVRegisterNumber(instructionStrings.at(instructionCount).at(1)) << 8;
                                break;
                            case SOUND_TIMER:
                                instructionValue = 0xF018;
                                instructionValue += getVRegisterNumber(instructionStrings.at(instructionCount).at(1)) << 8;
                                break;
                            case F:
                                instructionValue = 0xF029;
                                instructionValue += getVRegisterNumber(instructionStrings.at(instructionCount).at(1)) << 8;
                                break;
                            case B:
                                instructionValue = 0xF033;
                                instructionValue += getVRegisterNumber(instructionStrings.at(instructionCount).at(1)) << 8;
                                break;
                            case V_REGISTER:
                                switch (insTok[1]) {
                                    
                                    case V_REGISTER:
                                        instructionValue = 0x8000;
                                        instructionValue += getVRegisterNumber(instructionStrings.at(instructionCount).at(0)) << 8;
                                        instructionValue += getVRegisterNumber(instructionStrings.at(instructionCount).at(1)) << 4;
                                        break;
                                    case LITERAL:
                                        instructionValue = 0x6000;
                                        instructionValue += getVRegisterNumber(instructionStrings.at(instructionCount).at(0)) << 8;
                                        instructionValue += parseNumber(instructionStrings.at(instructionCount).at(1));
                                        break;
                                    case DELAY_TIMER:
                                        instructionValue = 0xF007;
                                        instructionValue += getVRegisterNumber(instructionStrings.at(instructionCount).at(0)) << 8;
                                        break;
                                    case K_REGISTER:
                                        instructionValue = 0xF00A;
                                        instructionValue += getVRegisterNumber(instructionStrings.at(instructionCount).at(0)) << 8;
                                        break;
                                    case I_REGISTER:
                                        instructionValue = 0xF065;
                                        instructionValue += getVRegisterNumber(instructionStrings.at(instructionCount).at(0)) << 8;
                                        break;
                                }
                                break;
                        }
                        break;
                    case ADD:
                        switch (insTok[0]) {
                            case V_REGISTER:
                                switch (insTok[1]) {
                                    case V_REGISTER:
                                        instructionValue = 0x8004;
                                        instructionValue += getVRegisterNumber(instructionStrings.at(instructionCount).at(0)) << 8;
                                        instructionValue += getVRegisterNumber(instructionStrings.at(instructionCount).at(1)) << 4;
                                        break;
                                    case LITERAL:
                                        instructionValue = 0x7000;
                                        instructionValue += getVRegisterNumber(instructionStrings.at(instructionCount).at(0)) << 8;
                                        instructionValue += parseNumber(instructionStrings.at(instructionCount).at(1));
                                        break;
                                }
                                break;
                            case I_REGISTER:
                                instructionValue = 0xF01E;
                                instructionValue += getVRegisterNumber(instructionStrings.at(instructionCount).at(1)) << 8;
                                break;
                            }
                        break;
                    case OR:
                        instructionValue = 0x8001;
                        instructionValue += getVRegisterNumber(instructionStrings.at(instructionCount).at(0)) << 8;
                        instructionValue += getVRegisterNumber(instructionStrings.at(instructionCount).at(1)) << 4;
                        break;
                    case AND:
                        instructionValue = 0x8002;
                        instructionValue += getVRegisterNumber(instructionStrings.at(instructionCount).at(0)) << 8;
                        instructionValue += getVRegisterNumber(instructionStrings.at(instructionCount).at(1)) << 4;
                        break;
                    case XOR:
                        instructionValue = 0x8003;
                        instructionValue += getVRegisterNumber(instructionStrings.at(instructionCount).at(0)) << 8;
                        instructionValue += getVRegisterNumber(instructionStrings.at(instructionCount).at(1)) << 4;
                        break;
                    case SUB:
                        instructionValue = 0x8005;
                        instructionValue += getVRegisterNumber(instructionStrings.at(instructionCount).at(0)) << 8;
                        instructionValue += getVRegisterNumber(instructionStrings.at(instructionCount).at(1)) << 4;
                        break;
                    case SHR:
                        instructionValue = 0x8006;
                        instructionValue += getVRegisterNumber(instructionStrings.at(instructionCount).at(0)) << 8;
                        instructionValue += getVRegisterNumber(instructionStrings.at(instructionCount).at(1)) << 4;
                        break;
                    case SUBN:
                        instructionValue = 0x8007;
                        instructionValue += getVRegisterNumber(instructionStrings.at(instructionCount).at(0)) << 8;
                        instructionValue += getVRegisterNumber(instructionStrings.at(instructionCount).at(1)) << 4;
                        break;
                    case SHL:
                        instructionValue = 0x800E;
                        instructionValue += getVRegisterNumber(instructionStrings.at(instructionCount).at(0)) << 8;
                        instructionValue += getVRegisterNumber(instructionStrings.at(instructionCount).at(1)) << 4;
                        break;
                    case RND:
                        instructionValue = 0xC000;
                        instructionValue += getVRegisterNumber(instructionStrings.at(instructionCount).at(0)) << 8;
                        instructionValue += parseNumber(instructionStrings.at(instructionCount).at(1));
                        break;
                    case DRW:
                        instructionValue = 0xD000;
                        instructionValue += getVRegisterNumber(instructionStrings.at(instructionCount).at(0)) << 8;
                        instructionValue += getVRegisterNumber(instructionStrings.at(instructionCount).at(1)) << 4;
                        instructionValue += parseNumber(instructionStrings.at(instructionCount).at(2));
                        break;
                    case SKP:
                        instructionValue = 0xE09E;
                        instructionValue += getVRegisterNumber(instructionStrings.at(instructionCount).at(0)) << 8;
                        break;
                    case SKNP:
                        instructionValue = 0xE0A1;
                        instructionValue += getVRegisterNumber(instructionStrings.at(instructionCount).at(0)) << 8;
                        break;
                }

                program[programStart + 2 * instructionCount] = (instructionValue & 0xFF00) >> 8;
                program[programStart + 2 * instructionCount + 1] = (instructionValue & 0xFF);
                instructionCount++;
            }
        }

        instructionTotal = instructionCount;

    }

    uint16_t parseNumber(std::string n) {

        // given a string of hex, decimal or binary number of the form 0xFF, 123
        // or 0b10101 respectively, return the decimal value
        if (n.substr(0,2) == "0x") {
            uint16_t ret = 0;
            std::string hexString = n.substr(2,n.size() - 2);
            for (int i = 0; i < hexString.size(); i++){
                //const char c = *it++;
                ret <<= 4;
                //ret += atoi(&(hexString[i]));
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

    InstructionToken getExpressionType(std::string ins) {
        for (int matchIndex = 0; matchIndex < instructionExpressions.size(); matchIndex++) {
            if (std::regex_search(ins, instructionExpressions[matchIndex])) {
                return static_cast<InstructionToken>(matchIndex);
            }
        }
        return NO_INSTRUCTION;
    }

    OperationToken getOperationType(std::string op) {

        if (stringOpMap.find(op) != stringOpMap.end()) {
            return stringOpMap[op];
        }
        return NO_OPERATION;
        
    }

    uint8_t getVRegisterNumber(std::string Vx) {
        uint8_t v = (uint8_t)(std::toupper(Vx.at(1)) - '0');
        return (v < 10) ? v : v - 7;
    }

    void copyProgram(uint8_t prog[]) {
        std::copy(std::begin(program), std::begin(program) + programStart + instructionTotal * 2, prog);
    }

private:

};

#endif