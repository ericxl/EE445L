; Print.s
; Student names: change this to your names or look very silly
; Last modification date: change this to the last modification date or look very silly
; Runs on LM4F120 or TM4C123
; EE319K lab 7 device driver for any LCD
;
; As part of Lab 7, students need to implement these LCD_OutDec and LCD_OutFix
; This driver assumes two low-level LCD functions
; ST7735_OutChar   outputs a single 8-bit ASCII character
; ST7735_OutString outputs a null-terminated string 

    IMPORT   ST7735_OutChar
    IMPORT   ST7735_OutString
    EXPORT   LCD_OutDec
    EXPORT   LCD_OutFix

    AREA    |.text|, CODE, READONLY, ALIGN=2
    THUMB

  

;-----------------------LCD_OutDec-----------------------
; Output a 32-bit number in unsigned decimal format
; Input: R0 (call by value) 32-bit unsigned number
; Output: none
; Invariables: This function must not permanently modify registers R4 to R11
LCD_OutDec
	PUSH {R4,LR}
	MOV R1,#10
	MOV R4,#0      ;i=0
	
Again
	
	UDIV R2,R0,R1  ;R2=R0/R1
	MUL R3,R2,R1   ;R3=(R0/R1)*R1
	SUB R3,R0,R3   ;R3=R0%R1, R3 is remainder of R0/R1
	PUSH {R3,LR}
	ADD R4,#1      ;i++	
	ADDS R0,R2,#0  ;if result of divison is 0 end of number
	BEQ Print      ;print the result
	
    B Again

Print	
	
	POP {R0,LR}    ;Retrieve digit from big to small
	ADD R0,#48     ;convert from binary to ASCII equivalent
	BL ST7735_OutChar ;outputs ASCII forms
	SUBS R4,#1        ;i--
	BEQ DONE          ;if count=0 return
	
	B Print

DONE
    POP {R4,LR}

      BX  LR
;* * * * * * * * End of LCD_OutDec * * * * * * * *

; -----------------------LCD _OutFix----------------------
; Output characters to LCD display in fixed-point format
; unsigned decimal, resolution 0.001, range 0.000 to 9.999
; Inputs:  R0 is an unsigned 32-bit number
; Outputs: none
; E.g., R0=0,    then output "0.000 "
;       R0=3,    then output "0.003 "
;       R0=89,   then output "0.089 "
;       R0=123,  then output "0.123 "
;       R0=9999, then output "9.999 "
;       R0>9999, then output "*.*** "
; Invariables: This function must not permanently modify registers R4 to R11
LCD_OutFix
	PUSH {R4,LR}
	MOV R1,#10
	MOV R4,#4      ;i=4
	MOV R2,#10000
	CMP R0,R2
	BCS PrintAst
	
Again2
	UDIV R2,R0,R1  ;R2=R0/R1
	MUL R3,R2,R1   ;R3=(R0/R1)*R1
	SUB R3,R0,R3   ;R3=R0%R1, R3 is remainder of R0/R1
	PUSH {R3,LR}
	ADD R0,R2,#0
	SUBS R4,#1      ;i--	
	BEQ Print2      ;print the result
	
    B Again2

Print2	
	
	MOV R4,#3         ;i=3
	POP {R0,LR}       ;Retrieve digit from big to small
	ADD R0,#48        ;convert from binary to ASCII equivalent
	BL ST7735_OutChar ;outputs ASCII forms
	MOV R0,#46
	BL ST7735_OutChar ;outputs "."
	
OutputNum
	
	POP {R0,LR}        ;outputs decimal places
	ADD R0,#48
	BL ST7735_OutChar
	SUBS R4,#1         ;i--
	BEQ DONE2          ;if count=0 return
	B OutputNum
	
PrintAst              ;;outputs "*.***"
	 MOV R4,#3
	 MOV R0,#42
	 BL ST7735_OutChar
	 MOV R0,#46
	 BL ST7735_OutChar
Times3	 
	 MOV R0,#42
	 BL ST7735_OutChar
	 SUBS R4,#1
	 BNE Times3
	 

DONE2
    POP {R4,LR}
     
	 BX   LR
 
     ALIGN
;* * * * * * * * End of LCD_OutFix * * * * * * * *

     ALIGN                           ; make sure the end of this section is aligned
     END                             ; end of file
