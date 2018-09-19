/* Fitxer: procs.c
   Descripció: Joc de proves de la pràctica (diferents processos)                             */
   
#include "Ec.h"
#include "main.h"
#include "nucli.h"
#include "pantalla.h"
#include "proces.h"
#include "syscalls.h"
#include "semafors.h"

#define SDELAY 1
#define DELAY  5000000UL
#define DELAY2 100000000UL
#define INTCLK 20                   /* 20 Interrupcions de rellotge/segon 
                                       1 Interrupció cada 50 milisegons   */                              

extern struct tPCB *procesRUN;
extern struct tSEMAFOR taulaSem[MAX_NUM_SEMS];
void proc1(void);
void procA(void);
void procA2(void);
void procM(void);
void procB(void);
void aleste(void);
void shoot(void);
void fire(void);
void randenem(void);
void enemic(void);

unsigned int posy;

// ############################### [ JOC DE PROVES 1 ] ###############################

void proc1(void) {
   unsigned int col = 0, atr = 7;
   char chr[4] = "\\x90";
   unsigned long i = 0;
   unsigned int pos = 0;
   
   EscriureString(QuiSoc(), 1, procesRUN->Proces, CARN);
   
   for(;;) {
      EscriureString(QuiSoc(), 1, procesRUN->Proces, BLAU2);     
      for (i = 0; i < DELAY; i++);
	  EscriureCar(QuiSoc(), 7 + col, chr[pos], atr); pos = ((pos + 1) % 4);
      EscriureCar(21, 43, ROMBE, FGRIS + VERMELL);
      EscriureCar(21, 8, ROMBE, FGRIS + VERMELL);
      EscriureString(QuiSoc(), 1, procesRUN->Proces, CARN);      
	  col = (col + 1) % 72;
      if (col == 0) atr = ((atr + 1) % 16); 
	  Passo();
   }
}

// ############################### [ JOC DE PROVES 2 ] ###############################

void jproves2(void) {
   unsigned int j;
   
   CrearProces("PROCB", procB, 1);
   CrearProces("PROCA", procA, 6);
   CrearProces("PROCM", procM, 3);
   EscriureString(3, 1, procesRUN->Proces, CARN);
   EscriureString(3, 7, "He creat 3 procesos [ PROCA, PROCB, PROCM ] i ara m'autodestruire", NORMAL);
   EscriureString(5, 7, "[   ]", GROC); EscriureCar(5, 9, '+', CARN);
   EscriureString(5, 12, " Codi Retorn", NORMAL);
   EscriureString(5, 24, " [   ]", GROC);
   EscriureCar(5, 27, DestruirProces(QuiSoc()) + 48, CARN);
   // El còdig seguent no s'ha d'executar, ens hem autodestruit
   for(j = 0;;j++) {
      j = j % 80;    
      EscriureCar(5, j, 'A', NORMAL);
   }
}

void procA(void) {
   unsigned int col = 0, atr = 7;
   char chr = 'A';
   unsigned long i = 0, j = 0;
   
   EscriureString(7, 1, procesRUN->Proces, CARN);
   for(j = 0; j < 144; j++) {
      for (i = 0; i < DELAY; i++);        
	  EscriureCar(7, 7 + col, chr, atr);
      EscriureCar(21, 43, ROMBE, FGRIS + VERMELL);
      EscriureCar(21, 8, ROMBE, FGRIS + VERMELL);      
	  col = (col + 1) % 72;
      if (col == 0) atr = ((atr + 1) % 16);
   }
   DormirProces(INTCLK * 10);
   EscriureString(9, 7, "[   ]", GROC); EscriureCar(9, 9, '+', CARN);
   EscriureString(9, 12, " Destruir PROCB -> Codi Retorn", NORMAL);
   EscriureString(9, 42, " [   ]", GROC);
   EscriureCar(9, 45, DestruirProces(3) + 48, CARN);
   EscriureString(10, 7, "[   ]", GROC); EscriureCar(10, 9, '+', CARN);
   EscriureString(10, 12, " Destruir PROCM -> Codi Retorn", NORMAL);
   EscriureString(10, 42, " [   ]", GROC);
   EscriureCar(10, 45, DestruirProces(5) + 48, CARN);
   for(j = 0; j < COLS; j++) EscriureCar(11, j, ' ', NORMAL);
   for(;;) {
      EscriureCar(21, 43, ROMBE, FGRIS + VERMELL);
      EscriureCar(21, 8, ROMBE, FGRIS + VERMELL);
   }       
} 
   
