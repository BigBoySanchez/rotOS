# rotOS

# Barebones Kernel (4/16)

## Changes
- Implemented barebones kernel with VGA text output
- Added basic terminal functionality

## Needs
Kernel should look like this:
```
function kernel_main():
    initialize_screen()
    print("Hello from the kernel!")

    initialize_memory()
    initialize_interrupts()

    while true:
        handle_input_if_any()
        run_scheduler_if_tasks_exist()
```

<<<<<<< HEAD
## How to Run
Start **WSL**
=======
## How to Build and Run (with Docker)

### On macOS

1. **Build the Docker image:**
   Open Terminal in the `rotOS` directory and run:
   ```sh
   docker build -t rotos-dev .
   ```

2. **Start the Docker container:**
   Mount your project directory so changes persist:
   ```sh
   docker run --rm -it -v "$PWD:/usr/src/app" rotos-dev
   ```
   > **Note:**
   > - `$PWD` works natively on macOS (and Linux).
   > - The first time you run Docker, you may need to grant file sharing access to your project directory in Docker Desktop settings.

3. **Build your OS inside the container:**
   Run `make` to build the project using the provided Makefile:
```sh
make
```

4. **Run in QEMU:**
   After building, you can run your kernel image with:
   ```sh
   qemu-system-x86_64 os-image1.bin
   ```

---


1. **Build the Docker image:**
   Open PowerShell in the `rotOS` directory and run:
   ```sh
   docker build -t rotos-dev .
   ```

2. **Start the Docker container:**
   This mounts your project so changes persist:
   ```sh
   docker run --rm -it -v ${PWD}:/usr/src/app rotos-dev
   ```
   (If `${PWD}` doesn't work, use `%cd%` on Windows.)

3. **Build your OS inside the container:**
   Run `make` to build the project using the provided Makefile:
```sh
make
```

4. **Run in QEMU:**
   After building, you can run your kernel image with:
   ```sh
   qemu-system-x86_64 os-image1.bin
   ```

---

## Makefile Targets

- `make` (default): Builds the kernel, bootloader, and creates `os-image1.bin`.
- `make run`: Builds and runs your OS in QEMU.
- `make clean`: Removes build artifacts.

## How to Run (without Docker)
>>>>>>> Dockerfile
Set your current directory to **rotOS**

Run the following:
```bash
# Make scripts executable
sudo chmod +x ./scripts/*.sh

sudo ./scripts/linux-tools.sh
sudo ./scripts/linux-build.sh

# Run OS in qemu
cd build
qemu-system-x86_64 os-image.bin
```

## Expected Output
The emulator should display:
```
Welcome to rotOS!
System initialized successfully.
Terminal is ready.
```

## Notes
