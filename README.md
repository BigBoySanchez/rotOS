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

## How to Run
Set your current directory to **rotOS**

Run the following in powershell:
```cmd
qemu-system-x86_64 os-image1.bin
```

## Expected Output
The emulator should display:
```
Welcome to rotOS!
System initialized successfully.
Terminal is ready.
```

## Notes
- I need to find a better way to cross-compile. It's too complicated to learn from scratch.
- It might be possible to build with x64 compiler anyway