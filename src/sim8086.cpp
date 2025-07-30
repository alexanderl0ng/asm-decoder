#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <bitset>

using u8 = uint8_t;

class InstructionDecoder {
    std::vector<u8> code;
    std::size_t pc = 0;
public:
    InstructionDecoder(const std::string& filename) {
        code = readBinaryFile(filename);
    }

    void decode() {

        std:: cout << "bits 16\n\n";
        while (pc < code.size()) {
            u8 opcode = code[pc];

            if ((opcode & 0xFC) == 0x88) {
                decodeMov(opcode);
            } else {
                std::cout << "[WARNING] Not implemented or unknown opcode: " << std::hex << (int)opcode << '\n';
                pc++;
            }
        }
    }

private:
    void decodeMov(u8 opcode) {
        u8 d = (opcode >> 1) & 1;
        u8 w = opcode & 1;

        if (pc + 1 >= code.size()) return;

        u8 modrm = code[pc + 1];
        u8 mod = (modrm >> 6) & 3;
        u8 reg = (modrm >> 3) & 7;
        u8 rm = modrm & 7;

        std::cout << "mov ";

        if (d) {
            printRegister(reg, w);
            std::cout << ", ";
            printRM(mod, rm, w);
        } else {
            printRM(mod, rm, w);
            std::cout << ", ";
            printRegister(reg, w);
        }

        std::cout << '\n';

        pc += 2;

        if (mod == 1) pc++;
        else if (mod == 2) pc += 2;
    }

    void printRegister(u8 reg, u8 w) {
        const char* regs8[] = {"al", "cl", "dl", "bl", "ah", "ch", "dh", "bh"};
        const char* regs16[] = {"ax", "cx", "dx", "bx", "sp", "bp", "si", "di"};

        std::cout << (w ? regs16[reg] : regs8[reg]);
    }

    void printRM(u8 mod, u8 rm, u8 w) {
        if (mod == 3) {
            printRegister(rm, w);
        } else {
            std::cout << "[memory]";
        }
    }

    std::vector<u8> readBinaryFile(const std::string& filename) {
        std::ifstream file(filename, std::ios::binary | std::ios::in);
        if (!file) {
            throw std::runtime_error("[ERROR] Cannot open file: " + filename);
        }

        file.seekg(0, std::ios::end);
        std::size_t file_size = file.tellg();
        file.seekg(0, std::ios::beg);

        std::vector<u8> buffer(file_size);
        file.read(reinterpret_cast<char*>(buffer.data()), file_size);

        return buffer;
    }
};


int main(int argc, char* argv[]) {

    if (argc != 2) {
        std::cout << "[ERROR] Usage: " << argv[0] << " <filepath>" << std::endl;
    }

    InstructionDecoder decoder(argv[1]);
    decoder.decode();

    return 0;
}