void procM(void) {
   unsigned int col = 0, atr = 7;
   char chr = 'M';
   unsigned long i = 0;
   
   EscriureString(11, 1, procesRUN->Proces, CARN);
   for(;;) {
      for (i = 0; i < DELAY; i++);        
	  EscriureCar(11, 7 + col, chr, atr);
      EscriureCar(21, 43, ROMBE, FGRIS + VERMELL);
      EscriureCar(21, 8, ROMBE, FGRIS + VERMELL);      
	  col = (col + 1) % 72;
      if (col == 0) atr = ((atr + 1) % 16);
   }         
} 

void procB(void) {
   EscriureString(7, 1, procesRUN->Proces, CARN);
   EscriureString(7, 7, "Soc el Proces amb menys prioritat", NORMAL);
   EscriureString(9, 7, "[   ]", GROC); EscriureCar(9, 9, '+', CARN);
   EscriureString(9, 12, " Destruir PROCM -> Codi Retorn", NORMAL);
   EscriureString(9, 42, " [   ]", GROC);
   EscriureCar(9, 45, DestruirProces(5) + 48, CARN);
   for(;;) {
      EscriureCar(21, 43, ROMBE, FGRIS + VERMELL);
      EscriureCar(21, 8, ROMBE, FGRIS + VERMELL);
   }
}

// ############################### [ JOC DE PROVES 3 ] ###############################

void jproves3(void) {
   unsigned int j, i;
   
   CrearProces("PROCB", procB, 1);
   CrearProces("PROCA", procA2, 6);
   CrearProces("PROCM", procM, 3);
   
   EscriureString(5, 31, "[   ]", GROC);
   EscriureString(5, 37, "Total Procesos", NORMAL);
   for(i = 1; CrearProces("PROC", proc1, 2) != -1; i++) {
      EscriureCar(5, 33, 48 + 6 + i, CARN);
   }
   // Els pròxims 3 procesos no es poden crear per falta de PCB's lliures
   if (Buida(pcuaPCB)) {
      EscriureString(5, 52, "[   ]", GROC);
      EscriureCar(5, 54, '!', VERM2);                  
      EscriureString(5, 58, "Falta de PCB's", NORMAL);
   } 
   EscriureString(3, 1, procesRUN->Proces, CARN);
   EscriureString(3, 7, "He creat 3 procesos [ PROCA, PROCB, PROCM ] i ara m'autodestruire", NORMAL);
   EscriureString(5, 7, "[   ]", GROC); EscriureCar(5, 9, '+', CARN);
   EscriureString(5, 12, " Codi Retorn", NORMAL);
   EscriureString(5, 24, " [   ]", GROC);
   EscriureCar(5, 27, DestruirProces(QuiSoc()) + 48, CARN);
   // El còdig seguent no s'ha d'executar, ens hem autodestruit
   for(j = 0;;j++) {
      j = j % 80;    
      EscriureCar(5, j, 'A', NORMAL);
   }
}

