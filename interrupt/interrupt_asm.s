; interrupt/interrupt_asm.s
; Assembly routines for interrupt handling (NASM syntax)

extern isr_handler ; Defined in isr.c
extern irq_handler ; Defined in irq.c (will be created)

section .text
global idt_load   ; Export idt_load for C code
global isr0       ; Export ISR stubs
global isr1
global isr2
global isr3
global isr4
global isr5
global isr6
global isr7
global isr8
global isr9
global isr10
global isr11
global isr12
global isr13
global isr14
global isr15
global isr16
global isr17
global isr18
global isr19
global isr20
global isr21
global isr22
global isr23
global isr24
global isr25
global isr26
global isr27
global isr28
global isr29
global isr30
global isr31

global irq0       ; Export IRQ stubs (for IDT 32-47)
global irq1
global irq2
global irq3
global irq4
global irq5
global irq6
global irq7
global irq8
global irq9
global irq10
global irq11
global irq12
global irq13
global irq14
global irq15

; Loads the IDT pointer into the processor's IDTR register
idt_load:
    mov eax, [esp+4] ; Get the pointer argument (address of idt_ptr_t)
    lidt [eax]       ; Load IDT register
    ret

; Common ISR stub called by individual ISRs
isr_common_stub:
    pusha          ; Push edi,esi,ebp,esp,ebx,edx,ecx,eax
    mov ax, ds     ; Lower 16 bits of ds register
    push eax       ; Save the data segment descriptor

    mov ax, 0x10   ; Load the kernel data segment descriptor (adjust if different)
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    call isr_handler ; Call the C handler

    pop eax        ; Restore original data segment descriptor
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    popa           ; Pop edi,esi,ebp,esp,ebx,edx,ecx,eax
    add esp, 8     ; Clean up the pushed error code and ISR number
    iret           ; Pop cs, eip, eflags, ss, esp

; Common IRQ stub called by individual IRQs
irq_common_stub:
    pusha          ; Push edi,esi,ebp,esp,ebx,edx,ecx,eax
    mov ax, ds     ; Lower 16 bits of ds register
    push eax       ; Save the data segment descriptor

    mov ax, 0x10   ; Load the kernel data segment descriptor (adjust if different)
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    ; The C handler expects a pointer to the registers_t struct
    ; EAX currently holds the original DS value, ESP points to it.
    ; The full structure starts just above the pushed DS.
    mov eax, esp   ; Get pointer to the location of the pushed DS
    ; eax already points to start of registers_t (saved DS)
    push eax       ; Push pointer to registers_t struct for C handler

    call irq_handler ; Call the C IRQ handler

    pop eax        ; Pop the registers_t pointer argument

    ; C handler must have sent EOI

    pop eax        ; Restore original data segment descriptor
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    popa           ; Pop edi,esi,ebp,esp,ebx,edx,ecx,eax
    add esp, 8     ; Clean up the pushed interrupt number and error code (0 in this case)
    iret           ; Pop cs, eip, eflags, ss, esp


; Macro to generate ISR stubs
; %1: ISR number
; %2: Error code flag (1 if CPU pushes error code, 0 otherwise)
%macro ISR_STUB 2
isr%1:
  %if %2 == 0
    push dword 0   ; Push dummy error code if none provided by CPU
  %endif
    push dword %1  ; Push the interrupt number
    jmp isr_common_stub
%endmacro

; Generate ISR stubs (CPU Exceptions 0-31)
; Format: ISR_STUB <interrupt_number>, <has_error_code>
ISR_STUB 0, 0   ; Divide by zero
ISR_STUB 1, 0   ; Debug
ISR_STUB 2, 0   ; NMI
ISR_STUB 3, 0   ; Breakpoint
ISR_STUB 4, 0   ; Overflow
ISR_STUB 5, 0   ; Bound range exceeded
ISR_STUB 6, 0   ; Invalid opcode
ISR_STUB 7, 0   ; Device not available
ISR_STUB 8, 1   ; Double fault (has error code)
ISR_STUB 9, 0   ; Coprocessor segment overrun (legacy)
ISR_STUB 10, 1  ; Invalid TSS (has error code)
ISR_STUB 11, 1  ; Segment not present (has error code)
ISR_STUB 12, 1  ; Stack-segment fault (has error code)
ISR_STUB 13, 1  ; General protection fault (has error code)
ISR_STUB 14, 1  ; Page fault (has error code)
ISR_STUB 15, 0  ; Reserved
ISR_STUB 16, 0  ; x87 FPU error
ISR_STUB 17, 1  ; Alignment check (has error code)
ISR_STUB 18, 0  ; Machine check
ISR_STUB 19, 0  ; SIMD Floating-Point Exception
ISR_STUB 20, 0  ; Virtualization Exception
ISR_STUB 21, 1  ; Control Protection Exception (has error code)
; ISRs 22-27 are reserved
ISR_STUB 22, 0
ISR_STUB 23, 0
ISR_STUB 24, 0
ISR_STUB 25, 0
ISR_STUB 26, 0
ISR_STUB 27, 0
ISR_STUB 28, 0  ; Hypervisor Injection Exception
ISR_STUB 29, 1  ; VMM Communication Exception (has error code)
ISR_STUB 30, 1  ; Security Exception (has error code)
ISR_STUB 31, 0  ; Reserved

; Macro to generate IRQ stubs
; %1: IRQ number (0-15)
; Note: IRQs do not push an error code
%macro IRQ_STUB 1
irq%1:
    push dword 0   ; Push dummy error code (for stack alignment with ISRs)
    push dword %1 + 32 ; Push the interrupt number (IRQ 0 is INT 32)
    jmp irq_common_stub
%endmacro

; Generate IRQ stubs (Hardware Interrupts 0-15 -> Remapped to 32-47)
IRQ_STUB 0    ; Timer
IRQ_STUB 1    ; Keyboard
IRQ_STUB 2    ; Cascade (Slave PIC)
IRQ_STUB 3    ; COM2
IRQ_STUB 4    ; COM1
IRQ_STUB 5    ; LPT2
IRQ_STUB 6    ; Floppy Disk
IRQ_STUB 7    ; LPT1 / Spurious
IRQ_STUB 8    ; CMOS Real-time Clock
IRQ_STUB 9    ; Free / ACPI / SCSI
IRQ_STUB 10   ; Free / SCSI / NIC
IRQ_STUB 11   ; Free / SCSI / NIC
IRQ_STUB 12   ; PS/2 Mouse
IRQ_STUB 13   ; FPU / Coprocessor
IRQ_STUB 14   ; Primary ATA Hard Disk
IRQ_STUB 15   ; Secondary ATA Hard Disk
