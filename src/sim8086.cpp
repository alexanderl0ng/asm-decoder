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

            for (i16 i = 0x00; i <= 0x03; i++) {
                t[i] = &InstructionDecoder::decodeAddRegRem;
            }

            for (i16 i = 0x04; i <= 0x05; i++) {
                t[i] = &InstructionDecoder::decodeAddAccMem;
            }

            for (i16 i = 0x28; i <= 0x2B; i++) {
                t[i] = &InstructionDecoder::decodeSubRegRem;
            }

            for (i16 i = 0x2C; i <= 0x2D; i++) {
                t[i] = &InstructionDecoder::decodeSubAccMem;
            }

            for (i16 i = 0x38; i <= 0x3B; i++) {
                t[i] = &InstructionDecoder::decodeCmpRegRem;
            }

            for (i16 i = 0x3C; i <= 0x3D; i++) {
                t[i] = &InstructionDecoder::decodeCmpAccMem;
            }

            for (i16 i = 0x80; i <= 0x83; i++) {
                t[i] = &InstructionDecoder::decodeImmRegMem;
            }

            for (i16 i = 0x88; i <= 0x8B; i++) {
                t[i] = &InstructionDecoder::decodeMovRegRem;
            }

            for (i16 i = 0xA0; i <= 0xA3; i++) {
                t[i] = &InstructionDecoder::decodeMovAccMem;
            }

            for (i16 i = 0xB0; i <= 0xBF; i++) {
                t[i] = &InstructionDecoder::decodeMovImmToReg;
            }

            for (i16 i = 0xC6; i <= 0xC7; i++) {
                t[i] = &InstructionDecoder::decodeMovImmToMem;
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

    void decodeMovRegRem(u8 opcode) {
        std::cout << "mov ";
        decodeRegRem(opcode);
    }

    void decodeAddRegRem(u8 opcode) {
        std::cout << "add ";
        decodeRegRem(opcode);
    }

    void decodeSubRegRem(u8 opcode) {
        std::cout << "sub ";
        decodeRegRem(opcode);
    }

    void decodeCmpRegRem(u8 opcode) {
        std::cout << "cmp ";
        decodeRegRem(opcode);
    }

    void decodeRegRem(u8 opcode) {
        u8 d = (opcode >> 1) & 1;
        u8 w = opcode & 1;

        if (!isIndexValid(1)) return;

        u8 modrm = code[pc + 1];
        u8 mod = (modrm >> 6) & 3;
        u8 reg = (modrm >> 3) & 7;
        u8 rm = modrm & 7;

        pc += 2;

        if (d) {
            std::cout << getRegister(reg, w) << ", " << getRM(mod, rm, w) << '\n';
        } else {
            std::cout << getRM(mod, rm, w) << ", " << getRegister(reg, w) << '\n';
        }
    }

    void decodeMovAccMem(u8 opcode) {
        u8 w = opcode & 1;

        pc += 1;

        u16 addr = (w) ? readU16() : readU8();

        if ((opcode >> 1) & 1) {
            std::cout << "mov [" << std::to_string(addr) << "], " << getRegister(0, w) << '\n';
        } else {
            std::cout << "mov " << getRegister(0, w) << ", [" << std::to_string(addr) << "]" << '\n';
        }
    }

    void decodeAddAccMem(u8 opcode) {
        std::cout << "add ";
        decodeAccMem(opcode);
    }

    void decodeSubAccMem(u8 opcode) {
        std::cout << "sub ";
        decodeAccMem(opcode);
    }

    void decodeCmpAccMem(u8 opcode) {
        std::cout << "cmp ";
        decodeAccMem(opcode);
    }

    void decodeAccMem(u8 opcode) {
        u8 w = opcode & 1;

        pc += 1;

        i16 data = (w) ? static_cast<i16>(readU16()) : static_cast<i8>(readU8());

        std::cout << getRegister(0, w) << ", " << std::to_string(data) << '\n';
    }

    void decodeImmRegMem(u8 opcode) {
        u8 w = opcode & 1;
        u8 s = (opcode >> 1) & 1;

        if (!isIndexValid(1)) return;

        u8 modrm = code[pc + 1];
        u8 mod = (modrm >> 6) & 3;
        u8 reg = (modrm >> 3) & 7;
        u8 rm = modrm & 7;

        pc += 2;

        const char* instr[] = {"add", "or", "adc", "sbb", "and", "sub", "xor", "cmp"};

        std::string size = (mod != 3) ? ((w) ? " word" : " byte") : "";
        std::string mem = getRM(mod, rm, w);
        u16 data = (w &&  !s) ? readU16() : readU8();

        std::cout << instr[reg] << size << " " << mem << ", " << std::to_string(data) << '\n';
    }

    void decodeMovImmToReg(u8 opcode) {
        std::cout << "mov ";
        decodeImmToReg(opcode);
    }

    void decodeImmToReg(u8 opcode) {
        u8 w = (opcode >> 3) & 1;
        u8 reg = opcode & 7;

        pc += 1;

        if (w) {
            std::cout << getRegister(reg, w) << ", " << std::to_string(readU16()) << '\n';
        } else {
            std::cout << getRegister(reg, w) << ", " << std::to_string(readU8()) << '\n';
        }
    }

    void decodeMovImmToMem(u8 opcode) {
        std::cout << "mov ";
        decodeImmToMem(opcode);
    }

    void decodeImmToMem(u8 opcode) {
        u8 w = opcode & 1;

        if (!isIndexValid(1)) return;

        u8 modrm = code[pc + 1];
        u8 mod = (modrm >> 6) & 3;
        u8 reg = (modrm >> 3) & 7;
        u8 rm = modrm & 7;

        pc += 2;

        if (w) {
            std::cout << getRM(mod, rm, w) << ", word " << std::to_string(readU16()) << '\n';
        } else {
            std::cout << getRM(mod, rm, w) << ", byte " << std::to_string(readU8()) << '\n';
        }
    }

    std::string getRegister(u8 reg, u8 w) const {
        const char* regs8[] = {"al", "cl", "dl", "bl", "ah", "ch", "dh", "bh"};
        const char* regs16[] = {"ax", "cx", "dx", "bx", "sp", "bp", "si", "di"};

        return w ? regs16[reg] : regs8[reg];
    }

    std::string getRM(u8 mod, u8 rm, u8 w) {
        if (mod == 3) {
            return this->getRegister(rm, w);
        }

        const char* addr[] = {"bx + si", "bx + di", "bp + si", "bp + di", "si", "di", "bp", "bx"};

        if (rm == 6 && mod == 0) {
            return "[" + std::to_string(readU16()) + "]";
        }

        if (mod == 1) {
            return "[" + std::string(addr[rm]) + formatDisplacement<i8>() + "]";
        } else if (mod == 2) {
            return "[" + std::string(addr[rm]) + formatDisplacement<i16>() + "]";
        }

        return "[" + std::string(addr[rm]) + "]";
    }

    template<typename T>
    std::string formatDisplacement() {
        auto disp_unsigned = (sizeof(T) == 1) ? this->readU8() : this->readU16();
        auto disp_signed = static_cast<T>(disp_unsigned);

        if (disp_signed > 0) {
            return " + " + std::to_string(disp_signed);
        } else if (disp_signed < 0) {
            return " - " + std::to_string(-disp_signed);
        }

        return "";
    }

    u8 readU8() {
        if (!isIndexValid(1)) throw std::runtime_error("[ERROR] Cannot read u8.");
        return code[pc++];
    }

    u16 readU16() {
        if (!isIndexValid(2)) throw std::runtime_error("[ERROR] Cannot read u16.");
        u16 value = (code[pc + 1] << 8) | code[pc];
        pc += 2;
        return value;
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

    bool isIndexValid(std::size_t bytes) const {
        return pc + bytes <= code.size();
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

