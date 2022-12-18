################################################################################
# Homework 3: Matrix-matrix multiplication
# CSCI-2500 Fall 2021
################################################################################
## Data segment
################################################################################
.data
  newline:  .asciiz "\n"
  tab:      .asciiz "\t"

################################################################################
## Text segment
################################################################################
.text
  .align 2          # instructions must be on word boundaries
  .globl main       # main is a global label
  .globl mm_read    # read in NxM matrix
  .globl mm_alloc   # allocate space for NxM matrix
  .globl mm_print   # print out formatted NxM matrix
  .globl mm_mult    # multiple two non-square matrices

################################################################################
# Main segment
################################################################################
main:
  # save return address on the stack
  sub $sp, $sp, 4   
  sw  $ra, 0($sp)

  # Read in a matrix 1 of some NxM size
  # Return in $v0 - N
  # Return in $v1 - M
  # Return in 0($sp) - base address of matrix
  sub $sp, $sp, 4   # make space on stack for return value
  jal mm_read

  # save matrix 1 values
  move  $s0, $v0
  move  $s1, $v1
  lw    $s2, 0($sp)
  add   $sp, $sp, 4   # restore stack

  # Setup arguments and print out matrix 1
  move  $a0, $s0
  move  $a1, $s1
  move  $a2, $s2
  jal mm_print

  # Read in matrix 2 of some NxM size
  sub $sp, $sp, 4   # make space on stack for return value
  jal mm_read

  # save matrix 2 values
  move  $s3, $v0
  move  $s4, $v1
  lw    $s5, 0($sp)
  add   $sp, $sp, 4   # restore stack

  # Setup arguments and print out matrix 2
  move  $a0, $s3
  move  $a1, $s4
  move  $a2, $s5
  jal mm_print

  # Setup arguments and call mm_mult
  # Note: 5 arguments, so we need to use stack
  # Note: num cols matrix 1 == num rows matrix 2
  move  $a0, $s0    # num rows matrix1
  move  $a1, $s1    # num cols matrix1
  move  $a2, $s2    # address of matrix 1
  move  $a3, $s4    # num cols matrix2
  sub   $sp, $sp, 4   
  sw    $s5, 0($sp) # address of matrix 2
  jal mm_mult
  add   $sp, $sp, 4

  # print the result
  move $a0, $s0 
  move $a1, $s4
  move $a2, $v0
  jal mm_print

  # restore $ra, free stack space, and return
  lw  $ra, 0($sp)
  add $sp, $sp, 4
  jr  $ra

################################################################################
# mm_read: Read in a NxM matrix from standard input
# Note: You will be calling mm_alloc from this function, so you'll need to save 
#       $ra on the stack. Also, you'll be returning 3 values back to main, one
#       value (the matrix base address), will need to be saved on the stack.
#       Matrix format is:
#         First line:  N = num rows
#         Second line: M = num cols
#         Subsequent lines contain one value each, filling each row in turn
#         (same format as hw01, except N,M on separate lines)  
# Input:
#   None
# Return:
#   v0 = N
#   v1 = M
#   0($sp) = base address of matrix
################################################################################
mm_read:
  # save return address and any saved registers on the stack, if necessary
  sub $sp, $sp, 16
  sw $ra, 0($sp)
  sw $s0, 4($sp)
  sw $s1, 8($sp)
  sw $s2, 12($sp)
  # get N
  li  $v0, 5
  syscall
  move  $s0, $v0
  # get M
  li  $v0, 5
  syscall
  move  $s1, $v0
  # Setup up arguments and call mm_alloc - v0 is returned as base address
  move $a0, $s0
  move $a1, $s1
  jal mm_alloc
  move $s2, $v0 # returns the address here
  # do nested loops to read in values
  li  $t0, 0 # creating a counter for N
loop_row_read:
  beq $t0, $s0, done_row_read
  li  $t1, 0  # creating a counter for M
loop_col_read:
  beq $t1, $s1, done_col_read
  # address = base + offset
  # offset = sizeof(int)*(row_length*row_counter + col_counter)
  mul $t2, $t0, $s1
  add $t2, $t2, $t1
  sll $t2, $t2, 2
  add $t2, $t2, $s2

  # read value then store it
  li $v0, 5
  syscall
  sw $v0, 0($t2)

  # increment counter
  add $t1, $t1, 1
  j loop_col_read

done_col_read:
  add $t0, $t0, 1
  j loop_row_read

done_row_read:  
  # setup up return values
  # Note: third return value goes on the stack *after* restoration below
  move  $v0, $s0  # for N
  move  $v1, $s1  # for M
  move  $t0, $s2  # third return value
  # restore stack, ra, and any saved registers, if necessary
  lw  $ra, 0($sp)
  lw  $s0, 4($sp)
  lw  $s1, 8($sp)
  lw  $s2, 12($sp)
  add $sp, $sp, 16

  sw  $t0, 0($sp) # last return value on the stack
  # return to main
  jr  $ra