void procA2(void) {
   unsigned int col = 0, atr = 7;
   char chr = 'A';
   unsigned long i = 0, j = 0;
   
   EscriureString(7, 1, procesRUN->Proces, CARN);
   for(j = 0; j < 144; j++) {
      for (i = 0; i < DELAY; i++);        
	  EscriureCar(7, 7 + col, chr, atr);
      EscriureCar(21, 43, ROMBE, FGRIS + VERMELL);
      EscriureCar(21, 8, ROMBE, FGRIS + VERMELL);      
	  col = (col + 1) % 72;
      if (col == 0) atr = ((atr + 1) % 16);
   }
   DormirProces(250);
   EscriureString(9, 7, "[   ]", GROC); EscriureCar(9, 9, '+', CARN);
   EscriureString(9, 12, " Destruir PROCB -> Codi Retorn", NORMAL);
   EscriureString(9, 42, " [   ]", GROC);
   EscriureCar(9, 45, DestruirProces(3) + 48, CARN);
   EscriureString(10, 7, "[   ]", GROC); EscriureCar(10, 9, '+', CARN);
   EscriureString(10, 12, " Destruir PROCM -> Codi Retorn", NORMAL);
   EscriureString(10, 42, " [   ]", GROC);
   EscriureCar(10, 45, DestruirProces(5) + 48, CARN);
   for(j = 0; j < COLS; j++) EscriureCar(11, j, ' ', NORMAL);
   // Finalment creem els 3 procesos que no podiem crear anteriorment -> Reaprofitem PCB's
   CrearProces("PROC4", proc1, 2);
   CrearProces("PROC5", proc1, 2);
   CrearProces("PROC6", proc1, 2);
   ClearScreen();
   EscriureString(3, 1, procesRUN->Proces, CARN);
   EscriureString(3, 7, "S'han destruit 3 procesos anteriors i n'hem creat 3 + =>", NORMAL);
   EscriureString(3, 64, "REAPROFITAMENT", GROC);
   DestruirProces(QuiSoc());     
} 

// ############################### [ JOC DE PROVES 4 ] ###############################

void procDorm15(void) {
   unsigned int cont = 0;
   unsigned long i = 0;
   
   EscriureString(QuiSoc(), 1, procesRUN->Proces, CARN);
      
   for (cont = 0; cont < 5; cont++) {                        
      if (cont == 0) EscriureString(QuiSoc(), 7, "Ara m'adormire 15 segons", NORMAL);
      else EscriureString(QuiSoc(), 7, "Ara m'adormire 15 segons mes", NORMAL);
      EscriureString(QuiSoc(), 40, "[   ]", GROC); EscriureCar(QuiSoc(), 42, (5 - cont) + 48, CARN);
      DormirProces(INTCLK * 15);
      EscriureString(QuiSoc(), 7, "M'he despertat              ", NORMAL);
      EscriureCar(21, 43, ROMBE, FGRIS + VERMELL);
      EscriureCar(21, 8, ROMBE, FGRIS + VERMELL);
      for(i = 0; i < DELAY2; i++); 
   }
   EscriureCar(QuiSoc(), 42, (5 - cont) + 48, CARN);
   DestruirProces(QuiSoc());
}

void procDorm20(void) {
   unsigned int cont = 0;
   unsigned long i = 0;

   EscriureString(QuiSoc(), 1, procesRUN->Proces, CARN);      
   for (cont = 0; cont < 5; cont++) {                     
      if (cont == 0) EscriureString(QuiSoc(), 7, "Ara m'adormire 20 segons", NORMAL);
      else EscriureString(QuiSoc(), 7, "Ara m'adormire 20 segons mes", NORMAL);
      EscriureString(QuiSoc(), 40, "[   ]", GROC); EscriureCar(QuiSoc(), 42, (5 - cont) + 48, CARN);
      DormirProces(INTCLK * 20);
      EscriureString(QuiSoc(), 7, "M'he despertat              ", NORMAL);
      EscriureCar(21, 43, ROMBE, FGRIS + VERMELL);
      EscriureCar(21, 8, ROMBE, FGRIS + VERMELL);
      for(i = 0; i < DELAY2; i++); 
   }
   EscriureCar(QuiSoc(), 42, (5 - cont) + 48, CARN);
   DestruirProces(QuiSoc());
}

void procDorm10(void) {
   unsigned int cont = 0;
   unsigned long i = 0;
   
   EscriureString(QuiSoc(), 1, procesRUN->Proces, CARN);   
   for (cont = 0; cont < 5; cont++) {                     
      if (cont == 0) EscriureString(QuiSoc(), 7, "Ara m'adormire 10 segons", NORMAL);
      else EscriureString(QuiSoc(), 7, "Ara m'adormire 10 segons mes", NORMAL);
      EscriureString(QuiSoc(), 40, "[   ]", GROC); EscriureCar(QuiSoc(), 42, (5 - cont) + 48, CARN);
      DormirProces(INTCLK * 10);
      EscriureString(QuiSoc(), 7, "M'he despertat              ", NORMAL);
      EscriureCar(21, 43, ROMBE, FGRIS + VERMELL);
      EscriureCar(21, 8, ROMBE, FGRIS + VERMELL);
      for(i = 0; i < DELAY2; i++); 
   }
   EscriureCar(QuiSoc(), 42, (5 - cont) + 48, CARN);
   DestruirProces(QuiSoc());
}

