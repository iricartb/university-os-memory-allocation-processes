/* Fitxer: pila.h
   Implementació: pila.asm
   Descripció: Capçalera de funcions relacionades amb la pila (STACK) del procés             */
                  
extern int *(inicialitzar_Stack(PTR direc, WORD sp, WORD ss));
extern int *(salvar_Context(void));
extern void restaurar_Context(int *);