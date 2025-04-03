; args: ax = base of str
print:
    pusha 
    mov si, ax      ; lodsb gets baseptr from si
    mov ah, 0x0e    ; tty code
    
print_loop:
    lodsb           ; byte now in al, si moves to next byte
    cmp al, 0
    je  loop_end
    int 0x10        ; print al
    jmp print_loop

loop_end:  
    popa
    ret

; args: none
print_nl:
    pusha
    mov ah, 0x0e    ; tty code

    mov al, 0x0a    ; print '\n'
    int 0x10
    mov al, 0x0d    ; print '\r'
    int 0x10
   
    popa
    ret