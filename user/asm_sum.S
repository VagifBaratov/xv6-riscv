.section .data
buffer:     .space  24        
fmt_output: .asciz  "|%s|%d\n"
err_format: .asciz  "Ошибка: некорректный формат\n"
err_overflow: .asciz "Ошибка: слишком длинная строка\n"

    .section .text
    .global main
    .extern gets
    .extern printf
    .extern atoi

main:
    # Вызов gets
    la a0, buffer
    call gets              

    la a0, buffer
    call strlen        
    li t1, 24       
    bge a0, t1, overflow_error

    lb t0, buffer
    beqz t0, format_error 

    la a1, buffer

find_space:
    lb t0, 0(a1)          
    beqz t0, format_error   
    li t2, ' '
    beq t0, t2, parse_numbers 
    addi a1, a1, 1         
    j find_space

parse_numbers:
    sb zero, 0(a1)          
    addi a1, a1, 1       

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
    la a0, err_format
    call printf
    
    li a0, 1
    call exit

overflow_error:
    la a0, err_overflow
    call printf

    li a0, 1
    call exit
