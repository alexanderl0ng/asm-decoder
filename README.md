# 8086 Decoder

A complete 8086 instruction decoder that converts machine code to assembly.

## Build & Run

Run the build script:
```bash
./build.sh
./build/sim8086 binary_file
```

## Round-trip Verification
```bash
# Assemble your code
nasm program.asm -o program

# Decode it
./build/sim8086 program > decoded.asm

# Reassemble and verify
nasm decoded.asm -o reassembled
diff program reassembled # Should be identical
```
Perfect round-trip compatibility ensures the decoder output can be reassembled to identical machine code.

