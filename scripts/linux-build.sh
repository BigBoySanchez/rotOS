#!/bin/bash
# Exit on error
set -e

# Paths
TARGET="i686-elf"
MBR_SRC="mbr.asm"
KERNEL_SRC="kernel.c"
KERNEL_ENTRY_SRC="kernel_entry.asm"
BUILD_DIR="./build"
MBR_BIN="$BUILD_DIR/mbr.bin"
KERNEL_BIN="$BUILD_DIR/kernel.bin"
FINAL_BIN="$BUILD_DIR/../os-image.bin"

# Temporarily add bin folder to path
export PATH="./cross-tools/cross/bin:$PATH"

# Create build directory if it doesn't exist
mkdir -p "$BUILD_DIR"

# Assemble MBR
cd "./bootloader"
nasm -f bin "$MBR_SRC" -o "../$MBR_BIN"
cd ../

# Compile kernel_entry.asm to object file
nasm -f elf "$KERNEL_ENTRY_SRC" -o "$BUILD_DIR/kernel_entry.o"

# Compile kernel.c to object file
$TARGET-gcc -ffreestanding -c "$KERNEL_SRC" -o "$BUILD_DIR/kernel.o"

# Link kernel and kernel_entry to ELF
$TARGET-ld -Ttext 0x1000 --oformat binary "$BUILD_DIR/kernel_entry.o" "$BUILD_DIR/kernel.o" -o "$KERNEL_BIN"

# Concatenate MBR and kernel binary
cat "$MBR_BIN" "$KERNEL_BIN" > "$FINAL_BIN"

echo "Build complete: $FINAL_BIN"