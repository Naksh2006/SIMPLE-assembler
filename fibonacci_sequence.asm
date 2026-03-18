start:
    ldc nval
    ldnl 0
    brz returna      ; If n=0, result is vara (0)
    
    ldc 1
    sub
    brz returnb      ; If n=1, result is varb (1)

loop:
    ; to Calculate next Fibonacci 
    ldc vara
    ldnl 0
    ldc varb
    ldnl 0
    add
    ldc temp
    stnl 0

    ; Shift values (vara = varb, varb = temp) 
    ldc varb
    ldnl 0
    ldc vara
    stnl 0

    ldc temp
    ldnl 0
    ldc varb
    stnl 0

    ;  Decrement nval
    ldc nval
    ldnl 0           ; Load current n
    ldc 1
    sub              ; Subtract 1
    ldc nval
    stnl 0           ; Update nval in memory

    ; The Fix: Explicitly check nval for exit 
    ldc nval         ; Reload the updated nval
    ldnl 0
    ldc 1            ; We check against 1 because we handled n=1 at start
    sub              ; If (n-1) == 0, we are done
    brz returnb
    
    br loop          ; Otherwise, keep looping

returna:
    ldc vara
    ldnl 0
    ldc result
    stnl 0
    HALT

returnb:
    ldc varb
    ldnl 0
    ldc result
    stnl 0
    HALT

; --- Data ---
nval:   data 6       ; Finding the 6th Fibonacci number
vara:   data 0
varb:   data 1
temp:   data 0
result: data 0