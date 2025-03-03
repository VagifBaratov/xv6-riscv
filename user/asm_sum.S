.section .data
buffer:     .space  25        
fmt_output: .asciz  "|%s|%d\n"
err_format: .asciz  "Ошибка: некорректный формат\n"
err_overflow: .asciz "Ошибка: слишком большие числа\n"

    .section .text
    .global main
    .extern gets
    .extern printf
    .extern fprintf
    .extern atoi

main:
    la a0, buffer
    la a1, 25
    call gets              
    la a0, buffer

replace_newline:
    lb t0, 0(a0)           
    li t1, '\n'   
    beq t0, t1, replace    
    addi a0, a0, 1         
    j replace_newline      

replace:
    sb zero, 0(a0)    

check_first_char:
    la a0, buffer          
    lb t0, 0(a0)           
    li t1, ' '             
    beq t0, t1, format_error  
    beqz t0, format_error

    la a1, buffer
    li a2, 0
    li a3, 11

find_space:
    lb t0, 0(a1)          
    beqz t0, format_error   
    li t2, ' '
    beq t0, t2, check_single_space 
    addi a1, a1, 1
    addi a2, a2, 1
    beq a2, a3, overflow_error
    j find_space

check_single_space:
    addi a1, a1, 1        
    lb t0, 0(a1)          
    beqz t0, format_error 
    li t1, ' '
    beq t0, t1, format_error
    
    mv a2, a1             
    li a4, 0
find_end:
    lb t0, 0(a2)          
    beqz t0, parse_numbers  
    li t1, ' '            
    beq t0, t1, format_error  
    addi a2, a2, 1
    addi a4, a4, 1
    beq a4, a3, overflow_error
    j find_end  

parse_numbers:          
    la a0, buffer
    call atoi
    mv s1, a0              

    mv a0, a1
    call atoi
    mv s2, a0            

    add s3, s1, s2

    la a0, fmt_output
    la a1, buffer

    mv a2, s3
    call printf

    li a0, 0
    call exit

format_error:
    la a0, 2
    la a1, err_format     
    call fprintf   
    
    li a0, 1
    call exit

overflow_error:
    la a0, 2
    la a1, err_overflow
    call fprintf

    li a0, 1
    call exit
