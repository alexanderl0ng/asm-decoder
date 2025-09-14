# 8086 Decoder

A basic 8086 instruction decoder that converts machine code back to assembly.

## Current Support

- **Arithmetic Operations**     - ADD, SUB, ADC, SBB, INC, DEC, CMP, NEG, MUL, IMUL, DIV, IDIV

- **Logical Operations**        - AND, OR, XOR, NOT, TEST

- **Shift/Rotate Operations**   - ROL, ROR, RCL, RCR, SAL, SHR, SAR

- **Data Movement**             - MOV (all variants including segment registers), XCHG

- **Stack Operations**          - PUSH, POP (registers and segment registers), PUSHF, POPF

- **Control Flow**              - All conditional jumps (JO, JNO, JB, JNB, JE, JNE, JBE, JNBE, JS, JNS, JP, JNP, JL, JNL, JLE, JNLE)

- **Loop Instructions**         - LOOP, LOOPE, LOOPNE, JCXZ

- **Arithmetic Group**          - MUL, IMUL, DIV, IDIV

- **Flag Operations**           - CLC, STC, CLI, STI, CLD, STD, CMC, SAHF, LAHF

- **System Instructions**       - HLT, WAIT, CBW, CWD

- **Interrupt Instructions**    - INT, INT3, INTO, IRET

- **I/O Operations**            - IN, OUT

- **Special Instructions**      - DAA, DAS, AAA, AAS (ASCII/BCD adjust), LOCK prefix, REP/REPNE prefixes

## Building

Run the build script:
```bash
./build.sh
```
This creates the  `sim8086`  executable in the  `build/`  directory.

## Usage

Decode a binary file:
```bash
./build/sim8086 example
```

## Round-trip Assembly

You can verify the decoder by round-tripping assembly code:

1.  **Assemble**  your  `.asm`  file with NASM (or any assembler):
    ```bash
    nasm data/example.asm -o data/example
    ```

2.  **Decode**  the binary through sim8086:
    ```bash
    ./build/sim8086 data/example > decoded.asm
    ```

3.  **Reassemble**  the decoded output:
    ```bash
    nasm decoded.asm -o reassembled
    ```

4.  **Compare**  the original and reassembled binaries:
    ```bash
    diff data/example reassembled
    ```

If the decoder is working correctly, the files should be identical.