// ############################### [ JOC DE PROVES 5 ] ###############################

void procSem(void) {
   unsigned int col = 0, atr = 7;
   char chr[4] = "\\x00";
   unsigned long i = 0;
   unsigned int pos = 0, csem = 0;
   struct tPCB * PCBaux;
   
   EscriureString(QuiSoc(), 1, procesRUN->Proces, CARN);
   EscriureString(10, 1, "[   ]", GROC); EscriureCar(10, 3, '+', CARN); 
   EscriureString(10, 7, "Semafor -> CuaBLK:", NORMAL); 
   
   for(;;) {      
      
      PCBaux = (struct tPCB *) taulaSem[0].cuaBLK.primer;
      for(csem = 0;((struct item *) PCBaux) != taulaSem[0].cuaBLK.ultim; csem++) {
         EscriureCar(10, (26 + 3 * csem), PCBaux->ID + 48, NORMAL);      
         PCBaux = (struct tPCB *) ((struct item *) PCBaux)->seguent;
      }
      EscriureCar(10, (26 + 3 * csem), PCBaux->ID + 48, NORMAL);
      EscriureString(QuiSoc(), 1, procesRUN->Proces, BLAU2);
      for (i = 0; i < DELAY; i++);
      SignalS(0);
      if (pos != 3) EscriureCar(QuiSoc(), 7 + col, chr[pos], atr); 
      else EscriureCar(QuiSoc(), 7 + col, chr[pos] + QuiSoc() - 1, atr);
      pos = ((pos + 1) % 4);
      EscriureCar(21, 43, ROMBE, FGRIS + VERMELL);
      EscriureCar(21, 8, ROMBE, FGRIS + VERMELL);
      EscriureString(QuiSoc(), 1, procesRUN->Proces, CARN);      
	  col = (col + 1) % 72;
      if (col == 0) atr = ((atr + 1) % 16); 
	  WaitS(0);
   }
}

// ############################### [ JOC DE PROVES 6 ] ###############################

void procSem2(void) {
   unsigned int col = 0, atr = 7;
   char chr[4] = "\\x00";
   unsigned long i = 0;
   unsigned int pos = 0, csem = 0;
   struct tPCB * PCBaux;
   
   EscriureString(QuiSoc(), 1, procesRUN->Proces, CARN);
   EscriureString(10, 1, "[   ]", GROC); EscriureCar(10, 3, '+', CARN); 
   EscriureString(10, 7, "Semafor -> CuaBLK:", NORMAL); 
   
   for(;;) {      
      
      EscriureString(QuiSoc(), 1, procesRUN->Proces, CARN);
      WaitS(1);
      if (pos != 3) EscriureCar(QuiSoc(), 7 + col, chr[pos], atr); 
      else EscriureCar(QuiSoc(), 7 + col, chr[pos] + QuiSoc() - 1, atr);
      pos = ((pos + 1) % 4);
      EscriureCar(21, 43, ROMBE, FGRIS + VERMELL);
      EscriureCar(21, 8, ROMBE, FGRIS + VERMELL);
      EscriureString(QuiSoc(), 1, procesRUN->Proces, BLAU2);      
	  col = (col + 1) % 72;
      if (col == 0) atr = ((atr + 1) % 16); 
      PCBaux = (struct tPCB *) taulaSem[0].cuaBLK.primer;
      for(csem = 0;((struct item *) PCBaux) != taulaSem[0].cuaBLK.ultim; csem++) {
         EscriureCar(10, (26 + 3 * csem), PCBaux->ID + 48, NORMAL);      
         PCBaux = (struct tPCB *) ((struct item *) PCBaux)->seguent;
      }
      EscriureCar(10, (26 + 3 * csem), PCBaux->ID + 48, NORMAL);
      Passo();
      for (i = 0; i < DELAY; i++);
	  SignalS(1);
   }
}

