/* Fitxer: main.c
   Descripció: Programa principal amb les estructures de dades bàsiques                      */
      
#include "cues.h"
#include "Ec.h"
#include "nucli.h"
#include "pantalla.h"
#include "pila.h"
#include "syscalls.h"
#include "proces.h"
#include "procs.h"
#include "semafors.h"

/* ############################################### [ CONSTANTS ] ############################################## */

#define MAX_NUM_PCBS 9
#define MAX_CMD_LEN  32768U

/* ############################################### [ VARIABLES ] ############################################## */

extern INT contBLK_SLEEP, contBLK_KEYB, contREADY;
struct cua cuaRUN, cuaRDY, cuaBLK_SLEEP, cuaBLK_KEYB, cuaPCB;
struct cua *pcuaRUN, *pcuaRDY, *pcuaBLK_SLEEP, *pcuaPCB, *pcuaBLK_KEYB;
struct tPCB PCBS_lliures[MAX_NUM_PCBS];
struct tSEMAFOR taulaSem[MAX_NUM_SEMS];
struct tPCB *procesRUN;
struct tCIRCULAR bufferTeclat;
void (*rsi_ck)();
void (*rsi_tr)();
void (*rsi_kb)();
WORD flags;
char molinet[] = "|/-\\";
char separador[COLS + 1];
char separador2[COLS + 1];
char separador3[COLS + 1];
char prompt[] = "[ SHELL ]:\\>";
char clean[59];

/* ############################################# [ DECLARACIONS ] ############################################# */

void inicialitzarCues(void);
void ClearScreen(void);
void main(void);

/* ########################################## [ INICIALITZAR CUES ] ########################################### 
     - Inicialitzem les cues de RUN, READY, BLOCKED i PCB's lliures                                             */
     
void inicialitzarCues(void) {
   INT i;
   
   pcuaRUN = &cuaRUN;
   pcuaRDY = &cuaRDY;
   pcuaBLK_SLEEP = &cuaBLK_SLEEP;
   pcuaBLK_KEYB = &cuaBLK_KEYB;
   pcuaPCB = &cuaPCB;
   
   Inicialitzar_cua(pcuaRUN);
   Inicialitzar_cua(pcuaRDY);
   Inicialitzar_cua(pcuaBLK_SLEEP);
   Inicialitzar_cua(pcuaBLK_KEYB);
   Inicialitzar_cua(pcuaPCB);
   
   for(i = 0; i < MAX_NUM_PCBS; i++) {
      PCBS_lliures[i].Estat = NIL;
      PCBS_lliures[i].Prioritat = NIL;
      Encuar(pcuaPCB, ((struct item *) &(PCBS_lliures[i]))); 
   } 
}

/* ######################################## [ INICIALITZAR SEMAFORS ] ######################################### 
     - Inicialitzem la taula de semàfors 
                                                                                                                */
void iniTaulaSem(void) {
   int i;
   
   for(i = 0; i < MAX_NUM_SEMS; i++) taulaSem[i].ID = SEM_NIL;
}

/* ###################################### [ INICIALITZAR BUFF TECLAT ] ######################################## 
     - Inicialitzem els punters del buffer teclat                                                              
                                                                                                                */                                                                                                              
void iniBufferTeclat(void) {
   bufferTeclat.ini = 0;
   bufferTeclat.fi = 0;
}

/* ###################################### [ INICIALITZAR GRAFICS ] ############################################ 
     - Dibuixem els gràfics inicials, la shell, cues, opcions                                                               
                                                                                                                */ 
