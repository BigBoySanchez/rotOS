; args: ax = hex number to print
print_hex:
    pusha
    mov bx, ax
    mov ah, 0x0e    ; tty code
    xor cl, cl      ; hold number of loops

hex_loop:
    cmp cl, 4
    je  hex_end

    mov al, bh
    shl bx, 4       ; kick high 4 bits from bx
    shr al, 4       ; kick the low 4 bits from al

    cmp al, 10      ; hex letters start at 10
    jl  print_char
    add al, 'a'     ; add 'a' to al instead of '0'
    sub al, 10      ; offset so 10 = 'a'
    sub al, '0'     ; b/c print_char adds '0' anyway

print_char:
    add al, '0'     ; number values get added here
    int 0x10        ; print al

    inc cl
    jmp hex_loop

hex_end:  
    popa
    ret