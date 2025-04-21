[org 0x7C00]                ; BIOS loads boot sector to this address
mov sp, 0x7C00              ; Temporary stack
KERNEL_OFFSET equ 0x1000

; ========================
; Start in 16-bit real mode
; ========================
[bits 16]
main:
    ; print 16-bit message
    mov ax, msg16
    call print
    call print_nl
    cli                 ; Disable interrupts
    xor ax, ax          ; Clear segment registers
    mov ds, ax
    mov es, ax
    mov ss, ax

    mov ax, msg_boot
    call print
    call print_nl

    ; load kernel from boot sector
    mov bx, KERNEL_OFFSET ; Read from disk and store in 0x1000
    mov dh, 2
    mov dl, [BOOT_DRIVE]
    call disk_load

    call enable_a20     ; Enable access above 1MB
    call setup_gdt      ; Load GDT

    ; Set PE (Protection Enable) bit in CR0
    mov eax, cr0
    or eax, 1
    mov cr0, eax

    ; Far jump to protected mode code
    jmp 0x08:protected_mode_start

; ========================
; Protected Mode Code
; ========================
[bits 32]
protected_mode_start:
    ; Set up segments with flat model
    mov ax, 0x10        ; Data segment selector
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    mov esp, 0x9FBFF    ; Setup stack

    ; print 32-bit message
    mov ebx, msg32
    call print_string_pm

    ; Enter kernel
    call KERNEL_OFFSET
    jmp $

; ========================
; GDT Setup
; ========================
[bits 16]
setup_gdt:
    lgdt [gdt_descriptor]
    ret

; ========================
; A20 Line Enabler (simple BIOS method)
; ========================
enable_a20:
    in al, 0x92         ; Read System Control Port A
    or al, 00000010b    ; Set A20 bit (bit 1; expands accessible memory)
    out 0x92, al
    ret

; ========================
; GDT (Global Descriptor Table)
; ========================
gdt_start:
    ; Null descriptor
    dq 0x0000000000000000

    ; Code segment: base=0, limit=4GB, flags=0x9A
    dq 0x00CF9A000000FFFF

    ; Data segment: base=0, limit=4GB, flags=0x92
    dq 0x00CF92000000FFFF

gdt_descriptor:
    dw gdt_end - gdt_start - 1      ; Size (limit)
    dd gdt_start                    ; Address (base)

gdt_end:


%include './boot_sect.asm'
BOOT_DRIVE db 0x80

; ========================
; Strings, for debugging
; ========================
%include './print.asm'
%include './print_hex.asm'
%include './print32.asm'

msg16:
    db 'wsg 16-bit', 0
msg32:
    db 'wagwan 32-bit', 0
msg_boot:
    db 'mi get dat bombaclaat kernel v0.1', 0

; ========================
; Boot Signature (MUST be last 2 bytes)
; ========================
times 510 - ($ - $$) db 0
dw 0xAA55