void iniGraphics(void) {
   INT i;
     
   EsborrarPantalla(NORMAL);
   for (i = 0; i < COLS - 22; i++) clean[i] = ' ';
   clean[COLS - 22] = '\0';
   for (i = 0; i < COLS; i++) separador[i] = ' ';
   separador[COLS] = '\0';
   for (i = 0; i < COLS; i++) separador2[i] = '_';
   separador2[COLS] = '\0';
   for (i = 1; i < COLS; i++) separador3[i] = LINIA;
   separador3[0] = LINIA;
   separador3[COLS] = '\0';
      
   EscriureString(0, 0, separador, FCARN);
   EscriureString(20, 0, separador2, NORMAL);
   EscriureString(13, 0, separador3, NORMAL);
   
   EscriureCar(0, 29, ROMBE, FCARN + BLAU2); EscriureCar(0, 30, ROMBE, FCARN + BLAU2);
   EscriureCar(0, 31, ROMBE, FCARN + BLAU2);
   EscriureString(0, 32, " [ PRACTICA SOP ]", FCARN);  
   EscriureCar(0, 50, ROMBE, FCARN + BLAU2); EscriureCar(0, 51, ROMBE, FCARN + BLAU2);
   EscriureCar(0, 52, ROMBE, FCARN + BLAU2);   
      
   EscriureString(15, 0, "_____ [     ] _____", NORMAL);
   EscriureString(20, 0, "___________________", NORMAL);
   EscriureString(15, 8, "RUN", VERD2);
    
   EscriureString(17, 0, " [ ", GROC); EscriureCar(17, 3, '+', CARN);
   EscriureString(17, 4, " ] ", GROC); EscriureString(17, 7, "Proc: ", NORMAL);      
   EscriureString(18, 0, " [ ", GROC); EscriureCar(18, 3, '+', CARN);
   EscriureString(18, 4, " ] ", GROC); EscriureString(18, 7, "ID: ", NORMAL);    
   EscriureString(19, 0, " [ ", GROC); EscriureCar(19, 3, '+', CARN);
   EscriureString(19, 4, " ] ", GROC); EscriureString(19, 7, "Prior: ", NORMAL); 

   EscriureString(15, 20, "____ [       ] ____", NORMAL);
   EscriureString(20, 20, "___________________", NORMAL);
   EscriureString(15, 27, "READY", GROC);
   
   EscriureString(17, 20, " [   ] ", GROC);
   EscriureCar(17, 23, '0', CARN);
   EscriureString(17, 27, "ListID: ", NORMAL);

   EscriureString(15, 41, "__ [           ] __", NORMAL);
   EscriureString(20, 41, "___________________", NORMAL);
   EscriureString(15, 46, "BLK SLEEP", VERM2);  

   EscriureString(17, 41, " [   ] ", GROC);
   EscriureCar(17, 44, '0', CARN);
   EscriureString(17, 48, "ListID: ", NORMAL);
   
   EscriureString(15, 61, "__ [           ] __", NORMAL);
   EscriureString(20, 61, "___________________", NORMAL);
   EscriureString(15, 66, "BLK KEYBD", VERM2);

   EscriureString(17, 61, " [   ] ", GROC);
   EscriureCar(17, 64, '0', CARN);
   EscriureString(17, 68, "ListID: ", NORMAL);
                          
   EscriureString(21, 0, separador, INVERSE);
   EscriureString(24, 0, separador2, NORMAL);
   EscriureString(23, 0, " [ ", GROC); EscriureString(23, 3, ">>>", CARN);
   EscriureString(23, 6, " ] ", GROC); 

   EscriureString(23, 9, prompt, NORMAL);
}

void Help(void) {
   EscriureString(2, 1, "[     ] ", GROC); EscriureString(2, 3, "R 0", CARN);
   EscriureString(2, 9, "Comprovacio del FIFO i crida Passo", NORMAL);
   EscriureString(2, 60, "[   ] ", GROC); EscriureString(2, 62, "K", CARN);
   EscriureString(2, 66, "Kill all", NORMAL);
   EscriureString(3, 60, "[   ] ", GROC); EscriureString(3, 62, "H", CARN);
   EscriureString(3, 66, "Help", NORMAL);
   EscriureString(4, 60, "[   ] ", GROC); EscriureString(4, 62, "Q", CARN);
   EscriureString(4, 66, "Quit", NORMAL);
   EscriureString(3, 1, "[     ] ", GROC); EscriureString(3, 3, "R 1", CARN);
   EscriureString(3, 9, "Comprovacio de prioritats i de l'apropiacio", NORMAL);
   EscriureString(4, 1, "[     ] ", GROC); EscriureString(4, 3, "R 2", CARN);
   EscriureString(4, 9, "Comprovacio de la reutilitzacio dels PCB's", NORMAL);
   EscriureString(5, 1, "[     ] ", GROC); EscriureString(5, 3, "R 3", CARN);
   EscriureString(5, 9, "Comprovacio de la crida DormirProces", NORMAL);  
   EscriureString(6, 1, "[     ] ", GROC); EscriureString(6, 3, "R 4", CARN);
   EscriureString(6, 9, "Comprovacio del comportament dels semafors", NORMAL); 
   EscriureString(7, 1, "[     ] ", GROC); EscriureString(7, 3, "R 5", CARN);
   EscriureString(7, 9, "Comprovacio del comportament dels semafors n-aris", NORMAL);
   EscriureString(8, 1, "[     ] ", GROC); EscriureString(8, 3, "R 6", CARN);
   EscriureString(8, 9, "Problema dels filosofs", NORMAL); 
   EscriureString(9, 1, "[     ] ", GROC); EscriureString(9, 3, "R 7", CARN);
   EscriureString(9, 9, "Comprovacio del comportament del teclat", NORMAL); 
   EscriureString(10, 1, "[     ] ", GROC); EscriureString(10, 3, "R 8", CARN);
   EscriureString(10, 9, "Comprovacio del Buffering de teclat i la insercio de la tecla <CR>", NORMAL); 
   EscriureString(11, 1, "[     ] ", GROC); EscriureString(11, 3, "R 9", CARN);
   EscriureString(11, 9, "Altres comprovacions", NORMAL);
}

