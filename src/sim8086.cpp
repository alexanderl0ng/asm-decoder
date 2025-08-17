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

            for (u8 i = 0x00; i <= 0x03; i++) {
                t[i] = &InstructionDecoder::decodeAddRegMem;
            }

            for (u8 i = 0x04; i <= 0x05; i++) {
                t[i] = &InstructionDecoder::decodeAddAccMem;
            }

            for (u8 i = 0x06; i <= 0x07; i++) {
                t[i] = &InstructionDecoder::decodeSegRegPushPop;
            }

            for (u8 i = 0x08; i <= 0x0B; i++) {
                t[i] = &InstructionDecoder::decodeOrRegMem;
            }

            for (u8 i = 0x0C; i <= 0x0D; i++) {
                t[i] = &InstructionDecoder::decodeOrAccMem;
            }

            for (u8 i = 0x0E; i <= 0x0F; i++) {
                t[i] = &InstructionDecoder::decodeSegRegPushPop;
            }

            for (u8 i = 0x10; i <= 0x13; i++) {
                t[i] = &InstructionDecoder::decodeAdcRegMem;
            }

            for (u8 i = 0x14; i <= 0x15; i++) {
                t[i] = &InstructionDecoder::decodeAdcAccMem;
            }

            for (u8 i = 0x16; i <= 0x17; i++) {
                t[i] = &InstructionDecoder::decodeSegRegPushPop;
            }

            for (u8 i = 0x18; i <= 0x1B; i++) {
                t[i] = &InstructionDecoder::decodeSbbRegMem;
            }

            for (u8 i = 0x1C; i <= 0x1D; i++) {
                t[i] = &InstructionDecoder::decodeSbbAccMem;
            }

            for (u8 i = 0x1E; i <= 0x1F; i++) {
                t[i] = &InstructionDecoder::decodeSegRegPushPop;
            }

            for (u8 i = 0x20; i <= 0x23; i++) {
                t[i] = &InstructionDecoder::decodeAndRegMem;
            }

            for (u8 i = 0x24; i <= 0x25; i++) {
                t[i] = &InstructionDecoder::decodeAndAccMem;
            }

            for (u8 i = 0x28; i <= 0x2B; i++) {
                t[i] = &InstructionDecoder::decodeSubRegMem;
            }

            for (u8 i = 0x2C; i <= 0x2D; i++) {
                t[i] = &InstructionDecoder::decodeSubAccMem;
            }

            for (u8 i = 0x30; i <= 0x33; i++) {
                t[i] = &InstructionDecoder::decodeXorRegMem;
            }

            for (u8 i = 0x34; i <= 0x35; i++) {
                t[i] = &InstructionDecoder::decodeXorAccMem;
            }

            for (u8 i = 0x38; i <= 0x3B; i++) {
                t[i] = &InstructionDecoder::decodeCmpRegMem;
            }

            for (u8 i = 0x3C; i <= 0x3D; i++) {
                t[i] = &InstructionDecoder::decodeCmpAccMem;
            }

            for (u8 i = 0x40; i <= 0x4F; i++) {
                t[i] = &InstructionDecoder::decodeIncDec;
            }

            for (u8 i = 0x50; i <= 0x5F; i++) {
                t[i] = &InstructionDecoder::decodePushPop;
            }

            for (u8 i = 0x70; i <= 0x7F; i++) {
                t[i] = &InstructionDecoder::decodeConJmp;
            }

            for (u8 i = 0x80; i <= 0x83; i++) {
                t[i] = &InstructionDecoder::decodeImmRegMem;
            }

            for (u8 i = 0x84; i <= 0x85; i++) {
                t[i] = &InstructionDecoder::decodeTestRegMem;
            }

            for (u8 i = 0x86; i <= 0x87; i++) {
                t[i] = &InstructionDecoder::decodeXchgRegMem;
            }

            for (u8 i = 0x88; i <= 0x8B; i++) {
                t[i] = &InstructionDecoder::decodeMovRegMem;
            }

            for (u8 i = 0xA0; i <= 0xA3; i++) {
                t[i] = &InstructionDecoder::decodeMovAccMem;
            }

            for (u8 i = 0xA8; i <= 0xA9; i++) {
                t[i] = &InstructionDecoder::decodeTestAccMem;
            }

            for (u8 i = 0xB0; i <= 0xBF; i++) {
                t[i] = &InstructionDecoder::decodeMovImmToReg;
            }

            for (u8 i = 0xC6; i <= 0xC7; i++) {
                t[i] = &InstructionDecoder::decodeMovImmToMem;
            }

            for (u8 i = 0xCC; i <= 0xCE; i++) {
                t[i] = &InstructionDecoder::decodeInt;
            }

            for (u8 i = 0xE0; i <= 0xE3; i++) {
                t[i] = &InstructionDecoder::decodeLoop;
            }

            for (u8 i = 0xF2; i <= 0xF3; i++) {
                t[i] = &InstructionDecoder::decodeRepPrefix;
            }

            for (u8 i = 0xF6; i <= 0xF7; i++) {
                t[i] = &InstructionDecoder::decodeTestGroupImmRegMem;
            }

            for (u8 i = 0xF8; i <= 0xFD; i++) {
                t[i] = &InstructionDecoder::decodeFlagOps;
            }


            t[0x27] = &InstructionDecoder::decodeDaa;
            t[0x2F] = &InstructionDecoder::decodeDas;
            t[0x37] = &InstructionDecoder::decodeAaa;
            t[0x3F] = &InstructionDecoder::decodeAas;
            t[0x8C] = &InstructionDecoder::decodeRegSegReg;
            t[0x8E] = &InstructionDecoder::decodeRegSegReg;
            t[0x90] = &InstructionDecoder::decodeNop;
            t[0x9B] = &InstructionDecoder::decodeWait;
            t[0x9C] = &InstructionDecoder::decodePushf;
            t[0x9D] = &InstructionDecoder::decodePopf;
            t[0x9E] = &InstructionDecoder::decodeSahf;
            t[0x9F] = &InstructionDecoder::decodeLahf;
            t[0xF0] = &InstructionDecoder::decodeLock;
            t[0xF4] = &InstructionDecoder::decodeHlt;
            t[0xF5] = &InstructionDecoder::decodeCmc;

            return t;
        }();

        return table;
    }

    void unknownOpcode(u8 opcode) {
        std::cout << "[WARNING] Unknown opcode " << std::format("0x{:02x}", int(opcode))
            << " at position " << std::dec << pc << '\n';
        pc++;
    }

    void decodeAddRegMem(u8 opcode) {
        std::cout << "add ";
        decodeRegMem(opcode);
    }

    void decodeOrRegMem(u8 opcode) {
        std::cout << "or ";
        decodeRegMem(opcode);
    }

    void decodeAdcRegMem(u8 opcode) {
        std::cout << "adc ";
        decodeRegMem(opcode);
    }

    void decodeSbbRegMem(u8 opcode) {
        std::cout << "sbb ";
        decodeRegMem(opcode);
    }

    void decodeAndRegMem(u8 opcode) {
        std::cout << "and ";
        decodeRegMem(opcode);
    }

    void decodeSubRegMem(u8 opcode) {
        std::cout << "sub ";
        decodeRegMem(opcode);
    }

    void decodeCmpRegMem(u8 opcode) {
        std::cout << "cmp ";
        decodeRegMem(opcode);
    }

    void decodeXorRegMem(u8 opcode) {
        std::cout << "xor ";
        decodeRegMem(opcode);
    }

    void decodeTestRegMem(u8 opcode) {
        std::cout << "test ";
        decodeRegMem(opcode);
    }

    void decodeXchgRegMem(u8 opcode) {
        std::cout << "xchg ";
        decodeRegMem(opcode);
    }

    void decodeMovRegMem(u8 opcode) {
        std::cout << "mov ";
        decodeRegMem(opcode);
    }

    void decodeRegMem(u8 opcode) {
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

    void decodeRegSegReg(u8 opcode) {
        u8 is_to_segReg = (opcode >> 1) & 1;

        if (!isIndexValid(1)) return;

        u8 modrm = code[pc + 1];
        u8 mod = (modrm >> 6) & 3;
        u8 sr = (modrm >> 3) & 3;
        u8 rm = modrm & 7;

        pc += 2;

        if (is_to_segReg) {
            std::cout << "mov " << getsegReg(sr) << ", " << getRM(mod, rm, 1) << '\n';
        } else {
            std::cout << "mov " << getRM(mod, rm, 1) << ", " << getsegReg(sr) << '\n';
        }
    }

    void decodeMovAccMem(u8 opcode) {
        u8 w = opcode & 1;

        pc++;

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

    void decodeOrAccMem(u8 opcode) {
        std::cout << "or ";
        decodeAccMem(opcode);
    }

    void decodeAdcAccMem(u8 opcode) {
        std::cout << "adc ";
        decodeAccMem(opcode);
    }

    void decodeSbbAccMem(u8 opcode) {
        std::cout << "sbb ";
        decodeAccMem(opcode);
    }

    void decodeAndAccMem(u8 opcode) {
        std::cout << "and ";
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

    void decodeXorAccMem(u8 opcode) {
        std::cout << "xor ";
        decodeAccMem(opcode);
    }

    void decodeTestAccMem(u8 opcode) {
        std::cout << "test ";
        decodeAccMem(opcode);
    }

    void decodeAccMem(u8 opcode) {
        u8 w = opcode & 1;

        pc++;

        i16 data = (w) ? static_cast<i16>(readU16()) : static_cast<i8>(readU8());

        std::cout << getRegister(0, w) << ", " << std::to_string(data) << '\n';
    }

    void decodeIncDec(u8 opcode) {
        u8 is_dec = (opcode >> 3) & 1;
        u8 reg = opcode & 7;

        pc++;

        std::cout << ((is_dec) ? "dec " : "inc ") << getRegister(reg, 1) << '\n';
    }

    void decodeSegRegPushPop (u8 opcode) {
        u8 is_pop = opcode & 1;
        u8 sr = (opcode >> 3) & 3;

        pc++;

        std::cout << ((is_pop) ? "pop " : "push ") << getsegReg(sr) << '\n';
    }


    void decodePushPop(u8 opcode) {
        u8 is_pop = (opcode >> 3) & 1;
        u8 reg = opcode & 7;

        pc++;

        std::cout << ((is_pop) ? "pop " : "push ") << getRegister(reg, 1) << '\n';
    }

    void decodeConJmp(u8 opcode) {
        const char* jumpNames[] = {
            "jo", "jno", "jb", "jnb", "je", "jne", "jbe", "jnbe",
            "js", "jns", "jp", "jnp", "jl", "jnl", "jle", "jnle"
        };

        pc++;
        i8 disp = static_cast<i8>(readU8());
        std::cout << jumpNames[opcode - 0x70] << ((disp >= 0) ? " $+ " : " $- ") << ((disp >= 0) ? disp + 2 : -(disp + 2)) << '\n';
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
        std::string mem = getRM(mod, rm, w); //getRM first to increment pc counter correctly for data
        u16 data = (w && !s) ? readU16() : readU8();

        std::cout << instr[reg] << size << " " << mem << ", " << std::to_string(data) << '\n';
    }

    void decodeTestGroupImmRegMem(u8 opcode) {
        u8 w = opcode & 1;

        if (!isIndexValid(1)) return;

        u8 modrm = code[pc + 1];
        u8 mod = (modrm >> 6) & 3;
        u8 reg = (modrm >> 3) & 7;
        u8 rm = modrm & 7;

        pc += 2;

        const char* instr[] = {"test", "", "not", "neg", "mul", "imul", "div", "idiv"};

        std::string size = (mod != 3) ? ((w) ? " word" : " byte") : "";
        std::string mem = getRM(mod, rm, w);

        if (reg == 0) {
            u16 data = (w) ? readU16() : readU8();
            std::cout << instr[reg] << size << " " << mem << ", " << std::to_string(data) << '\n';
        } else {
            std::cout << instr[reg] << size << " " << mem << '\n';
        }
    }

    void decodeMovImmToReg(u8 opcode) {
        std::cout << "mov ";
        decodeImmToReg(opcode);
    }

    void decodeImmToReg(u8 opcode) {
        u8 w = (opcode >> 3) & 1;
        u8 reg = opcode & 7;

        pc++;

        if (w) {
            std::cout << getRegister(reg, w) << ", " << std::to_string(static_cast<i16>(readU16())) << '\n';
        } else {
            std::cout << getRegister(reg, w) << ", " << std::to_string(static_cast<i8>(readU8())) << '\n';
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
        u8 rm = modrm & 7;

        pc += 2;

        if (w) {
            std::cout << getRM(mod, rm, w) << ", word " << std::to_string(readU16()) << '\n';
        } else {
            std::cout << getRM(mod, rm, w) << ", byte " << std::to_string(readU8()) << '\n';
        }
    }

    void decodeInt(u8 opcode) {
        const char* ints[] = {"int3", "int", "into", "iret"};

        pc++;
        if (opcode - 0xCC != 1) {
            std::cout << ints[opcode - 0xCC] << '\n';
        } else {
            std::cout << ints[opcode - 0xCC] << " " << readU8() << '\n';
        }
    }

    void decodeLoop(u8 opcode) {
        const char* loopNames[] = {"loopne", "loope", "loop", "jcxz"};

        pc++;
        i8 disp = static_cast<i8>(readU8());
        std::cout << loopNames[opcode - 0xE0] << ((disp >= 0) ? " $+ " : " $- ") << ((disp >= 0) ? disp + 2 : -(disp + 2)) << '\n';
    }

    void decodeFlagOps(u8 opcode) {
        const char* flagOps[] = {"clc", "stc", "cli", "sti", "cld", "std"};

        std::cout << flagOps[opcode - 0xF8] << '\n';
        pc++;
    }


    void decodeDaa(u8 opcode) { std::cout << "daa" << '\n'; pc++; }

    void decodeDas(u8 opcode) { std::cout << "das" << '\n'; pc++; }

    void decodeAaa(u8 opcode) { std::cout << "aaa" << '\n'; pc++; }

    void decodeAas(u8 opcode) { std::cout << "aas" << '\n'; pc++; }

    void decodeNop(u8 opcode) { std::cout << "nop" << '\n'; pc++; }

    void decodeWait(u8 opcode) { std::cout << "wait" << '\n'; pc++; }

    void decodePushf(u8 opcode) { std::cout << "pushf" << '\n'; pc++; }

    void decodePopf(u8 opcode) { std::cout << "popf" << '\n'; pc++; }

    void decodeSahf(u8 opcode) { std::cout << "sahf" << '\n'; pc++; }

    void decodeLahf(u8 opcode) { std::cout << "lahf" << '\n'; pc++; }

    void decodeRepPrefix(u8 opcode) {
        u8 is_rep = opcode & 1;

        std::cout << ((is_rep) ? "rep " : "repne ");
    }


    void decodeHlt(u8 opcode) { std::cout << "htl" << '\n'; pc++; }

    void decodeCmc(u8 opcode) { std::cout << "cmc" << '\n'; pc++; }

    void decodeLock(u8 opcode) { std::cout << "lock "; pc++; }

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

    std::string getsegReg(u8 segReg) {
        const char* sregs[] = {"es", "cs", "ss", "ds"};

        return sregs[segReg];
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

