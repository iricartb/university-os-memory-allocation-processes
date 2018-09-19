/* Fitxer: pila.h
   Implementaci�: pila.asm
   Descripci�: Cap�alera de funcions relacionades amb la pila (STACK) del proc�s             */
                  
extern int *(inicialitzar_Stack(PTR direc, WORD sp, WORD ss));
extern int *(salvar_Context(void));
extern void restaurar_Context(int *);