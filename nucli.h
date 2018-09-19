/* Fitxer: nucli.h
   Descripció: Agafem estructures, variables i funcions del fitxer nucli.c                   */
                 
extern void multiplexar(void);
extern void teclat(void);
extern idproces;

/* ################################# [ DECLARACIONS ] ###################################### */

void CrearProces_K(void);
void QuiSoc_K(void);
void Passo_K(void);
void DormirProces_K(void);
void DestruirProces_K(void);
void IniSem_K(void);
void WaitS_K(void);
void SignalS_K(void);
void ElimSem_K(void);
void EscriurePantalla_K(void);
void LlegirTeclat_K(void);
void EscriurePantalla2_K(void);

/* ################################ [ CRIDES AL SISTEMA ] ################################## */

extern void CrearProces_K();               // ID Syscall -> 1
extern void QuiSoc_K();                    // ID Syscall -> 2
extern void Passo_K();                     // ID Syscall -> 3
extern void DormirProces_K();              // ID Syscall -> 4
extern void DestruirProces_K();            // ID Syscall -> 5
extern void IniSem_K();                    // ID Syscall -> 6
extern void WaitS_K();                     // ID Syscall -> 7
extern void SignalS_K();                   // ID Syscall -> 8
extern void ElimSem_K();                   // ID Syscall -> 9
extern void EscriurePantalla_K();          // ID Syscall -> 10
extern void LlegirTeclat_K();              // ID Syscall -> 11
extern void EscriurePantalla2_K();         // ID Syscall -> 12