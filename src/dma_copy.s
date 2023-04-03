; ---------------------------------
; Function dma_copy
; Parameters:
;   - source address in HL
;   - destination address in DE
;   - length in BC
; ---------------------------------
;_dma_copy::
;    push af ; Save AF to the stack
;    push bc ; Save BC to the stack
;
;    ; Set DMG DMA transfer mode
;    ld a, 0x01
;    ldh (0xFF46), a
;
;    ; Set DMG DMA base address
;    ld a, 0x28
;    ldh (0xFF51), a
;
;    ; Set DMG DMA trigger
;    ld a, 0x00
;    ldh (0xFF52), a
;
;    ; Enable DMG DMA transfer
;    ld a, 0x80
;    ldh (0xFF53), a
;    ldh (0xFF54), a
;
;    ; Set DMG DMA transfer length
;    pop bc ; Restore BC from the stack
;    dec bc ; Decrement BC to account for 0-based length
;    ld a, b ; Load B (upper byte of BC) into A
;    or a, a ; Check if A is zero
;    jr z, dma_set_length ; If A is zero, jump to dma_set_length
;    dec a ; Decrement A to account for 0-based length
;
;dma_set_length:
;    ldh (0xFF55), a ; Write length to HDMA5
;
;    ; Perform the DMA transfer
;    ld a, h
;    ldh (0xFF51), a
;    ld a, l
;    ldh (0xFF52), a
;    ld a, d
;    ldh (0xFF53), a
;    ld a, e
;    ldh (0xFF54), a
;
;    pop af ; Restore AF from the stack
;    ret
;