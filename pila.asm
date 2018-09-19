;  Fitxer: pila.asm
;  Descripci�: Implementaci� de funcions relacionades amb la pila (STACK) del proc�s

public _inicialitzar_Stack
public _salvar_Context
public _restaurar_Context

_DATA   segment word public 'DATA'
_DATA   ends

_BSS    segment word public 'BSS'
_BSS    ends

DGROUP  group   _DATA, _BSS

assume cs:PILA_CODI, ds:DGROUP

PILA_CODI segment byte public 'CODE'


; ##################################### [ INICIALITZAR PILA ] #####################################

_inicialitzar_Stack proc far

   PUSH   bp
   MOV    bp, sp                           ; Enlla� din�mic
   
   MOV    bx, ss                           ; Salvem Stack Segment  <- - - - - - - - - - - - - - - - - - - - - - - - - 
   MOV    es, bx
   MOV    cx, sp                           ; Salvem Stack Pointer                                                    |
                                                                                                                   ; |
   ; ################## [ Redirecci� cap a la zona de la nostra pila ] ##################                            |
                                                                                                                   ; |
   MOV    sp, [bp + 12]                    ; Stack Pointer (SP del proc�s)                                           | 
   MOV    ss, [bp + 10]                    ; Stack Segment (SS del proc�s)                                           |
                                                                                                                   ; |
   PUSHF                                   ; 1a Posici� de la nostra pila -> Flags                                   |
   POP    ax                               ; Agafem els Flags                                                        |
   OR     ax, 200h                         ; Obliguem que el FLAG d'interrupci� (IF) estigui activat -> bit 9        |
   PUSH   ax                                                                                                       ; |
                                                                                                                   ; | 
   PUSH   es:[bp + 8]                      ; Segment       (CS del proc�s)                                           |
   PUSH   es:[bp + 6]                      ; Despla�ament  (IP del proc�s)                                           |
                                                                                                                   ; |
   PUSH   65535                            ; Registre -> bp                                                        ; |
   PUSH   65535                            ; Registre -> dx                                                        ; |
   PUSH   65535                            ; Registre -> cx                                                        ; |
   PUSH   65535                            ; Registre -> bx                                                        ; |
   PUSH   65535                            ; Registre -> ax                                                        ; |
   PUSH   65535                            ; Registre -> si                                                        ; |
   PUSH   65535                            ; Registre -> di                                                        ; |
   PUSH   ds                               ; Registre -> es                                                        ; | 
   PUSH   ds                               ; Registre -> ds                                                        ; |
                                                                                                                   ; |
   MOV    dx, ss                           ; Retornem el Stack Segment (pila nostra)                                 |
   MOV    ax, sp                           ; Retornem el Stack Pointer (pila nostra -> �ltima posici�)               |
                                                                                                                   ; |
   MOV    ss, bx                           ; Tornem a la pila inicial  >- - - - - - - - - - - - - - - - - - - - - - - 
   MOV    sp, cx
   
   POP    bp                                 
   RET
    
_inicialitzar_Stack endp

_salvar_Context proc far
   
   PUSH   bx                               
   MOV    bx, sp                           ; Creem enlla� din�mic amb bx 
   XCHG   cx, [bx + 2]                     ; Intercanvi cx <-> despla�ament de l'adre�a de retorn de l'RSI
   XCHG   dx, [bx + 4]                     ; Intercanvi dx <-> segment de l'adre�a de retorn de l'RSI
   
   ; ################## [ Salvar registres ] ##################   

   PUSH   ax
   PUSH   si
   PUSH   di
   PUSH   es
   PUSH   ds    
    
   MOV    ax, DGROUP
   MOV    ds, ax
   MOV    es, ax
   
   PUSH   dx                               ; Tornem cap a la RSI ficant a la pila el segment i el despla�ament
   PUSH   cx
       
   MOV    dx, ss                           ; Retornem el Stack Segment
   MOV    ax, sp                           ; Retornem el Stack Pointer (la part m�s baixa de la pila del proc�s)
   ADD    ax, 4
   
   RET
         
_salvar_Context endp
    
_restaurar_Context proc far

    ADD   sp, 4                            ; Eliminem l'adre�a de retorn cap a l'RSI (CS:IP)

    POP   ax                               ; Agafem l'argument passat per par�metre a la funci� (SS:SP)
    POP   ss                               ; Ens dirigim cap a la pila del proc�s
    MOV   sp, ax
    
    ; ################## [ Restaurar registres ] ##################
    
    POP   ds
    POP   es 
    POP   di
    POP   si
    POP   ax
    POP   bx
    POP   cx
    POP   dx
    POP   bp
    
    IRET                                   ; Agafem l'adre�a de Retorn del proc�s i els flags
      
_restaurar_Context endp
     
PILA_CODI ends

end