void ClearScreen(void) {
   INT i, j;
   for(i = 1; i < 13; i++) {
      for(j = 0; j < COLS; j++) {
         EscriureCar(i, j, ' ', NORMAL);
      }
   }
}

/* ######################################## [ INTERRUPCIÓ SOFTWARE ] ########################################## */

void trap(void) {

   procesRUN->SS_SP = salvar_Context();
   
   /* ##### Identificador de la crida en el registre AX => SS_SP + 8 bytes ##### */
   switch(*(((WORD *) (procesRUN->SS_SP)) + 4)) { 
   
      case 1: CrearProces_K();
              break;

      case 2: QuiSoc_K(); 
              break;

      case 3: Passo_K(); 
              break;

      case 4: DormirProces_K(); 
              break;
                            
      case 5: DestruirProces_K(); 
              break;
      
      case 6: IniSem_K();
              break;

      case 7: WaitS_K();
              break;

      case 8: SignalS_K();
              break;

      case 9: ElimSem_K();
              break;

      case 10: EscriurePantalla_K();
               break;

      case 11: LlegirTeclat_K();
               break;

      case 12: EscriurePantalla2_K();
               break;
                                                                                                 
      default: break;
   }
   EOI();
   restaurar_Context(procesRUN->SS_SP);
}

void kill_all(void) {
   INT i, insertar = 0;
   struct tPCB * procNull, * PCBaux;   
   WORD flags;   
   
   flags = Inhibir();
   
   // Alliberar PCB's Bloquejats per un semàfor
   for(i = 0; i < MAX_NUM_SEMS; i++) {
      if (taulaSem[i].ID != SEM_NIL) {
         while(!Buida(&taulaSem[i].cuaBLK)) 
            Encuar(pcuaPCB, Primer(&taulaSem[i].cuaBLK));      
            
         taulaSem[i].ID = SEM_NIL;
      }
   }
   
   // Alliberar PCB's de la Cua de READY
   while(!Buida(pcuaRDY)) {
      PCBaux = (struct tPCB *) Primer(pcuaRDY);
      if (PCBaux->ID == 0) {
         procNull = PCBaux;
         insertar = 1;
      }
      else Encuar(pcuaPCB, (struct item *) PCBaux);
   }
   if (insertar) Encuar(pcuaRDY, (struct item *) procNull);
   contREADY = 1;

   // Alliberar PCB's de la Cua de BLK_SLEEP [ Ni procés null ni Shell poden estar dormits ]
   while (!Buida(pcuaBLK_SLEEP)) Encuar(pcuaPCB, Primer(pcuaBLK_SLEEP));      
   contBLK_SLEEP = 0;

   // Alliberar PCB's de la Cua de BLK_KEYB [ Ni procés null ni Shell poden estar BLK_KEYB ]
   // - Shell no pot estar Bloquejat ja que és ella qui executa la comanda Kill_all => RUN
   while (!Buida(pcuaBLK_KEYB)) Encuar(pcuaPCB, Primer(pcuaBLK_KEYB));
   contBLK_KEYB = 0; 

   idproces = 2;   
   Desinhibir(flags);            
}

