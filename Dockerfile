# Dockerfile for building and testing your OS kernel project
FROM ubuntu:22.04

# Install dependencies
RUN apt-get update && \
    DEBIAN_FRONTEND=noninteractive apt-get install -y \
    build-essential \
    nasm \
    qemu-system-x86 \
    xorriso \
    grub-pc-bin \
    mtools \
    && rm -rf /var/lib/apt/lists/*

# Set the working directory inside the container
WORKDIR /usr/src/app

# Copy all project files into the container
COPY . .

# Default command: open a shell (so you can run make, etc.)
CMD ["/bin/bash"]
