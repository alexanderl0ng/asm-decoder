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
            (this->*handler.handler)(opcode, handler.mnemonic);
        }
    }

private:
    std::vector<u8> code;
    std::size_t pc = 0;
    std::string ctx = "";

    struct OpcodeEntry {
        void(InstructionDecoder::*handler)(u8, std::string);
        std::string mnemonic;
    };

    class ContextGuard {
        std::string& ctx_ref;
    public:
        ContextGuard(std::string& ctx) : ctx_ref(ctx) {};
        ~ContextGuard() { ctx_ref = ""; }
    };

    static std::array<OpcodeEntry, 256>& getOpcodeTable() {
        static std::array<OpcodeEntry, 256> table = []() {
            std::array<OpcodeEntry, 256> t;
            t.fill({&InstructionDecoder::unknownOpcode, ""});

            for (u8 i = 0x00; i <= 0x03; i++) {
                t[i] = {&InstructionDecoder::decodeRegMem, "add"};
            }

            for (u8 i = 0x04; i <= 0x05; i++) {
                t[i] = {&InstructionDecoder::decodeAccMem, "add"};
            }

            for (u8 i = 0x06; i <= 0x07; i++) {
                t[i] = {&InstructionDecoder::decodeSegRegPushPop, ""};
            }

            for (u8 i = 0x08; i <= 0x0B; i++) {
                t[i] = {&InstructionDecoder::decodeRegMem, "or"};
            }

            for (u8 i = 0x0C; i <= 0x0D; i++) {
                t[i] = {&InstructionDecoder::decodeAccMem, "or"};
            }

            for (u8 i = 0x0E; i <= 0x0F; i++) {
                t[i] = {&InstructionDecoder::decodeSegRegPushPop, ""};
            }

            for (u8 i = 0x10; i <= 0x13; i++) {
                t[i] = {&InstructionDecoder::decodeRegMem, "adc"};
            }

            for (u8 i = 0x14; i <= 0x15; i++) {
                t[i] = {&InstructionDecoder::decodeAccMem, "adc"};
            }

            for (u8 i = 0x16; i <= 0x17; i++) {
                t[i] = {&InstructionDecoder::decodeSegRegPushPop, ""};
            }

            for (u8 i = 0x18; i <= 0x1B; i++) {
                t[i] = {&InstructionDecoder::decodeRegMem, "sbb"};
            }

            for (u8 i = 0x1C; i <= 0x1D; i++) {
                t[i] = {&InstructionDecoder::decodeAccMem, "sbb"};
            }

            for (u8 i = 0x1E; i <= 0x1F; i++) {
                t[i] = {&InstructionDecoder::decodeSegRegPushPop, ""};
            }

            for (u8 i = 0x20; i <= 0x23; i++) {
                t[i] = {&InstructionDecoder::decodeRegMem, "and"};
            }

            for (u8 i = 0x24; i <= 0x25; i++) {
                t[i] = {&InstructionDecoder::decodeAccMem, "and"};
            }

            for (u8 i = 0x28; i <= 0x2B; i++) {
                t[i] = {&InstructionDecoder::decodeRegMem, "sub"};
            }

            for (u8 i = 0x2C; i <= 0x2D; i++) {
                t[i] = {&InstructionDecoder::decodeAccMem, "sub"};
            }

            for (u8 i = 0x30; i <= 0x33; i++) {
                t[i] = {&InstructionDecoder::decodeRegMem, "xor"};
            }

            for (u8 i = 0x34; i <= 0x35; i++) {
                t[i] = {&InstructionDecoder::decodeAccMem, "xor"};
            }

            for (u8 i = 0x38; i <= 0x3B; i++) {
                t[i] = {&InstructionDecoder::decodeRegMem, "cmp"};
            }

            for (u8 i = 0x3C; i <= 0x3D; i++) {
                t[i] = {&InstructionDecoder::decodeAccMem, "cmp"};
            }

            for (u8 i = 0x40; i <= 0x4F; i++) {
                t[i] = {&InstructionDecoder::decodeIncDec, ""};
            }

            for (u8 i = 0x50; i <= 0x5F; i++) {
                t[i] = {&InstructionDecoder::decodePushPop, ""};
            }

            for (u8 i = 0x70; i <= 0x7F; i++) {
                t[i] = {&InstructionDecoder::decodeConJmp, ""};
            }

            for (u8 i = 0x80; i <= 0x83; i++) {
                t[i] = {&InstructionDecoder::decodeImmRegMem, ""};
            }

            for (u8 i = 0x84; i <= 0x85; i++) {
                t[i] = {&InstructionDecoder::decodeRegMem, "test"};
            }

            for (u8 i = 0x86; i <= 0x87; i++) {
                t[i] = {&InstructionDecoder::decodeXchgRegMem, "xchg"};
            }

            for (u8 i = 0x88; i <= 0x8B; i++) {
                t[i] = {&InstructionDecoder::decodeRegMem, "mov"};
            }

            for (u8 i = 0x90; i <= 0x97; i++) {
                t[i] = {&InstructionDecoder::decodeXchgAccMem, "xchg"};
            }

            for (u8 i = 0xA0; i <= 0xA3; i++) {
                t[i] = {&InstructionDecoder::decodeMovAccMem, "mov"};
            }

            for (u8 i = 0xA4; i <= 0xA7; i++) {
                t[i] = {&InstructionDecoder::decodeStrOps, ""};
            }

            for (u8 i = 0xA8; i <= 0xA9; i++) {
                t[i] = {&InstructionDecoder::decodeAccMem, "test"};
            }

            for (u8 i = 0xAA; i <= 0xAF; i++) {
                t[i] = {&InstructionDecoder::decodeStrOps, ""};
            }

            for (u8 i = 0xB0; i <= 0xBF; i++) {
                t[i] = {&InstructionDecoder::decodeImmToReg, "mov"};
            }

            for (u8 i = 0xC6; i <= 0xC7; i++) {
                t[i] = {&InstructionDecoder::decodeImmToMem, "mov"};
            }

            for (u8 i = 0xCC; i <= 0xCF; i++) {
                t[i] = {&InstructionDecoder::decodeInt, ""};
            }

            for (u8 i = 0xD0; i <= 0xD3; i++) {
                t[i] = {&InstructionDecoder::decodeShtRot, ""};
            }

            for (u8 i = 0xD8; i <= 0xDF; i++) {
                t[i] = {&InstructionDecoder::decodeESC, "esc"};
            }

            for (u8 i = 0xE0; i <= 0xE3; i++) {
                t[i] = {&InstructionDecoder::decodeLoop, ""};
            }

            for (u8 i = 0xE4; i <= 0xE7; i++) {
                t[i] = {&InstructionDecoder::decodeInOut, ""};
            }

            for (u8 i = 0xEC; i <= 0xEF; i++) {
                t[i] = {&InstructionDecoder::decodeInOut, ""};
            }

            for (u8 i = 0xF2; i <= 0xF3; i++) {
                t[i] = {&InstructionDecoder::decodeRepPrefix, ""};
            }

            for (u8 i = 0xF6; i <= 0xF7; i++) {
                t[i] = {&InstructionDecoder::decodeGrp3, ""};
            }

            for (u8 i = 0xF8; i <= 0xFD; i++) {
                t[i] = {&InstructionDecoder::decodeFlagOps, ""};
            }

            t[0x26] = {&InstructionDecoder::decodeSegmentPrefix, "es:"};
            t[0x27] = {&InstructionDecoder::decodeNullaryInstruction, "daa"};
            t[0x2E] = {&InstructionDecoder::decodeSegmentPrefix, "cs:"};
            t[0x2F] = {&InstructionDecoder::decodeNullaryInstruction, "das"};
            t[0x36] = {&InstructionDecoder::decodeSegmentPrefix, "ss:"};
            t[0x37] = {&InstructionDecoder::decodeNullaryInstruction, "aaa"};
            t[0x3E] = {&InstructionDecoder::decodeSegmentPrefix, "ds:"};
            t[0x3F] = {&InstructionDecoder::decodeNullaryInstruction, "aas"};
            t[0x8C] = {&InstructionDecoder::decodeRegSegReg, "mov"};
            t[0x8D] = {&InstructionDecoder::decodeLoads, "lea"};
            t[0x8E] = {&InstructionDecoder::decodeRegSegReg, "mov"};
            t[0x8F] = {&InstructionDecoder::decodeRegMem16, "pop"};
            t[0x98] = {&InstructionDecoder::decodeNullaryInstruction, "cbw"};
            t[0x99] = {&InstructionDecoder::decodeNullaryInstruction, "cwd"};
            t[0x9A] = {&InstructionDecoder::decodeControlTransfer, "call"};
            t[0x9B] = {&InstructionDecoder::decodeNullaryInstruction, "wait"};
            t[0x9C] = {&InstructionDecoder::decodeNullaryInstruction, "pushf"};
            t[0x9D] = {&InstructionDecoder::decodeNullaryInstruction, "popf"};
            t[0x9E] = {&InstructionDecoder::decodeNullaryInstruction, "sahf"};
            t[0x9F] = {&InstructionDecoder::decodeNullaryInstruction, "lahf"};
            t[0xD4] = {&InstructionDecoder::decodeNullaryInstructionTwoBytes, "aam"};
            t[0xD5] = {&InstructionDecoder::decodeNullaryInstructionTwoBytes, "aad"};
            t[0xD7] = {&InstructionDecoder::decodeNullaryInstruction, "xlat"};
            t[0xC2] = {&InstructionDecoder::decodeRets, "ret"};
            t[0xC3] = {&InstructionDecoder::decodeRets, "ret"};
            t[0xC4] = {&InstructionDecoder::decodeLoads, "les"};
            t[0xC5] = {&InstructionDecoder::decodeLoads, "lds"};
            t[0xCA] = {&InstructionDecoder::decodeRets, "retf"};
            t[0xCB] = {&InstructionDecoder::decodeRets, "retf"};
            t[0xE8] = {&InstructionDecoder::decodeControlTransfer, "call"};
            t[0xE9] = {&InstructionDecoder::decodeControlTransfer, "jmp"};
            t[0xEA] = {&InstructionDecoder::decodeControlTransfer, "jmp"};
            t[0xEB] = {&InstructionDecoder::decodeControlTransfer, "jmp"};
            t[0xF0] = {&InstructionDecoder::decodeNullaryPrefix, "lock"};
            t[0xF4] = {&InstructionDecoder::decodeNullaryInstruction, "hlt"};
            t[0xF5] = {&InstructionDecoder::decodeNullaryInstruction, "cmc"};
            t[0xF8] = {&InstructionDecoder::decodeNullaryInstruction, "clc"};
            t[0xF9] = {&InstructionDecoder::decodeNullaryInstruction, "stc"};
            t[0xFA] = {&InstructionDecoder::decodeNullaryInstruction, "cli"};
            t[0xFB] = {&InstructionDecoder::decodeNullaryInstruction, "sti"};
            t[0xFC] = {&InstructionDecoder::decodeNullaryInstruction, "cld"};
            t[0xFD] = {&InstructionDecoder::decodeNullaryInstruction, "std"};
            t[0xFE] = {&InstructionDecoder::decodeGrp5, ""};
            t[0xFF] = {&InstructionDecoder::decodeGrp5, ""};

            return t;
        }();

        return table;
    }

    void unknownOpcode(u8 opcode, std::string mnemonic) {
        std::cout << "[WARNING] Unknown opcode " << std::format("0x{:02x}", int(opcode))
            << " at position " << std::dec << pc << '\n';
        pc++;
    }

    void decodeXchgRegMem(u8 opcode, std::string mnemonic) {
        u8 w = opcode & 1;

        if (!isIndexValid(1)) return;

        u8 modrm = code[pc + 1];
        u8 mod = (modrm >> 6) & 3;
        u8 reg = (modrm >> 3) & 7;
        u8 rm = modrm & 7;

        pc += 2;

        std::cout << mnemonic << " " << getRM(mod, rm, w) << ", " << getRegister(reg, w) << '\n';
    }

    void decodeRegMem(u8 opcode, std::string mnemonic) {
        u8 d = (opcode >> 1) & 1;
        u8 w = opcode & 1;

        if (!isIndexValid(1)) return;

        u8 modrm = code[pc + 1];
        u8 mod = (modrm >> 6) & 3;
        u8 reg = (modrm >> 3) & 7;
        u8 rm = modrm & 7;

        pc += 2;

        if (d) {
            std::cout << mnemonic << " " << getRegister(reg, w) << ", " << getRM(mod, rm, w) << '\n';
        } else {
            std::cout << mnemonic << " " << getRM(mod, rm, w) << ", " << getRegister(reg, w) << '\n';
        }
    }

    void decodeRets(u8 opcode, std::string mnemonic) {
        u8 has_imm = !(opcode & 1);

        pc++;

        if (has_imm) {
            std::cout << mnemonic << " " << std::to_string(static_cast<i16>(readU16())) << '\n';
        } else {
            std::cout << mnemonic << '\n';
        }
    }

    void decodeLoads(u8 opcode, std::string mnemonic) {
        u8 w = 1;

        if (!isIndexValid(1)) return;

        u8 modrm = code[pc + 1];
        u8 mod = (modrm >> 6) & 3;
        u8 reg = (modrm >> 3) & 7;
        u8 rm = modrm & 7;

        pc += 2;

        std::cout << mnemonic << " " << getRegister(reg, w) << ", " << getRM(mod, rm, w) << '\n';
    }

    void decodeRegSegReg(u8 opcode, std::string mnemonic) {
        u8 is_to_segReg = (opcode >> 1) & 1;

        if (!isIndexValid(1)) return;

        u8 modrm = code[pc + 1];
        u8 mod = (modrm >> 6) & 3;
        u8 sr = (modrm >> 3) & 3;
        u8 rm = modrm & 7;

        pc += 2;

        if (is_to_segReg) {
            std::cout << mnemonic << " " << getSegReg(sr) << ", " << getRM(mod, rm, 1) << '\n';
        } else {
            std::cout << mnemonic << " " << getRM(mod, rm, 1) << ", " << getSegReg(sr) << '\n';
        }
    }

    void decodeMovAccMem(u8 opcode, std::string mnemonic) {
        u8 w = opcode & 1;

        pc++;

        u16 addr = (w) ? readU16() : readU8();

        if ((opcode >> 1) & 1) {
            std::cout << mnemonic << " [" << std::to_string(addr) << "], " << getRegister(0, w) << '\n';
        } else {
            std::cout << mnemonic << " " << getRegister(0, w) << ", [" << std::to_string(addr) << "]" << '\n';
        }
    }

    void decodeXchgAccMem(u8 opcode, std::string mnemonic) {
        u8 reg = opcode & 7;

        pc++;

        std::cout << mnemonic << " ax, " << getRegister(reg, 1) << '\n';
    }

    void decodeAccMem(u8 opcode, std::string mnemonic) {
        u8 w = opcode & 1;

        pc++;

        i16 data = (w) ? static_cast<i16>(readU16()) : static_cast<i8>(readU8());

        std::cout << mnemonic << " " << getRegister(0, w) << ", " << std::to_string(data) << '\n';
    }

    void decodeStrOps(u8 opcode, std::string mnemonic) {
        const char* instr[] = {"movs", "cmps", "", "stos", "lods", "scas"};

        u8 w = opcode & 1;
        u8 reg = (opcode - 0xA4) >> 1;

        pc++;

        std::cout << instr[reg] << ((w) ? 'w' : 'b') << '\n';
    }


    void decodeIncDec(u8 opcode, std::string mnemonic) {
        u8 is_dec = (opcode >> 3) & 1;
        u8 reg = opcode & 7;

        pc++;

        std::cout << ((is_dec) ? "dec " : "inc ") << getRegister(reg, 1) << '\n';
    }

    void decodeSegRegPushPop (u8 opcode, std::string mnemonic) {
        u8 is_pop = opcode & 1;
        u8 sr = (opcode >> 3) & 3;

        pc++;

        std::cout << ((is_pop) ? "pop " : "push ") << getSegReg(sr) << '\n';
    }


    void decodePushPop(u8 opcode, std::string mnemonic) {
        u8 is_pop = (opcode >> 3) & 1;
        u8 reg = opcode & 7;

        pc++;

        std::cout << ((is_pop) ? "pop " : "push ") << getRegister(reg, 1) << '\n';
    }

    void decodeConJmp(u8 opcode, std::string mnemonic) {
        const char* jumpNames[] = {
            "jo", "jno", "jb", "jnb", "je", "jne", "jbe", "jnbe",
            "js", "jns", "jp", "jnp", "jl", "jnl", "jle", "jnle"
        };

        pc++;
        i8 disp = static_cast<i8>(readU8());
        std::cout << jumpNames[opcode - 0x70] << ((disp >= 0) ? " $+ " : " $- ") << ((disp >= 0) ? disp + 2 : -(disp + 2)) << '\n';
    }

    void decodeImmRegMem(u8 opcode, std::string mnemonic) {
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

    void decodeInOut(u8 opcode, std::string mnemonic) {
        u8 is_out = (opcode >> 1) & 1;
        u8 is_dx = (opcode >> 3) & 1;
        u8 w = opcode & 1;

        pc++;
        if (is_dx) {
            std::cout << ((is_out) ? "out " : "in ") << ((is_out) ? "dx, " : getRegister(0, w)) << ((is_out) ? getRegister(0, w) : ", dx") << '\n';
        } else {
            std::cout << ((is_out) ? "out " : "in ") << ((is_out) ? std::to_string(readU8()) : getRegister(0, w)) << ", " << ((is_out) ? getRegister(0, w) : std::to_string(readU8())) << '\n';
        }
    }

    void decodeControlTransfer(u8 opcode, std::string mnemonic) {
        pc++;

        if (opcode == 0x9A || opcode == 0xEA) {
            u16 displacement = readU16();
            u16 segment = readU16();
            std::cout << mnemonic << " " << segment << ":" << displacement << '\n';
        } else if (opcode == 0xE8 || opcode == 0xE9) {
            i16 displacement = static_cast<i16>(readU16());
            i16 target_displacement = displacement + pc;
            std::cout << mnemonic << " " << target_displacement << '\n';
        } else {
            std::cout << mnemonic << " " << readU8() << '\n';
        }
    }

    void decodeGrp3(u8 opcode, std::string mnemonic) {
        const char* instr[] = {"test", "", "not", "neg", "mul", "imul", "div", "idiv"};
        u8 w = opcode & 1;

        if (!isIndexValid(1)) return;

        u8 modrm = code[pc + 1];
        u8 mod = (modrm >> 6) & 3;
        u8 reg = (modrm >> 3) & 7;
        u8 rm = modrm & 7;

        pc += 2;

        std::string size = (mod != 3) ? ((w) ? " word" : " byte") : "";
        std::string mem = getRM(mod, rm, w);

        if (reg == 0) {
            u16 data = (w) ? readU16() : readU8();
            std::cout << instr[reg] << size << " " << mem << ", " << std::to_string(data) << '\n';
        } else {
            std::cout << instr[reg] << size << " " << mem << '\n';
        }
    }

    void decodeGrp5(u8 opcode, std::string mnemonic) {
        const char* instr[] =  {"inc", "dec", "call", "call far", "jmp", "jmp far", "push", ""};

        if (!isIndexValid(1)) return;
        u8 modrm = code[pc + 1];
        u8 reg = (modrm >> 3) & 7;

        decodeRegMem16(opcode, instr[reg]);
    }

    void decodeRegMem16(u8 opcode, std::string mnemonic) {

        u8 w = opcode & 1;

        if (!isIndexValid(1)) return;

        u8 modrm = code[pc + 1];
        u8 mod = (modrm >> 6) & 3;
        u8 reg = (modrm >> 3) & 7;
        u8 rm = modrm & 7;

        pc += 2;
        if (2 <= reg && reg <= 5) {
            std::cout << mnemonic << " " << getRM(mod, rm, w) << '\n';
        } else {
            std::cout << mnemonic << " " << getRM(mod, rm, w, true) << '\n';
        }
    }

    void decodeImmToReg(u8 opcode, std::string mnemonic) {
        u8 w = (opcode >> 3) & 1;
        u8 reg = opcode & 7;

        pc++;

        if (w) {
            std::cout << mnemonic << " " << getRegister(reg, w) << ", " << std::to_string(static_cast<i16>(readU16())) << '\n';
        } else {
            std::cout << mnemonic << " " << getRegister(reg, w) << ", " << std::to_string(static_cast<i8>(readU8())) << '\n';
        }
    }

    void decodeImmToMem(u8 opcode, std::string mnemonic) {
        u8 w = opcode & 1;

        if (!isIndexValid(1)) return;

        u8 modrm = code[pc + 1];
        u8 mod = (modrm >> 6) & 3;
        u8 rm = modrm & 7;

        pc += 2;

        if (w) {
            std::cout << mnemonic << " " << getRM(mod, rm, w) << ", word " << std::to_string(readU16()) << '\n';
        } else {
            std::cout << mnemonic << " " << getRM(mod, rm, w) << ", byte " << std::to_string(readU8()) << '\n';
        }
    }

    void decodeInt(u8 opcode, std::string mnemonic) {
        const char* ints[] = {"int3", "int", "into", "iret"};

        pc++;
        if (opcode - 0xCC != 1) {
            std::cout << ints[opcode - 0xCC] << '\n';
        } else {
            std::cout << ints[opcode - 0xCC] << " " << std::to_string(readU8()) << '\n';
        }
    }

    void decodeShtRot(u8 opcode, std::string mnemonic) {
        const char* instr[] = {"rol", "ror", "rcl", "rcr", "sal", "shr", "", "sar"};
        const char* cnt[] = {"1", "cl"};

        u8 w = opcode & 1;
        u8 count = (opcode >> 1) & 1;

        if (!isIndexValid(1)) return;

        u8 modrm = code[pc + 1];
        u8 mod = (modrm >> 6) & 3;
        u8 reg = (modrm >> 3) & 7;
        u8 rm = modrm & 7;

        pc += 2;

        std::cout << instr[reg] << " " << getRM(mod, rm, w, true) << ", " << cnt[count] << '\n';
    }

    void decodeESC(u8 opcode, std::string mnemonic) {
        if (!isIndexValid(1)) return;

        u8 modrm = code[pc + 1];
        u8 mod = (modrm >> 6) & 3;
        u8 reg = (modrm >> 3) & 7;
        u8 rm = modrm & 7;

        u8 cop_op = ((opcode & 7) << 3) | reg;

        std::cout << mnemonic << " " << std::to_string(cop_op) << ", " << getRM(mod, rm, 1) << '\n';
    }

    void decodeLoop(u8 opcode, std::string mnemonic) {
        const char* loopNames[] = {"loopne", "loope", "loop", "jcxz"};

        pc++;
        i8 disp = static_cast<i8>(readU8());
        std::cout << loopNames[opcode - 0xE0] << ((disp >= 0) ? " $+ " : " $- ") << ((disp >= 0) ? disp + 2 : -(disp + 2)) << '\n';
    }

    void decodeFlagOps(u8 opcode, std::string mnemonic) {
        const char* flagOps[] = {"clc", "stc", "cli", "sti", "cld", "std"};

        pc++;
        std::cout << flagOps[opcode - 0xF8] << '\n';
    }

    void decodeRepPrefix(u8 opcode, std::string mnemonic) {
        u8 is_rep = opcode & 1;

        pc++;
        std::cout << ((is_rep) ? "rep " : "repne ");
    }

    void decodeNullaryInstruction(u8 opcode, std::string mnemonic) {std::cout << mnemonic << '\n'; pc++; }
    void decodeNullaryInstructionTwoBytes(u8 opcode, std::string mnemonic) {std::cout << mnemonic << '\n'; pc += 2; }
    void decodeSegmentPrefix(u8 opcode, std::string mnemonic) {this->ctx = mnemonic; pc++; }

    void decodeNullaryPrefix(u8 opcode, std::string mnemonic) {std::cout << mnemonic << " "; pc++; }

    std::string getRegister(u8 reg, u8 w) const {
        const char* regs8[] = {"al", "cl", "dl", "bl", "ah", "ch", "dh", "bh"};
        const char* regs16[] = {"ax", "cx", "dx", "bx", "sp", "bp", "si", "di"};

        return w ? regs16[reg] : regs8[reg];
    }

    std::string getRM(u8 mod, u8 rm, u8 w, bool include_size = false) {
        ContextGuard guard(this->ctx);

        if (mod == 3) {
            return this->getRegister(rm, w);
        }

        const char* addr[] = {"bx + si", "bx + di", "bp + si", "bp + di", "si", "di", "bp", "bx"};

        std::string size_prefix = include_size ? ((w) ? "word " : "byte ") : "";
        std::string segment_prefix = this->ctx;

        if (rm == 6 && mod == 0) {
            return size_prefix + segment_prefix + "[" + std::to_string(readU16()) + "]";
        }

        if (mod == 1) {
            return size_prefix + segment_prefix + "[" + std::string(addr[rm]) + formatDisplacement<i8>() + "]";
        } else if (mod == 2) {
            return size_prefix + segment_prefix + "[" + std::string(addr[rm]) + formatDisplacement<i16>() + "]";
        }

        return size_prefix + segment_prefix + "[" + std::string(addr[rm]) + "]";
    }

    std::string getSegReg(u8 segReg) {
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

