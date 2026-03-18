; Find the Maximum of array
start:
    ldc arrbase                                         ; load firt elementof array
    ldnl 0
    ldc maxval                                      ; Set maxval = array[0]
    stnl 0

    ldc 1                                               ; Set ptrval = 1
    ldc ptrval                                      ; (We start checking from the second element)
    stnl 0

    ldc arrlen                                          ; Get the array length
    ldnl 0
    ldc 1                                               ; Subtract 1 because we already processed the first item
    sub
    ldc arrlen                                       ; Store the remaining count back in arrlen
    stnl 0

loop:
    ldc arrlen                                       ; Load remaining items to check
    ldnl 0
    brz done                                           ; If count is 0, jump to the end

             ; to Fetch current element 
    ldc arrbase                                     ; Base address of array
    ldc ptrval                                          ; Current offset (index)
    ldnl 0
    add                                             ; Calculate address: arrbase + ptrval
    ldnl 0                                               ; Load value at that address
    ldc curval                                      ; Store it in curval
    stnl 0

             ; Compare: (curval - maxval)
    ldc curval       
    ldnl 0
    ldc maxval       
    ldnl 0
    sub                                             ; result = current - max
    brlz skip                                           ; If result < 0 (current is smaller), don't update

                    ;  Update Max 
    ldc curval                                      ; If we reached here, current is larger
    ldnl 0
    ldc maxval                                          ; Update maxval with the new high score
    stnl 0

skip:
                                ;  Increment Pointers & Decrement Counter
    ldc ptrval       
    ldnl 0
    ldc 1
    add              ; ptrval++ (move to next index)
    ldc ptrval       
    stnl 0

    ldc arrlen       
    ldnl 0
    ldc 1
    sub              ; arrlen-- (one less item to check)
    ldc arrlen       
    stnl 0
    br loop          ; Repeat the loop

done:
    HALT             ; Stop execution

    ; Data Section 
arrbase: data 12     ; The array of numbers
         data 45
         data 7
         data 89
         data 23
arrlen: data 5       ; Number of elements
maxval: data 0       ; Will hold the final maximum (89)
curval: data 0       ; Temporary storage for current comparison
ptrval: data 0       ; Current array index/offset