; --- BUBBLE SORT ---

        ldc N
        ldnl 0
        adc -1
        ldc i
        stnl 0           ; i = N - 1

outerloop:
        ldc i
        ldnl 0
        brlz end         
        brz end          

        ldc 0
        ldc j
        stnl 0           ; j = 0 

innerloop:
        ldc i
        ldnl 0
        ldc j
        ldnl 0
        sub
        brlz endinner    
        brz endinner

        ; --- Calculate Address of arr[j] ---
        ldc arr          ; Get base address
        ldc j
        ldnl 0           ; Get current index value
        add              ; Target address = base + j
        ldc addr
        stnl 0           ; Store this address in our variable 'addr'

        ; --- Load arr[j] and arr[j+1] ---
        ldc addr
        ldnl 0           ; Get the address we just stored
        ldnl 0           ; Load the value at that address (arr[j])
        ldc temp1
        stnl 0

        ldc addr
        ldnl 0           ; Get the address again
        ldnl 1           ; Load value at address + 1 (arr[j+1])
        ldc temp2
        stnl 0

        ; --- Comparison ---
        ldc temp2
        ldnl 0
        ldc temp1
        ldnl 0
        sub              ; temp2 - temp1
        brlz swap        
        br noswap

swap:
        ldc temp2
        ldnl 0           ; Get value of temp2
        ldc addr
        ldnl 0           ; Get address of arr[j]
        stnl 0           ; arr[j] = temp2

        ldc temp1
        ldnl 0           ; Get value of temp1
        ldc addr
        ldnl 0           ; Get address of arr[j]
        stnl 1           ; arr[j+1] = temp1

noswap:
        ldc j
        ldnl 0
        adc 1            
        ldc j
        stnl 0
        br innerloop

endinner:
        ldc i
        ldnl 0
        adc -1           
        ldc i
        stnl 0
        br outerloop

end:
        HALT

; --- DATA ---
N:      data 5
i:      data 0
j:      data 0
addr:   data 0           ; Now used correctly with ldc/stnl
temp1:  data 0
temp2:  data 0
arr:    data 56
        data 12
        data 67
        data 7
        data 40