void shell(void) {
   static int i = 0;
   static char cmd[MAX_CMD_LEN];
   
   EscriureString(21, 9, " Proces Shell->ID [   ] [   ]", INVERSE);
   EscriureCar(21, 29, QuiSoc() + 48, INVERSE);
      
   for(;;) {     
      EscriurePantalla2(&molinet[i=(i+1) % 4], 21, 35, 1, INVERSE);
            
      LlegirTeclat(&cmd[0], MAX_CMD_LEN);
      EscriureCar(21, 43, ROMBE, FGRIS + VERMELL);
      EscriureCar(21, 8, ROMBE, FGRIS + VERD);
            
      switch(cmd[0]) {      
         case 'R': switch(cmd[1]) {
                      case ' ': switch(cmd[2]) {
                                   case '0': switch(cmd[3]) {
                                                case '\r': kill_all(); ClearScreen();
                                                           CrearProces("PROC1", proc1, 2);
                                                           CrearProces("PROC2", proc1, 2);
                                                           CrearProces("PROC3", proc1, 2);
                                                           CrearProces("PROC4", proc1, 2);
                                                           CrearProces("PROC5", proc1, 2);
                                                           CrearProces("PROC6", proc1, 2);
                                                           break;
                                                           
                                                default:   break; 
                                             }                                                
                                             break;

                                   case '1': switch(cmd[3]) {
                                                case '\r': kill_all(); ClearScreen();
                                                           CrearProces("JOCP2", jproves2, 8);
                                                           break;
                                                           
                                                default:   break; 
                                             }                                                
                                             break;
                                             
                                   case '2': switch(cmd[3]) {
                                                case '\r': kill_all(); ClearScreen();
                                                           CrearProces("JOCP3", jproves3, 8);
                                                           break;
                                                           
                                                default:   break; 
                                             }                                                
                                             break;

                                   case '3': switch(cmd[3]) {
                                                case '\r': kill_all(); ClearScreen();
                                                           CrearProces("PROC1", procDorm15, 5);
                                                           CrearProces("PROC2", procDorm20, 5);
                                                           CrearProces("PROC3", procDorm10, 5);
                                                           break;
                                                           
                                                default:   break; 
                                             }                                                
                                             break;

                                   case '4': switch(cmd[3]) {
                                                case '\r': kill_all(); ClearScreen();
                                                           IniSem(0, -1);
                                                           CrearProces("PROC1", procSem, 5);
                                                           CrearProces("PROC2", procSem, 5);
                                                           CrearProces("PROC3", procSem, 5);
                                                           CrearProces("PROC4", procSem, 5);
                                                           CrearProces("PROC5", procSem, 5);
                                                           break;
                                                           
                                                default:   break; 
                                             }                                                
                                             break;

                                   case '5': switch(cmd[3]) {
                                                case '\r': kill_all(); ClearScreen();
                                                           IniSem(1, 2);
                                                           CrearProces("PROC1", procSem2, 5);
                                                           CrearProces("PROC2", procSem2, 5);
                                                           CrearProces("PROC3", procSem2, 5);
                                                           CrearProces("PROC4", procSem2, 5);
                                                           CrearProces("PROC5", procSem2, 5);
                                                           break;
                                                           
                                                default:   break; 
                                             }                                                
                                             break;

                                   case '6': switch(cmd[3]) {
                                                case '\r': kill_all(); ClearScreen();
                                                           IniSem(0, 1); IniSem(1, 1);
                                                           IniSem(2, 1); IniSem(3, 1);
                                                           IniSem(4, 1); IniSem(5, 1);
                                                           CrearProces("FILO1", filosofs, 5);
                                                           CrearProces("FILO2", filosofs, 5);
                                                           CrearProces("FILO3", filosofs, 5);
                                                           CrearProces("FILO4", filosofs, 5);
                                                           CrearProces("FILO5", filosofs, 5);
                                                           CrearProces("FILO6", filosofs, 5);
                                                           break;
                                                           
                                                default:   break; 
                                             }                                                
                                             break;

                                   case '7': switch(cmd[3]) {
                                                case '\r': kill_all(); ClearScreen();
                                                           IniSem(0, 0);
                                                           CrearProces("PTEC1", procTec, 5);
                                                           CrearProces("PTEC2", procTec, 5);
                                                           CrearProces("PTEC3", procTec, 5);
                                                           WaitS(0);
                                                           kill_all(); ClearScreen(); Help();
                                                           break;
                                                           
                                                default:   break; 
                                             }                                                
                                             break;

                                   case '8': switch(cmd[3]) {
                                                case '\r': kill_all(); ClearScreen();
                                                           IniSem(0, 0);
                                                           CrearProces("PTEC1", procTec2, 5);
                                                           CrearProces("PTEC2", procTec3, 5);
                                                           CrearProces("PTEC3", procTec4, 5);
                                                           WaitS(0);
                                                           kill_all(); ClearScreen(); Help();
                                                           break;
                                                           
                                                default:   break; 
                                             }                                                
                                             break;                                           

                                   case '9': switch(cmd[3]) {
                                                case '\r': kill_all(); ClearScreen();
                                                           IniSem(0, 0);
                                                           CrearProces("P1", aleste, 7);
                                                           CrearProces("RNDE", randenem, 6);
                                                           WaitS(0);
                                                           kill_all(); ClearScreen(); Help();
                                                           break;
                                                           
                                                default:   break; 
                                             }                                                
                                             break; 
                                                                                                                                                                                                                                                                                                                                                                                                                     
                                   default:  break; 
                                } 
                                break;
                      
                      default: break;
                   }
                   break;                                           
         
         case 'H': switch(cmd[1]) {
                      case '\r': kill_all(); ClearScreen(); Help(); break;
                      default:   break;
                   }
                   break;

         case 'K': switch(cmd[1]) {
                      case '\r': kill_all(); ClearScreen(); Help(); break;
                      default:   break;
                   }
                   break;
                      
         case 'Q': switch (cmd[1]) {
                      
                      case '\r': kill_all();
                                 EsborrarPantalla(NORMAL);
                                 flags=Inhibir(); 
                                 CanviarVectInt(INT_CK, rsi_ck);
                                 CanviarVectInt(INT_TR, rsi_tr);
                                 CanviarVectInt(INT_KB, rsi_kb);
                                 Desinhibir(flags);                   
                                 exit(0);
                                 break;
                                 
                      default:   break;
                   }
                   break;   
      
         default: break;
      }                     
   }   
}