// ############################### [ JOC DE PROVES 7 ] ###############################

void filosofs(void) {   
   unsigned long i = 0;
   
   EscriureString(QuiSoc(), 1, procesRUN->Proces, CARN);
   EscriureString(QuiSoc(), 7, "Estic pensant           ", NORMAL);
   for(;;) {
      EscriureString(QuiSoc(), 1, procesRUN->Proces, BLAU2);     
      EscriureCar(21, 43, ROMBE, FGRIS + VERMELL);
      EscriureCar(21, 8, ROMBE, FGRIS + VERMELL);
      WaitS(QuiSoc() - 2);
      WaitS((QuiSoc() - 1) % 6);
      EscriureString(QuiSoc(), 7, "Estic menjant espaguetis", NORMAL);
      SignalS((QuiSoc() - 1) % 6);
      SignalS(QuiSoc() - 2);
      for (i = 0; i < DELAY * 2; i++);
      EscriureString(QuiSoc(), 7, "Estic pensant           ", NORMAL);
      EscriureString(QuiSoc(), 1, procesRUN->Proces, CARN);            
      Passo();
   }
}    

// ############################### [ JOC DE PROVES 8 ] ###############################

void procTec(void) {
   char desti[20];
   unsigned int cont, fi;
   
   EscriureString((QuiSoc()-1)*2 + (QuiSoc()-2)*2, 1, procesRUN->Proces, CARN);
   EscriureString((QuiSoc()-1)*2 + (QuiSoc()-2)*2, 7, "Demano per llegir    dades", NORMAL); 
   EscriureCar((QuiSoc()-1)*2 + (QuiSoc()-2)*2, 25, ((QuiSoc() * 5) / 10) + 48, CARN);
   EscriureCar((QuiSoc()-1)*2 + (QuiSoc()-2)*2, 26, ((QuiSoc() * 5) % 10) + 48, CARN);
   for(;;) {
      fi = 0;
      LlegirTeclat(desti, QuiSoc() * 5);
      EscriureString((QuiSoc()-1)*2 + (QuiSoc()-2)*2, 37, "                     ", NORMAL);
      EscriureString((QuiSoc()-1)*2 + (QuiSoc()-2)*2 + 1, 37, "                              ", NORMAL);
      for(cont = 0; (cont < (QuiSoc() * 5) && (fi == 0) && (cont < TAM_BUFFER)); cont++) {
         if (desti[cont] == RETURN) fi = 1;
         /* Desbloquejem el proces shell si s'ha apretat la tecla backspace
            i tornem al menu principal */
         else if (desti[cont] == BACKSPACE) SignalS(0);
      }
      if (cont < 10) {
         EscriureString((QuiSoc()-1)*2 + (QuiSoc()-2)*2, 37, "[   ]", GROC);
         EscriureString((QuiSoc()-1)*2 + (QuiSoc()-2)*2, 43, "Dades Llegides", NORMAL);
         EscriureCar((QuiSoc()-1)*2 + (QuiSoc()-2)*2, 39, cont + 48, CARN);
      }
      else {
         EscriureString((QuiSoc()-1)*2 + (QuiSoc()-2)*2, 37, "[    ]", GROC);
         EscriureString((QuiSoc()-1)*2 + (QuiSoc()-2)*2, 44, "Dades Llegides", NORMAL);
         EscriureCar((QuiSoc()-1)*2 + (QuiSoc()-2)*2, 39, (cont / 10) + 48, CARN);
         EscriureCar((QuiSoc()-1)*2 + (QuiSoc()-2)*2, 40, (cont % 10) + 48, CARN);           
      }
      EscriureString((QuiSoc()-1)*2 + (QuiSoc()-2)*2 + 1, 37, "[       ]", GROC);
      EscriureString((QuiSoc()-1)*2 + (QuiSoc()-2)*2 + 1, 39, "DADES", CARN);
      for(fi = 0; fi < cont; fi++) {
         if (desti[fi] != RETURN) EscriureCar((QuiSoc()-1)*2 + (QuiSoc()-2)*2 + 1, 47 + fi, desti[fi], NORMAL);
         else EscriureCar((QuiSoc()-1)*2 + (QuiSoc()-2)*2 + 1, 47 + fi, 27, GROC);
      }      
   }
}
              
