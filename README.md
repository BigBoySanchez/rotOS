# rotOS

# Barebones Kernel (4/16)

## Features & Recent Changes

### Kernel & Terminal
- VGA text output with colored text, scrolling, and basic terminal emulation.
- `term_putchar` now uses a `switch` statement for extensible control character handling (newline, backspace, etc).
- Backspace (`\b`) support: erases the previous character on screen.
- Easily extensible for more control characters (tab, carriage return, etc).

### Interrupts & IRQs
- Full IDT setup (`idt_install`) and PIC remapping.
- Clean separation of ISRs (CPU exceptions) and IRQs (hardware interrupts).
- IRQ handlers can be registered per line; unhandled IRQs print their number in decimal.
- Assembly stubs fixed to pass correct register state to C handlers.

### Drivers
- **Timer:** PIT initialized to 100 Hz; handler increments a tick counter (ready for scheduling).
- **Keyboard:** Basic US QWERTY layout, prints characters to terminal, supports Enter, Backspace, Tab. Keyboard buffer can be extended for line editing.

### Build System
- `scripts/linux-build.sh` compiles all drivers, kernel, and interrupt code, links to ELF, and produces a bootable image.
- Automatically calculates kernel size for MBR.

### Bug Fixes
- Fixed IRQ handler pointer bug: now receives correct IRQ numbers (0-15).
- Backspace no longer prints a dot; instead, it erases as expected.
- Improved error messages for unhandled IRQs.

---

## How to Build and Run
1. **Set your current directory to `rotOS`**

2. **Make scripts executable:**
```bash
sudo chmod +x ./scripts/*.sh
```

3. **Install cross tools (if needed):**
```bash
sudo ./scripts/linux-tools.sh
```

4. **Build the OS:**
```bash
sudo ./scripts/linux-build.sh
```

5. **Run in QEMU:**
```bash
qemu-system-x86_64 os-image.bin
```

---

## Expected Output
The emulator should display:
```
Welcome to rotOS!
System initialized successfully.
Terminal is ready.
Memory initialized.
Interrupts installed.
Timer initialized (100 Hz).
Keyboard initialized.
Interrupts enabled. Type something!
```
You should be able to type on the keyboard and see characters echo, with working backspace.

---

## Directory Structure
- `kernel.c`         — Kernel entry, terminal, and core logic
- `kernel_entry.asm` - Kernel entry point (assembly)
- `drivers/`         — Keyboard and timer drivers
- `interrupt/`       — IDT, ISR, IRQ, and low-level interrupt logic
- `scripts/`         — Build scripts
- `bootloader/`      — MBR and boot sector code
- `build/`           — Output binaries (after build)

---

## Notes
- The kernel is modular and ready for further extension: paging, scheduler, advanced terminal, etc.
- For debugging, unhandled IRQs will print their IRQ number.
- See code comments for extension points and TODOs.
