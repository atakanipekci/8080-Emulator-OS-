	        ; 8080 assembler code
        .hexfile Assignment1.hex
        .binfile Assignment1.com
        ; try "hex" for downloading in hex format
        .download bin  
        .objcopy gobjcopy
        .postbuild echo "OK!"
        ;.nodump

	; OS call list
PRINT_B		equ 1
PRINT_MEM	equ 2
READ_B		equ 3
READ_MEM	equ 4
PRINT_STR	equ 5
READ_STR	equ 6
GET_RND		equ 7
TEXIT		equ 8
TJOIN		equ 9
TYIELD		equ 10
TCREATE		equ 11
FILECREATE	equ 12
FILECLOSE	equ 13
FILEOPEN	equ 14
FILEREAD	equ 15
FILEWRITE	equ 16
FILESEEK	equ 17
DIRREAD		equ 18 


	; Position for stack pointer
stack   equ 0F000h

	org 000H
	jmp begin

	; Start of our Operating System
GTU_OS:	PUSH D
	push D
	push H
	push psw
	nop	; This is where we run our OS in C++, see the CPU8080::isSystemCall()
		; function for the detail.
	pop psw
	pop h
	pop d
	pop D
	ret
	; ---------------------------------------------------------------
	; YOU SHOULD NOT CHANGE ANYTHING ABOVE THIS LINE        

	;This program 


	writevalues: DW 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50 ;
	readvalues: DW 99 ;
	file: dw 'Enter File Name For Creation:',00AH,00H ; null terminated string
	

	begin:
	LXI SP,stack 	; always initialize the stack pointer



	LXI B, file	; put the address of string in registers B and C
	MVI A, PRINT_STR	; store the OS call code to A
	CALL GTU_OS	; call the OS
	MVI A, READ_STR
	CALL GTU_OS

			
	
	MVI A, FILECREATE	; store the OS call code to A
	call GTU_OS	; call the OS

	

	MVI A, FILEOPEN	; store the OS call code to A
	call GTU_OS	; call the OS

	MOV E,B
	MVI D,51
	LXI B,writevalues
	MVI A, FILEWRITE
	
	call GTU_OS
	
	;LXI B,readvalues
	;MVI A, FILEREAD
	;call GTU_OS


	MOV B,E
	MVI A, FILECLOSE	; store the OS call code to A
	call GTU_OS	; call the OS



	

	hlt		; end program
