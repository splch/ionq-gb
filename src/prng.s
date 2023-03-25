; ---------------------------------
; Function rand
; ---------------------------------

_rand::
    ld  c, d          ; Load the higher byte of the 16-bit seed into register C
    ld  a, e          ; Load the lower byte of the 16-bit seed into register A
    res 0, c          ; Clear the least significant bit of register C
    or  a, c          ; Perform bitwise OR between registers A and C, and store the result in A
    ld  c, a          ; Store the result from register A back into C (the RAND state)
    and a, #0x01      ; Perform bitwise AND between register A and 1, to isolate the least significant bit
    sra c             ; Shift register C right arithmetically (divide by 2)
    or  a, a          ; Perform bitwise OR between register A and itself, to set the zero flag based on the least significant bit
    jr  Z, .no_xor    ; If the least significant bit was 0, do not bitwise XOR
    ld  a, c          ; Load the value from register C into A
    xor a, #0xb8      ; Perform bitwise XOR between register A and the polynomial 0xB8
    ld  c, a          ; Store the result from register A back into C (the updated RAND state)

.no_xor:              ; Label no_xor
    ld  a, c          ; Load the RAND state from register C into A
    ret               ; Return the RAND state in register A
