; error_test_v2.asm
; Program intentionally contains errors

init:
init:           ; duplicate label

adc             ; missing operand

sub 15          ; unexpected operand (sub takes no operand)

ldl foo         ; undefined label

brz ghost_loc   ; label does not exist

ldc 0xG77       ; invalid number format

4error:         ; invalid label name

HALT