void Null(void) {
   static int i = 0;

   CrearProces("Shell", shell, 9);
   
   EscriureString(21, 44, " Proces Null->ID [   ] [   ]", INVERSE);
   EscriureCar(21, 63, QuiSoc() + 48, INVERSE);
       
   for(;;) {
      EscriureCar(21, 43, ROMBE, FGRIS + VERD);
      EscriureCar(21, 8, ROMBE, FGRIS + VERMELL);
      EscriurePantalla2(&molinet[i=(i+1) % 4], 21, 69, 1, INVERSE);
   } 
           
}
     
void CrearProcesInicial(void) {
   struct tPCB * PCB;
   
   PCB = ((struct tPCB *) Primer(pcuaPCB));
   PCB->Proces = "Null";
   PCB->ID = ID_PROCES_NULL;
   PCB->Estat = RDY;
   PCB->Prioritat = 0;

   PCB->SS_SP = inicialitzar_Stack(Null, Segment(PCB->Pila), Desplacament(&(PCB->Pila[MIDA_PILA-1])));   
   Inserir(pcuaRDY, ((struct item *) PCB), PCB->Prioritat);
   procesRUN = PCB;     
}

void main(void) {     
   
   BufferPantalla();  
   inicialitzarCues();
   iniTaulaSem();
   iniBufferTeclat();
   iniGraphics();
   Help();
   
   CrearProcesInicial();

   // Encuem el primer proces de la cua de READY a RUN
   procesRUN = ((struct tPCB *) Primer(pcuaRDY));
   procesRUN->Estat = RUN;
   Encuar(pcuaRUN,(struct item *) procesRUN); 
         
   // Subsituim la RSI del rellotge per la funció multiplexar
   // Subsituim la RSI del teclat per la funció teclat    
   flags=Inhibir();
   rsi_ck = CanviarVectInt(INT_CK, multiplexar);
   rsi_tr = CanviarVectInt(INT_TR, trap);
   rsi_kb = CanviarVectInt(INT_KB, teclat);
   Desinhibir(flags);     
   
   restaurar_Context(procesRUN-> SS_SP);
       
   CanviarVectInt(INT_CK, rsi_ck);
   CanviarVectInt(INT_TR, rsi_tr);
   CanviarVectInt(INT_KB, rsi_kb);  
}