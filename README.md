# 8086 Decoder

A basic 8086 instruction decoder that converts machine code back to assembly.

## Current Support

-   **MOV instruction**     - Register/memory to/from register
                            - Immediate to register
                            - Immediate to memory
                            - Memory/accumulator to accumulator/memory

-   **ADD instruction**     - Register/memory with register to either
                            - Immediate to register/memory
                            - Immediate to accumulator

-   **SUB instruction**     - Register/memory and register to either
                            - Immediate from register/memory
                            - Immediate from accumulator

-   **CMP instruction**     - Register/memory and register
                            - Immediate with register/memory
                            - Immediate with accumulator

## Building

Run the build script:
```bash
./build.sh
```
This creates the  `sim8086`  executable in the  `build/`  directory.

## Usage

Decode a binary file:
```bash
./build/sim8086 data/test_0038
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

## Test Data

The  `data/`  directory contains sample assembly files and their assembled binaries for testing.
