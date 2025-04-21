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
INTERRUPT_DIR="./interrupt"
IDT_SRC="$INTERRUPT_DIR/idt.c"
ISR_SRC="$INTERRUPT_DIR/isr.c"
IRQ_SRC="$INTERRUPT_DIR/irq.c"
LIBC_DIR="./libc"
MEMSET_SRC="$LIBC_DIR/string/memset.c"
INTERRUPT_ASM="$INTERRUPT_DIR/interrupt_asm.s"
KERNEL_ELF="$BUILD_DIR/kernel.elf"


# Build flags
BUILD_FLAGS="-ffreestanding -Wall -Wextra -I. -g"

# Temporarily add bin folder to path
export PATH="./cross-tools/cross/bin:$PATH"

# Create build directory if it doesn't exist
mkdir -p "$BUILD_DIR"

# Compile kernel_entry.asm to object file
nasm -f elf "$KERNEL_ENTRY_SRC" -o "$BUILD_DIR/kernel_entry.o"

# Compile kernel.c to object file
$TARGET-gcc $BUILD_FLAGS -c "$KERNEL_SRC" -o "$BUILD_DIR/kernel.o"

# Compile memset.c to object file
$TARGET-gcc $BUILD_FLAGS -c "$MEMSET_SRC" -o "$BUILD_DIR/memset.o"

# Compile idt.c to object file
$TARGET-gcc $BUILD_FLAGS -c "$IDT_SRC" -o "$BUILD_DIR/idt.o"

# Compile isr.c to object file
$TARGET-gcc $BUILD_FLAGS -c "$ISR_SRC" -o "$BUILD_DIR/isr.o"

# Compile irq.c to object file
$TARGET-gcc $BUILD_FLAGS -c "$IRQ_SRC" -o "$BUILD_DIR/irq.o"

# Compile interrupt_asm.s to object file
nasm -f elf "$INTERRUPT_ASM" -o "$BUILD_DIR/interrupt_asm.o"

# Link kernel and kernel_entry to ELF file (with symbols)
$TARGET-ld -Ttext 0x1000 -o "$KERNEL_ELF" \
    "$BUILD_DIR/kernel_entry.o" \
    "$BUILD_DIR/kernel.o" \
    "$BUILD_DIR/memset.o" \
    "$BUILD_DIR/idt.o" \
    "$BUILD_DIR/interrupt_asm.o" \
    "$BUILD_DIR/isr.o" \
    "$BUILD_DIR/irq.o"

# Extract raw binary from ELF
$TARGET-objcopy -O binary "$KERNEL_ELF" "$KERNEL_BIN"

# Calculate how many 512‑byte sectors the kernel occupies
KERNEL_SIZE=$(stat -c%s "$KERNEL_BIN")
KERNEL_SECTORS=$(( (KERNEL_SIZE + 511) / 512 ))
echo "Kernel size: $KERNEL_SIZE bytes ($KERNEL_SECTORS sectors)"

# Assemble MBR with kernel sector count
cd "./bootloader"
nasm -f bin -DKERNEL_SECTORS=$KERNEL_SECTORS "$MBR_SRC" -o "../$MBR_BIN"
cd ../

# Concatenate MBR and kernel binary
cat "$MBR_BIN" "$KERNEL_BIN" > "$FINAL_BIN"

echo "Build complete: $FINAL_BIN"