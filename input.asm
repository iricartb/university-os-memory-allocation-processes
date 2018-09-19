;  Fitxer: input.asm
;  Descripció: Implementació de funcions relacionades amb l'entrada d'informació 
;              standard -> TECLAT
   
public _LlegirTecla

_DATA   segment word public 'DATA'
_DATA   ends

_BSS    segment word public 'BSS'
_BSS    ends

DGROUP  group   _DATA, _BSS

assume cs:INPUT_CODI, ds:DGROUP

INPUT_CODI segment byte public 'CODE'

; ################################# [ LLEGIR TECLA ] ####################################

_LlegirTecla proc far
		
    IN    al, 60h                          ; Scancode de la tecla pulsada
    TEST  al, 10000000b                    ; Mirem si es Make o Break
    JZ    MAKE                           
    MOV	  al, 1		                       ; Break -> retornem 1
    
  MAKE:
                       
    MOV   ah, 0
	RET
		
_LlegirTecla endp

INPUT_CODI ends

end