// ############################### [ JOC DE PROVES 9 ] ###############################

void procTec2(void) {
   char desti[1];
   
   EscriureString(2, 1, procesRUN->Proces, CARN);
   EscriureString(2, 7, "Demano per llegir   caracter", NORMAL); 
   EscriureCar(2, 25, '1', CARN);
   for(;;) {
      LlegirTeclat(desti, 1);
      EscriureString(2, 40, "                     ", NORMAL);
      
      /* Desbloquejem el proces shell si s'ha apretat la tecla backspace
            i tornem al menu principal */
      if (desti[0] == BACKSPACE) SignalS(0);
      
      EscriureString(2, 40, "[   ]", GROC);
      EscriureString(2, 46, "Dada Llegida", NORMAL);
      EscriureCar(2, 42, '1', CARN);         
      
      DormirProces(INTCLK * 5);
   }
}

void procTec3(void) {
   char desti[10];
   unsigned int cont, fi;
   
   EscriureString(6, 1, procesRUN->Proces, CARN);
   EscriureString(6, 7, "Demano per llegir    caracters", NORMAL); 
   EscriureString(6, 25, "10", CARN);
   for(;;) {
      fi = 0;
      LlegirTeclat(desti, 10);
      EscriureString(6, 40, "                     ", NORMAL);
      for(cont = 0; ((cont < 10) && (fi == 0) && (cont < TAM_BUFFER)); cont++) {
         if (desti[cont] == RETURN) fi = 1;
         /* Desbloquejem el proces shell si s'ha apretat la tecla backspace
            i tornem al menu principal */
         else if (desti[cont] == BACKSPACE) SignalS(0);
      }
      if (cont < 10) {
         EscriureString(6, 40, "[   ]", GROC);
         EscriureString(6, 46, "Dades Llegides", NORMAL);
         EscriureCar(6, 42, cont + 48, CARN);
      }
      else {
         EscriureString(6, 40, "[    ]", GROC);
         EscriureString(6, 47, "Dades Llegides", NORMAL);
         EscriureCar(6, 42, (cont / 10) + 48, CARN);
         EscriureCar(6, 43, (cont % 10) + 48, CARN);           
      }
            
      DormirProces(INTCLK * 5);      
   }
}

void procTec4(void) {
   char desti[100];
   unsigned int cont, fi;
   
   EscriureString(10, 1, procesRUN->Proces, CARN);
   EscriureString(10, 7, "Demano per llegir     caracters", NORMAL); 
   EscriureString(10, 25, "100", CARN);
   for(;;) {
      fi = 0;
      LlegirTeclat(desti, 100);
      EscriureString(10, 41, "                     ", NORMAL);

      for(cont = 0; ((cont < 100) && (fi == 0) && (cont < TAM_BUFFER)); cont++) {
         if (desti[cont] == RETURN) fi = 1;
         /* Desbloquejem el proces shell si s'ha apretat la tecla backspace
            i tornem al menu principal */
         else if (desti[cont] == BACKSPACE) SignalS(0);
      }
      
      if (cont < 10) {
         EscriureString(10, 40, "[   ]", GROC);
         EscriureString(10, 46, "Dades Llegides", NORMAL);
         EscriureCar(10, 42, cont + 48, CARN);
      }
      else {
         if (cont == 100) {
            EscriureString(10, 40, "[     ]", GROC);
            EscriureString(10, 48, "Dades Llegides", NORMAL);
            EscriureString(10, 42, "100", CARN);
         }
         else {  
            EscriureString(10, 40, "[    ]", GROC);
            EscriureString(10, 47, "Dades Llegides", NORMAL);
            EscriureCar(10, 42, (cont / 10) + 48, CARN);
            EscriureCar(10, 43, (cont % 10) + 48, CARN);  
         }         
      }     
      DormirProces(INTCLK * 5);      
   }
}