################################################################################
# mm_alloc: Allocate an NxM matrix
# Note: You can just allocate a single array of length N*M.
# Input:
#   a0 = N
#   a1 = M
# Return:
#   v0 = base address of allocated space
################################################################################
mm_alloc:
  # save return address and any saved registers on the stack, if necessary

  # Allocate sufficient space
  # set return value
  mul   $a0, $a0, $a1 # matrix's total length for the array
  sll   $a0, $a0, 2   # storing integers so we allocate 4*length
  # set return value
  li    $v0, 9        # allocates memory for a0
  # restore stack, ra, and any saved registers, if necessary
  syscall             # v0 overrides the pointer allocations
  # return to main
  jr  $ra

################################################################################
# mm_print: Output the formatted contents of NxM matrix
# Note: Within a row, separate each value by tab
#       End each row with a newline
# Input:
#   a0 = N
#   a1 = M
#   a2 = base pointer
# Return:
#   None
################################################################################
mm_print:
  # save return address and any saved registers on the stack, if necessary
  move  $t0, $a0
  move  $t1, $a1
  move  $t2, $a2
  # do nested loops to print out values
  li  $t3, 0  # counter for N in the loop
  loop_row_print:
    beq $t3, $t0, done_row_print

  li  $t4, 0 # counter for M in the loop
  loop_col_print:
    beq $t4, $t1, done_col_print
  # the addresses = base + 4*(M*t3 + t4)
  mul $t5, $t3, $t1
  add $t5, $t5, $t4
  sll $t5, $t5, 2
  add $t5, $t5, $t2

  # print value then tab each time (formatting)
  lw  $a0, 0($t5)
  li  $v0, 1
  syscall
  la  $a0, tab
  li  $v0, 4
  syscall

  # increment counter for the inner loop
  add $t4, $t4, 1
  j loop_col_print

# once you print everything print a new line
done_col_print:
  la  $a0, newline
  li  $v0, 4
  syscall

  # increment the outer loop
  add $t3, $t3, 1
  j loop_row_print

done_row_print:
  # restore stack, ra, and any saved registers, if necessary

  # return to main
  jr  $ra

################################################################################
# mm_mult: Perform the actual NxM matrix multiplication
# Input:
#   a0 = N for matrix 1
#   a1 = M for matrix 1 <-> N for matrix 2
#   a2 = base address for matrix 1
#   a3 = M for matrix 2
#   0($sp) = base address for matrix 2
# Return:
#   v0 = base address for result matrix
################################################################################
mm_mult:
  # save return address and any saved registers on the stack, if necessary
  sub $sp, $sp, 28   
  sw  $ra, 0($sp)
  sw  $s0, 4($sp)
  sw  $s1, 8($sp)
  sw  $s2, 12($sp)
  sw  $s3, 16($sp)
  sw  $s4, 20($sp)
  sw  $s5, 24($sp)

  # move our inputs into registers
  move  $s0, $a0      # rows of first matrix
  move  $s1, $a1      # columns of first matrix
  move  $s2, $a2      # address of first matrix
  move  $s3, $a3      # columns of second matrix
  lw    $s4, 28($sp)  # address of second matrix
  # set up and call mm_alloc for result matrix
  move  $a0, $s0  
  move  $a1, $s3
  jal mm_alloc
  move  $s5, $v0  # holds the address for the result matrix
  # three nested loops for matrix-matrix multiplication
  li  $t0, 0    # our i < N1
loop_i:
  beq $t0, $s0, end_i
  li  $t1, 0    # our j < M2
loop_j:
  beq $t1, $s3, end_j
  li  $t3, 0    # we set result[i,j] to this
  li  $t2, 0    # our k < M1
loop_k:
  beq $t2, $s1, end_k
  # get address of matrix 1 = 4*(i*M1 + k)
  mul $t4, $t0, $s1
  add $t4, $t4, $t2
  sll $t4, $t4, 2
  add $t4, $t4, $s2

  # get mat1[i,k]
  lw  $t4, 0($t4)

  # get address of matrix 2 = 4*(k*M2 + j)
  mul $t5, $t2, $s3
  add $t5, $t5, $t1
  sll $t5, $t5, 2
  add $t5, $t5, $s4

  # get mat2[k,j]
  lw  $t5, 0($t5)

  # multiply and save in accumulator
  mul $t4, $t4, $t5
  add $t3, $t3, $t4

  add $t2, $t2, 1
  j loop_k
end_k:

  # set result[i,j] = t3  
  # address of matrix 3 = 4*(i*M2 + j)
  mul $t4, $t0, $s3
  add $t4, $t4, $t1
  sll $t4, $t4, 2
  add $t4, $t4, $s5
  # save result
  sw  $t3, 0($t4)

  add $t1, $t1, 1
  j loop_j
end_j:

  add $t0, $t0, 1
  j loop_i
end_i:
  # set return value
  move  $v0, $s5
  # restore stack, ra, and any saved registers, if necessary
  lw  $ra, 0($sp)
  lw  $s0, 4($sp)
  lw  $s1, 8($sp)
  lw  $s2, 12($sp)
  lw  $s3, 16($sp)
  lw  $s4, 20($sp)
  lw  $s5, 24($sp)
  add $sp, $sp, 28
  # return to main
  jr  $ra
