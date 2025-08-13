#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <format>
#include <array>
#include <bitset>

using u8 = uint8_t;
using u16 = uint16_t;

using i8 = int8_t;
using i16 = int16_t;

class InstructionDecoder {
public:
    InstructionDecoder(const std::string& filename) {
        code = readBinaryFile(filename);
    }

    void decode() {
        std:: cout << "bits 16\n\n";
        while (pc < code.size()) {
            u8 opcode = code[pc];
            auto& table = getOpcodeTable();
            auto handler = table[opcode];
            (this->*handler)(opcode);
        }
    }

private:
    std::vector<u8> code;
    std::size_t pc = 0;

    static std::array<void(InstructionDecoder::*)(u8), 256>& getOpcodeTable() {
        static std::array<void(InstructionDecoder::*)(u8), 256> table = []() {
            std::array<void(InstructionDecoder::*)(u8), 256> t;
            t.fill(&InstructionDecoder::unknownOpcode);

            for (i16 i = 0x88; i <= 0x8B; i++) {
                t[i] = &InstructionDecoder::decodeMovRegRem;
            }

            for (i16 i = 0xA0; i <= 0xA3; i++) {
                t[i] = &InstructionDecoder::decodeMovAccMem;
            }

            for (i16 i = 0xB0; i <= 0xBF; i++) {
                t[i] = &InstructionDecoder::decodeImmToReg;
            }

            for (i16 i = 0xC6; i <= 0xC7; i++) {
                t[i] = &InstructionDecoder::decodeImmToMem;
            }

            return t;
        }();

        return table;
    }

    void unknownOpcode(u8 opcode) {
        std::cout << "[WARNING] Unknown opcode " << std::format("0x{:02x}", int(opcode))
            << " at position " << std::dec << pc << '\n';
        pc++;
    }

    void decodeMovAccMem(u8 opcode) {
        u8 w = opcode & 1;
        u16 addr {};

        std::cout << "mov ";

        if (w) {
            if (pc + 2 > code.size()) return;
            u8 addr_lo = code[pc + 1];
            u8 addr_hi = code[pc + 2];
            addr = (addr_hi << 8) | addr_lo;
            pc += 2;
        } else {
            if (pc + 1 > code.size()) return;
            addr = code[pc + 1];
            pc += 1;
        }

        if ((opcode >> 1) & 1) {
            std::cout << "[" << int(addr) << "], ";
            printRegister(0, w);
        } else {
            printRegister(0, w);
            std::cout << ", [" << int(addr) << "]";
        }

        std::cout << '\n';

        pc += 1;
    }

    void decodeImmToMem(u8 opcode) {
        u8 w = opcode & 1;

        if (pc + 1 > code.size()) return;

        u8 modrm = code[pc + 1];
        u8 mod = (modrm >> 6) & 3;
        u8 reg = (modrm >> 3) & 7;
        u8 rm = modrm & 7;

        std::cout << "mov ";

        printRM(mod, rm, w);

        if (w) {
            if (pc + 3 > code.size()) return;
            u8 data_lo = code[pc + 2];
            u8 data_hi = code[pc + 3];
            u16 data = (data_hi << 8) | data_lo;
            std::cout << ", word " << (int)data;
            pc += 2;
        } else {
            if (pc + 2 > code.size()) return;
            u8 data = code[pc + 2];
            std::cout << ", byte " << (int)data;
            pc += 1;
        }

        std::cout << '\n';

        pc += 2;
    }

    void decodeImmToReg(u8 opcode) {
        u8 w = (opcode >> 3) & 1;
        u8 reg = opcode & 7;

        std::cout << "mov ";

        printRegister(reg, w);

        std::cout << ", ";
        if (w) {
            if (pc + 2 > code.size()) return;
            u8 data_lo = code[pc + 1];
            u8 data_hi = code[pc + 2];
            u16 data = (data_hi << 8) | data_lo;
            std::cout << int(data);
            pc += 2;
        } else {
            if (pc + 1 > code.size()) return;
            u8 data_lo = code[pc + 1];
            std::cout << int(data_lo);
            pc += 1;
        }

        std::cout << '\n';

        pc += 1;
    }

    void decodeMovRegRem(u8 opcode) {
        u8 d = (opcode >> 1) & 1;
        u8 w = opcode & 1;

        if (pc + 1 > code.size()) return;

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
    }

    void printRegister(u8 reg, u8 w) const {
        const char* regs8[] = {"al", "cl", "dl", "bl", "ah", "ch", "dh", "bh"};
        const char* regs16[] = {"ax", "cx", "dx", "bx", "sp", "bp", "si", "di"};

        std::cout << (w ? regs16[reg] : regs8[reg]);
    }

    void printRM(u8 mod, u8 rm, u8 w) {
        if (mod == 3) {
            printRegister(rm, w);
            return;
        }

        const char* addr[] = {"bx + si", "bx + di", "bp + si", "bp + di", "si", "di", "bp", "bx"};

        if (rm == 6 && mod == 0) {
            if (pc + 3 > code.size()) return;
            u8 disp_lo = code[pc + 2];
            u8 disp_hi = code[pc + 3];
            u16 disp = (disp_hi << 8) | disp_lo;
            std::cout << '[' << int(disp) << ']';
            pc += 2;
            return;
        }

        std::cout << "[" << addr[rm];

        if (mod == 1) {
            if (pc + 2 > code.size()) return;
            u8 disp_lo = code[pc + 2];
            i8 disp_signed = static_cast<i8>(disp_lo);

            if (disp_signed > 0) {
                std::cout << " + " << int(disp_signed);
            } else if (disp_signed < 0) {
                std::cout << " - " << int(-disp_signed);
            }

            pc += 1;
        } else if (mod == 2) {
            if (pc + 3 > code.size()) return;
            u8 disp_lo = code[pc + 2];
            u8 disp_hi = code[pc + 3];
            u16 disp = (disp_hi << 8) | disp_lo;
            i16 disp_signed = static_cast<i16>(disp);

            if (disp_signed > 0) {
                std::cout << " + " << int(disp_signed);
            } else {
                std::cout << " - " << int(-disp_signed);
            }

            pc += 2;
        }

        std::cout << "]";
        return;
    }

    static std::vector<u8> readBinaryFile(const std::string& filename) {
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
        return 1;
    }

    InstructionDecoder decoder(argv[1]);
    decoder.decode();

    return 0;
}

