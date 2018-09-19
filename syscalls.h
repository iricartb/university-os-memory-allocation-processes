/* Fitxer: syscalls.h
   Implementació: syscalls.asm
   Descripció: Capçalera de les crides del sistema del SO                                    */
   
/* ################################ [ CRIDES AL SISTEMA ] ################################## */

extern int CrearProces(char * nomProces, void (*codi)(), unsigned int prioritat);                                          // ID Syscall -> 1
extern unsigned int QuiSoc(void);                                                                                          // ID Syscall -> 2
extern unsigned int Passo(void);                                                                                           // ID Syscall -> 3
extern void DormirProces(long ntics);                                                                                      // ID Syscall -> 4
extern int DestruirProces(unsigned int identificador);                                                                     // ID Syscall -> 5
extern int IniSem(unsigned int ID, unsigned int valor);                                                                    // ID Syscall -> 6
extern int WaitS(unsigned int ID);                                                                                         // ID Syscall -> 7
extern int SignalS(unsigned int ID);                                                                                       // ID Syscall -> 8
extern int ElimSem(unsigned int ID);                                                                                       // ID Syscall -> 9
extern int EscriurePantalla(char * string, unsigned int longitud);                                                         // ID Syscall -> 10
extern int LlegirTeclat(char * string, unsigned int longitud);                                                             // ID Syscall -> 11
extern int EscriurePantalla2(char * string, unsigned int fila, unsigned int col, unsigned int longitud, unsigned int atr); // ID Syscall -> 12