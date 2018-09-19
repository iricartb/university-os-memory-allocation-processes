public _CrearProces
public _QuiSoc
public _Passo
public _DormirProces
public _DestruirProces
public _IniSem
public _WaitS
public _SignalS
public _ElimSem
public _EscriurePantalla
public _LlegirTeclat
public _EscriurePantalla2

_DATA   segment word public 'DATA'
_DATA   ends

_BSS    segment word public 'BSS'
_BSS    ends

DGROUP  group   _DATA, _BSS

assume cs:SYSCALLS_CODI, ds:DGROUP

SYSCALLS_CODI segment byte public 'CODE'

; ################################# [ CREAR PROCES ] ####################################

_CrearProces proc far
		
    MOV   ax, 1                            ; Identificador de la rutina CrearProces
    INT   40h                              ; Interrupcio (crida al TRAP)
    RET                                    ; Retorn al codi d'usuari del proces
		
_CrearProces endp

; ################################### [ QUI SOC ] #######################################

_QuiSoc proc far
		
    MOV   ax, 2                            ; Identificador de la rutina QuiSoc
    INT   40h                              ; Interrupcio (crida al TRAP)
    RET                                    ; Retorn al codi d'usuari del proces
		
_QuiSoc endp

; #################################### [ PASSO ] ########################################

_Passo proc far
		
    MOV   ax, 3                            ; Identificador de la rutina Passo
    INT   40h                              ; Interrupcio (crida al TRAP)
    RET                                    ; Retorn al codi d'usuari del proces
		
_Passo endp

; ################################# [ DORMIR PROCES ] ###################################

_DormirProces proc far
		
    MOV   ax, 4                            ; Identificador de la rutina DormirProces
    INT   40h                              ; Interrupcio (crida al TRAP)
    RET                                    ; Retorn al codi d'usuari del proces
		
_DormirProces endp

; ################################ [ DESTRUIR PROCES ] ##################################

_DestruirProces proc far
		
    MOV   ax, 5                            ; Identificador de la rutina DestruirProces
    INT   40h                              ; Interrupcio (crida al TRAP)
    RET                                    ; Retorn al codi d'usuari del proces
		
_DestruirProces endp

; ############################## [ INICIALITZAR SEMAFOR ] ###############################

_IniSem proc far
    
    MOV   ax, 6                            ; Identificador de la rutina IniSem
    INT   40h                              ; Interrupcio (crida al TRAP)
    RET                                    ; Retorn al codi d'usuari del proces

_IniSem endp

; ################################## [ WAIT SEMAFOR ] ###################################

_WaitS proc far
    
    MOV   ax, 7                            ; Identificador de la rutina WaitS
    INT   40h                              ; Interrupcio (crida al TRAP)
    RET                                    ; Retorn al codi d'usuari del proces

_WaitS endp

; ################################## [ SIGNAL SEMAFOR ] #################################

_SignalS proc far
    
    MOV   ax, 8                            ; Identificador de la rutina SignalS
    INT   40h                              ; Interrupcio (crida al TRAP)
    RET                                    ; Retorn al codi d'usuari del proces

_SignalS endp

; ################################### [ ELIM SEMAFOR ] ##################################

_ElimSem proc far
    
    MOV   ax, 9                            ; Identificador de la rutina ElimSem
    INT   40h                              ; Interrupcio (crida al TRAP)
    RET                                    ; Retorn al codi d'usuari del proces

_ElimSem endp

; ################################# [ ESCRIURE PANTALLA ] ###############################

_EscriurePantalla proc far
    
    MOV   ax, 10                           ; Identificador de la rutina EscriurePantalla
    INT   40h                              ; Interrupcio (crida al TRAP)
    RET                                    ; Retorn al codi d'usuari del proces

_EscriurePantalla endp

; ################################## [ LLEGIR TECLAT ] ##################################

_LlegirTeclat proc far
    
    MOV   ax, 11                           ; Identificador de la rutina LlegirTeclat
    INT   40h                              ; Interrupcio (crida al TRAP)
    RET                                    ; Retorn al codi d'usuari del proces

_LlegirTeclat endp

; ################################# [ ESCRIURE PANTALLA ] ###############################

_EscriurePantalla2 proc far
    
    MOV   ax, 12                           ; Identificador de la rutina EscriurePantalla2
    INT   40h                              ; Interrupcio (crida al TRAP)
    RET                                    ; Retorn al codi d'usuari del proces

_EscriurePantalla2 endp

SYSCALLS_CODI ends

end