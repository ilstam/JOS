/* See COPYRIGHT for copyright information. */

#include <inc/mmu.h>
#include <inc/memlayout.h>
#include <inc/trap.h>

#include <kern/picirq.h>


###################################################################
# exceptions/interrupts
###################################################################

/* TRAPHANDLER defines a globally-visible function for handling a trap.
 * It pushes a trap number onto the stack, then jumps to _alltraps.
 * Use TRAPHANDLER for traps where the CPU automatically pushes an error code.
 *
 * You shouldn't call a TRAPHANDLER function from C, but you may
 * need to _declare_ one in C (for instance, to get a function pointer
 * during IDT setup).  You can declare the function with
 *   void NAME();
 * where NAME is the argument passed to TRAPHANDLER.
 */
#define TRAPHANDLER(name, num)						\
	.text;								\
	.globl name;		/* define global symbol for 'name' */	\
	.type name, @function;	/* symbol type is function */		\
	.align 2;		/* align function definition */		\
	name:			/* function starts here */		\
	pushl $(num);							\
	jmp _alltraps;							\
	.data;								\
	.long name;

/* Use TRAPHANDLER_NOEC for traps where the CPU doesn't push an error code.
 * It pushes a 0 in place of the error code, so the trap frame has the same
 * format in either case.
 */
#define TRAPHANDLER_NOEC(name, num)					\
	.text;								\
	.globl name;							\
	.type name, @function;						\
	.align 2;							\
	name:								\
	pushl $0;							\
	pushl $(num);							\
	jmp _alltraps;							\
	.data;								\
	.long name;

.data
.globl handlers
handlers:

TRAPHANDLER_NOEC(trap_handler_0, 0)
TRAPHANDLER_NOEC(trap_handler_1, 1)
TRAPHANDLER_NOEC(trap_handler_2, 2)
TRAPHANDLER_NOEC(trap_handler_3, 3)
TRAPHANDLER_NOEC(trap_handler_4, 4)
TRAPHANDLER_NOEC(trap_handler_5, 5)
TRAPHANDLER_NOEC(trap_handler_6, 6)
TRAPHANDLER_NOEC(trap_handler_7, 7)
TRAPHANDLER(trap_handler_8, 8)
.space 4 // reserved - we MUST fill the gap
TRAPHANDLER(trap_handler_10, 10)
TRAPHANDLER(trap_handler_11, 11)
TRAPHANDLER(trap_handler_12, 12)
TRAPHANDLER(trap_handler_13, 13)
TRAPHANDLER(trap_handler_14, 14)
.space 4 // reserved - we MUST fill the gap
TRAPHANDLER_NOEC(trap_handler_16, 16)
TRAPHANDLER(trap_handler_17, 17)
TRAPHANDLER_NOEC(trap_handler_18, 18)
TRAPHANDLER_NOEC(trap_handler_19, 19)
// interrupt vectors 20-31 are reserved

.space 48 // fill the gap for vectors 20-31

// IRQ handlers

TRAPHANDLER_NOEC(trap_handler_32, 32)
TRAPHANDLER_NOEC(trap_handler_33, 33)
TRAPHANDLER_NOEC(trap_handler_34, 34)
TRAPHANDLER_NOEC(trap_handler_35, 35)
TRAPHANDLER_NOEC(trap_handler_36, 36)
TRAPHANDLER_NOEC(trap_handler_37, 37)
TRAPHANDLER_NOEC(trap_handler_38, 38)
TRAPHANDLER_NOEC(trap_handler_39, 39)
TRAPHANDLER_NOEC(trap_handler_40, 40)
TRAPHANDLER_NOEC(trap_handler_41, 41)
TRAPHANDLER_NOEC(trap_handler_42, 42)
TRAPHANDLER_NOEC(trap_handler_43, 43)
TRAPHANDLER_NOEC(trap_handler_44, 44)
TRAPHANDLER_NOEC(trap_handler_45, 45)
TRAPHANDLER_NOEC(trap_handler_46, 46)
TRAPHANDLER_NOEC(trap_handler_47, 47)

TRAPHANDLER_NOEC(trap_handler_48, T_SYSCALL)

// we need to fill the gaps above in order to be able to index properly
// in the handlers array from within the C code

.text
_alltraps:
	pushl %ds
	pushl %es
	pushal
	movw $GD_KD, %ax
	movw %ax, %ds
	movw %ax, %es
	pushl %esp
	call trap
