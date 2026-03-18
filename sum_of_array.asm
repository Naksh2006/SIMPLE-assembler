start:
    ldc 0            
    ldc sumval       
    stnl 0           ; sum = 0

    ldc 0            
    ldc ptrval       
    stnl 0           ; index = 0

loop:
    ldc arrlen       
    ldnl 0           
    brz done         ; exit if counter == 0

    ldc arrbase      
    ldc ptrval       
    ldnl 0           
    add              ; calculate address
    ldnl 0           ; load array[index]

    ldc sumval       
    ldnl 0           
    add              ; sum + array[index]
    ldc sumval       
    stnl 0           ; update sum

    ldc ptrval       
    ldnl 0           
    ldc 1            
    add              
    ldc ptrval       
    stnl 0           ; index++

    ldc arrlen       
    ldnl 0           
    ldc 1            
    sub              
    ldc arrlen       
    stnl 0           ; counter--
    
    br loop          

done:
    ldc sumval       
    ldnl 0           
    ldc result       
    stnl 0           ; store final sum
    HALT

arrbase: data 10     
         data 20     
         data 30     
         data 40     
arrlen:  data 4      
sumval:  data 0      
ptrval:  data 0      
result:  data 0