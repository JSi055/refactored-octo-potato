; EE2361 - Lab2
; Thomas Lonneman <lonne025@umn.edu>

; .include "xc.inc"          ; required "boiler-plate" (BP)

.text           ; BP (put the following data in ROM(program memory))

.global __delay_half_us
.global __delay_us
.global _wait_1ms
.global _wait_100us
.global _waitMS

__delay_half_us:      ; 2
    repeat #(8-7)     ; 1    (2+1+1+3 = 7)
    nop               ; 1
    return            ; 3
    
_wait_100us:          ; 2
    repeat #(1600-7)  ; 1    (2+1+1+3 = 7)
    nop               ; 1
    return            ; 3


_wait_1ms:            ; 2
    repeat #(16000-7) ; 1    (2+1+1+3 = 7)
    nop               ; 1
    return            ; 3

pre_waitMS: ; hehehe, jump to before the function referance! :D
    repeat #(5-2)       ; 1
    nop                 ; 1
_waitMS:                ; 2*
    dec w0, w0          ; 1
    repeat #(16000-5-5) ; 1    (1+1+1+2) + (2+3)
    nop                 ; 1
    bra NZ, pre_waitMS  ; 2
    
    return              ; 3*

; just like the waitMS function, but with 16 cycles instead of 16000
pre_delay_us: ; hehehe, jump to before the function referance! :D
    repeat #(5-2)       ; 1
    nop                 ; 1
__delay_us:             ; 2*
    dec w0, w0          ; 1
    repeat #(16-5-5)    ; 1    (1+1+1+2) + (2+3)
    nop                 ; 1
    bra NZ, pre_delay_us; 2
    
    return              ; 3*

.end    ; this doesn?t actually end anything. Does not translate to assembly
        ; code. Just a way to tell the compiler we are done with this file.