// ############################### [ JOC DE PROVES 10 ] ##############################

void aleste(void) {
   char tec;
   int f = 0;

   EscriureCar(6, 3, LINIA2, NORMAL);
   EscriureCar(8, 3, LINIA, NORMAL); EscriureCar(7, 5, QUAD2, NORMAL);
   EscriureCar(7, 4, QUADRAT, NORMAL); posy = 7;
   CrearProces("FOC", fire, 5);   
   for(;;) {
      posy = 7 + f;                
      LlegirTeclat(&tec, 1);
      EscriureCar(6 + f, 3, ' ', NORMAL);
      EscriureCar(8 + f, 3, ' ', NORMAL); EscriureCar(7 + f, 5, ' ', GROC);
      EscriureCar(7 + f, 4, ' ', NORMAL);
      EscriureCar(6 + f, 2, ' ', NORMAL);
      EscriureCar(8 + f, 2, ' ', NORMAL);      
      if (tec == 'Q') { if (f > -4) f--; }
      else if (tec == BACKSPACE) SignalS(0);
      else if (tec == 'A') { if (f < 3) f++; }
      else if (tec == ESPACIO) { CrearProces("BALA", shoot, 5); }
      EscriureCar(6 + f, 3, LINIA2, NORMAL);
      EscriureCar(8 + f, 3, LINIA, NORMAL); EscriureCar(7 + f, 5, QUAD2, NORMAL);
      EscriureCar(7 + f, 4, QUADRAT, NORMAL);      
   }   
}

void fire(void) {
   char foc[]="\x15\x20";
   unsigned int c = 0;
   unsigned int i = 0;
   
   for(;;) {      
      EscriureCar(posy -1, 2, foc[c], BLAU2 + i);
      EscriureCar(posy + 1, 2, foc[c], BLAU2 + i);
      EscriureCar(21, 43, ROMBE, FGRIS + VERMELL);
      EscriureCar(21, 8, ROMBE, FGRIS + VERMELL);
      if ((i == 0) && (c != 0)) i = 3;
      else { if (c != 0) i = 0; }
      c++; c %= 2; 
      Passo();
   }            
}

void randenem(void) {
   for(;;) {
      CrearProces("ENEM", enemic, 5);
      DormirProces(INTCLK * 6);
   }
}   
     
void enemic(void) {
   unsigned int j, collision, ocollision;
   unsigned long loop;
   
   for(j = 78, collision = 0, ocollision = 0;((j > 3) && (collision == 0) && (ocollision == 0)); j--) { 
      if (LSB(QuinCarAtr(3 + (QuiSoc() % 8), j)) == RODONA) collision = 1;     
      else if (LSB(QuinCarAtr(3 + (QuiSoc() % 8), j)) == QUAD2) ocollision = 1;
      else EscriureCar(3 + (QuiSoc() % 8), j, 2, VERM2);
      for (loop = 0; loop < DELAY; loop++);
      if (j < 78) EscriureCar(3 + (QuiSoc() % 8), j + 1, ' ', NORMAL);
      Passo();      
   }
   if ((collision == 0) && (ocollision == 0)) EscriureCar(3 + (QuiSoc() % 8), j + 1, ' ', NORMAL);
   if (ocollision == 1) {
      DestruirProces(2); DestruirProces(4); 
      ClearScreen();
      EscriureString(6, 35, "GAME OVER", NORMAL);
      for (loop = 0; loop < DELAY * 100; loop++);
      SignalS(0);
   }
   DestruirProces(QuiSoc());   
}

void shoot(void) {
   unsigned int j, f, collision;
   unsigned long loop;

   f = posy;
   for(j = 6, collision = 0;((j < COLS - 1) && (collision == 0)); j++) {
      if (LSB(QuinCarAtr(f, j)) == 2) collision = 1;   
      else EscriureCar(f, j, RODONA, NORMAL);
      for (loop = 0; loop < DELAY; loop++);
      if (j > 6) EscriureCar(f, j - 1, ' ', NORMAL);
      Passo();
      
   }
   if (collision == 0) EscriureCar(f, j - 1, ' ', NORMAL);
   DestruirProces(QuiSoc());
}