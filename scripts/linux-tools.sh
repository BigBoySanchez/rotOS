#!/bin/bash
# Exit immediately if a command exits with a non-zero status.
set -e

# ===============================
# 1. Update System and Install Prerequisites
# ===============================
echo "Updating package list and installing prerequisites..."
apt-get update
apt-get install -y \
    gcc \
    g++ \
    build-essential \
    bison \
    flex \
    libgmp3-dev \
    libmpfr-dev \
    libmpc-dev \
    texinfo \
    curl \
    wget \
    git \

# ===============================
# 2. Define Variables and Directories
# ===============================
# Customize these variables as needed.
binutilsVersion="2.42"      # Example binutils version
gccVersion="12.2.0"         # Example GCC version


# ARGS
cwd=$(realpath .)

# Define directory paths in your WSL home.
CROSS_DIR="$cwd/cross-tools"
SRC_DIR="$CROSS_DIR/src"
BUILD_BINUTILS="$CROSS_DIR/build-binutils"
BUILD_GCC="$CROSS_DIR/build-gcc"
INSTALL_DIR="$CROSS_DIR/cross"

# Define build variables
export PREFIX="$INSTALL_DIR"
export TARGET="i686-elf"
export PATH="$PREFIX/bin:$PATH"

# Create necessary directories if they don't exist.
echo "Creating directories..."
mkdir -p "$SRC_DIR" "$BUILD_BINUTILS" "$BUILD_GCC" "$INSTALL_DIR"
ls -la "$CROSS_DIR"

# ===============================
# 3. Download and Extract Sources
# ===============================
echo "Changing to source directory: $SRC_DIR"
cd "$SRC_DIR"

# Download binutils source if not already present.
if [ ! -f "binutils-$binutilsVersion.tar.gz" ]; then
    echo "Downloading binutils-$binutilsVersion..."
    wget https://ftp.gnu.org/gnu/binutils/binutils-$binutilsVersion.tar.gz
else
    echo "Binutils-$binutilsVersion.tar.gz already exists."
fi

# Download GCC source if not already present.
if [ ! -f "gcc-$gccVersion.tar.gz" ]; then
    echo "Downloading gcc-$gccVersion..."
    wget https://ftp.gnu.org/gnu/gcc/gcc-$gccVersion/gcc-$gccVersion.tar.gz
else
    echo "GCC-$gccVersion.tar.gz already exists."
fi

# Extract binutils if not already extracted.
if [ ! -d "binutils-$binutilsVersion" ]; then
    echo "Extracting binutils-$binutilsVersion..."
    tar -xf binutils-$binutilsVersion.tar.gz
else
    echo "binutils-$binutilsVersion directory already exists."
fi

# Extract GCC if not already extracted.
if [ ! -d "gcc-$gccVersion" ]; then
    echo "Extracting gcc-$gccVersion..."
    tar -xf gcc-$gccVersion.tar.gz
else
    echo "gcc-$gccVersion directory already exists."
fi

# ===============================
# 4. Build and Install Binutils
# ===============================
echo "Building Binutils for target $TARGET..."
cd "$BUILD_BINUTILS"
echo "Configuring binutils..."
../src/binutils-$binutilsVersion/configure \
    --target="$TARGET" \
    --prefix="$INSTALL_DIR" \
    --with-sysroot \
    --disable-nls \
    --disable-werror \

echo "Compiling binutils..."
make -j$(nproc)

echo "Installing binutils..."
make install

# ===============================
# 5. Build and Install GCC (Stage 1: C Compiler)
# ===============================
echo "Building minimal GCC (C compiler only) for target $TARGET..."
cd "$BUILD_GCC"
echo "Configuring gcc..."
../src/gcc-$gccVersion/configure \
    --target=$TARGET \
    --prefix="$INSTALL_DIR" \
    --disable-nls \
    --enable-languages=c,c++ \
    --without-headers \
    --disable-hosted-libstdcxx \

make all-gcc -j$(nproc)
make all-target-libgcc -j$(nproc)
make all-target-libstdc++-v3 -j$(nproc)

make install-gcc
make install-target-libgcc
make install-target-libstdc++-v3

# ===============================
# 6. Final Instructions
# ===============================
echo "========================================"
echo "Cross-compiler build complete!"
echo "The cross-compiler has been installed in: $INSTALL_DIR"
echo "========================================"
