/* Fitxer: proces.h
   Descripció: Estructura de dades relacionades amb els procesos                             */
      
#include "cues.h"
#include "Tipus.h"

#define ID_PROCES_NULL   0
#define MIDA_PILA        1024
#define RDY              1
#define RUN              2 
#define BLK              3

typedef unsigned long LONG;
typedef unsigned int INT;

typedef struct tPCB {
   struct item Elem;                             // Estructura de la llibreria cues.h
   char * Proces;
   INT ID;
   PTR SS_SP;                                    // Punter (void *) de la llibreria Ec.h
   INT Estat;
   INT Prioritat;
   WORD Pila[MIDA_PILA];
// -------------------------- [ SYSCALL DORMIRPROCES] ------------------------------------   
   long Quantum;                                 // Dormir ntics -> Sleep
// ---------------------------------------------------------------------------------------
// --------------------------------- [ E/S ] ---------------------------------------------
   INT Ftec;                                     // Tecles que falten per llegir
   char * BufferDesti;                           // Destí on les dades serán dipositades
// ---------------------------------------------------------------------------------------
};

typedef struct tCONTEXT {
   int  DS, ES, DI, SI, AX, BX, CX, DX, BP;
   WORD SYSC_IP;  
   WORD SYSC_CS;
   WORD Flags;
   WORD IP;
   WORD CS;
};