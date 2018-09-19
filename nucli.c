/* Fitxer: nucli.c
   Descripci�: Kernel del sistema operatiu, gesti� de processos 
              (Planificador FCFS apropiatiu i amb prioritats, Crides al sistema)             */
      
#include "cues.h"
#include "Ec.h"
#include "input.h"
#include "main.h"
#include "pantalla.h"
#include "pila.h"
#include "proces.h"
#include "semafors.h"

/* ############################################### [ CONSTANTS ] ############################################## */

#define MAX_NUM_ID 32768U

/* ############################################### [ VARIABLES ] ############################################## */

extern struct tSEMAFOR taulaSem[MAX_NUM_SEMS];
struct tPCB * PCBaux;
INT num_elems;
INT idproces = 1;
INT contBLK_SLEEP = 0, contBLK_KEYB = 0, contREADY = 0, auxcont;
INT num_linia = 0;
INT lencmd = 0;
INT cont = 0, cont2 = 0;
char arrows[6] = ">>>   ";

/* ############################################# [ DECLARACIONS ] ############################################# */

void CrearProces_K(struct tCONTEXT c, char * nomProces, void (*codi)(), INT prioritat);
void QuiSoc_K(struct tCONTEXT c);
void Passo_K(struct tCONTEXT c);
int PassarCPU(unsigned int prioritat);
void DormirProces_K(struct tCONTEXT c, long ntics);
void DestruirProces_K(struct tCONTEXT c, unsigned int identificador);
int IniSem_K(struct tCONTEXT c, unsigned int ID, int valor);
int WaitS_K(struct tCONTEXT c, unsigned int ID);
int SignalS_K(struct tCONTEXT c, unsigned int ID);
int ElimSem_K(struct tCONTEXT c, unsigned int ID);
int EscriurePantalla_K(struct tCONTEXT c, char * string, unsigned int longitud);
int LlegirTeclat_K(struct tCONTEXT c, char * string, unsigned int longitud);
struct tPCB * BuscarProces(struct cua * cua, unsigned int identificador);
void multiplexar (void);
void PaintCuaReady(void);
void PaintCuaBLK_SLEEP(void);
void PaintCuaBLK_KEYB(void);
void BorrarGraphicRDY(void);
void BorrarGraphicBLK_SLEEP(void);
void BorrarGraphicBLK_KEYB(void);
void DespertarProcesos(void);
void teclat(void);
void DesbloquejarProcesos(void);
struct tPCB * scheduler(void);
void dispatcher(struct tPCB * pcbRUN);
void trap(void);


              /*      #######################################################################
                      ##########                                                   ##########
                     ##########              CRIDES AL SISTEMA (NUCLI)            ##########
                    ##########                                                   ##########
                   #######################################################################      */
                   
/* ############################################# [ CREAR PROCES ] #############################################
   %%% Crida al sistema 1: Asignem una estructura PCB a un programa => Creaci� Proc�s %%% 
     - Asignem un PCB al proc�s en el cas de que la cua de PCB's lliures no estigui buida
     - Asignem un nom al proc�s
     - Asignem una Identitat al proc�s
     - Fiquem el proc�s en estat READY
     - Posem la prioritat donada per parametre al proc�s
     - Asignem el Segment de codi i el desplacament al proc�s
     - Inicialitzem la pila del proc�s (Codi en asm -> pila.asm)
     - Finalment l'inserim a la cua de READY o en la cua de RUN segons si es produeix apropiaci�
     - Retornem la Identitat del proc�s:
          - Retornem -1 en el cas de que la cua de PCB's lliures estigui buida                              
          - Retornem el ID del proces en el cas contrari                                                        */
                                                                                                                                               
void CrearProces_K(struct tCONTEXT c, char * nomProces, void (*codi)(), INT prioritat) {
   
   if (Buida(pcuaPCB)) c.AX = -1;
   else {   
      PCBaux = ((struct tPCB *) Primer(pcuaPCB));
      PCBaux->Proces = nomProces;
      PCBaux->ID = idproces % MAX_NUM_ID; idproces++;
      PCBaux->Estat = RDY;
      PCBaux->Prioritat = prioritat;
   
      PCBaux->SS_SP = inicialitzar_Stack(codi, Segment(PCBaux->Pila), Desplacament(&(PCBaux->Pila[MIDA_PILA-1])));
   
      /* Comparem les prioritats per veure si el proc�s creat s'apropia de la CPU */
      if (PCBaux->Prioritat <= procesRUN->Prioritat) { 
         Inserir(pcuaRDY, ((struct item *) PCBaux), PCBaux->Prioritat);
         contREADY++;
         c.AX = (int) PCBaux->ID;
         // treure warning "parameter c is never used..."
         c.AX = c.AX;
      }  
      else {
         procesRUN->Estat = RDY;
         Inserir(pcuaRDY, Primer(pcuaRUN), procesRUN->Prioritat);
         contREADY++;
         c.AX = (int) PCBaux->ID;
         dispatcher(PCBaux);
      }
   }                       
}


/* ############################################# [ QUI SOC ] ##################################################
   %%% Crida al sistema 2: Metainformaci� sobre el identificador del proc�s %%%
     - Retornem la Identitat del proc�s                                                                         */
     
void QuiSoc_K(struct tCONTEXT c) { 
   c.AX = (int) procesRUN->ID; 
   // treure warning "parameter c is never used..."
   c.AX = c.AX;
}


/* ############################################# [ PASSO ] ####################################################
   %%% Crida al sistema 3: Passem el control a un altre proc�s que tingui la mateixa prioritat %%%     
     - Mirem si hi ha un proc�s amb la mateixa prioritat en la cua de READY
       - En cas afirmatiu encuem el proc�s a la cua de READY 
       - Cridem al scheduler i dispatcher
     - Planificador apropiatiu => proc�s RUN m�xima prioritat => Primer(pcuaRDY) en cas de passar el control
     - Retornem  0 en el cas que poguem executar un altre proc�s amb la mateixa prioritat                                                
     - Retornem -1 en cas contrari                                                                              */

void Passo_K(struct tCONTEXT c) {
     
   /* Si existeix un altre proc�s amb la mateixa prioritat passem la CPU al proc�s */
   if (PassarCPU(procesRUN->Prioritat)) {
      procesRUN->Estat = RDY;
      Inserir(pcuaRDY, Primer(pcuaRUN), procesRUN->Prioritat);
      contREADY++;
      c.AX = 0;
      // treure warning "parameter c is never used..."
      c.AX = c.AX;
      dispatcher(scheduler());
   }
   c.AX = -1;
}                           

/* Funci� PassarCPU    - Retorna Cert en el cas de que trobem un proces amb la mateixa prioritat
                         en la cua de READY
                       - Retorna Fals en cas contrari                                                           */
                       
int PassarCPU(unsigned int prioritat) {
   if (!Buida(pcuaRDY)) {
      PCBaux = (struct tPCB *) pcuaRDY->primer;
      if (PCBaux == NIL) return 0;
      else {
         while ((PCBaux->Prioritat != prioritat) && (PCBaux != (struct tPCB *) pcuaRDY->ultim)) {
            PCBaux = (struct tPCB *) PCBaux->Elem.seguent;           
         }
         if (PCBaux->Prioritat == prioritat) return 1;
         else return 0;
      }                           
   }
   else return 0;
}


/* ######################################### [ DORMIR PROCES ] ################################################
     %%% Crida al sistema 4: Dormim un proc�s N tics de rellotge (interrupci� 0x8 (multiplexar)) %%%
     - Asignem un quantum (passat per par�metre) al proc�s que sol.licita dormir-se
     - Fiquem el proc�s en la cua de BLOCKED per SLEEP
     - Desencuem el proc�s de la cua de RUN                                                                     */
     
void DormirProces_K(struct tCONTEXT c, long ntics) {
   procesRUN->Quantum = ntics;
   procesRUN->Estat = BLK;
   Encuar(pcuaBLK_SLEEP, Primer(pcuaRUN));
   contBLK_SLEEP++;
   // treure warning "parameter c is never used..."
   c.AX = c.AX;
   dispatcher(scheduler());   
}
              
/* ######################################## [ DESTRUIR PROCES ] ###############################################
   %%% Crida al sistema 5: Destruim un proc�s amb l'identificador passat per par�metre %%%
     - Mirem si el proc�s a destruir �s el proc�s NUL, si es aix� evitem la seva destrucci�
     - Mirem si el proc�s a destruir �s el proc�s que esta en la cua de RUN
       - En cas afirmatiu el treiem de la cua de RUN
       - Encuem el PCB a la cua de PCB's lliures
       - Cridem al scheduler i dispatcher per escollir el seguent proc�s a executar
     - Mirem si el proc�s a destruir esta en la cua de READY o BLOCKED SLEEP
       - En cas afirmatiu el treiem de la cua de READY o BLOCKED SLEEP
       - Encuem el PCB a la cua de PCB's lliures
     - Retornem  0 si trobem el proc�s a eliminar
     - Retornem -1 si el proc�s a eliminar no existeix
     - Retornem -2 si el proc�s a eliminar �s el proc�s NULL                                                    */
       
void DestruirProces_K(struct tCONTEXT c, unsigned int identificador) {

   if (identificador == ID_PROCES_NULL) c.AX = -2;
   else {
      /* El proc�s a destruir �s el que est� en execuci� */   
      if (procesRUN->ID == identificador) {
         procesRUN->Estat = NIL;
         procesRUN->Prioritat = NIL;
         Encuar(pcuaPCB, Primer(pcuaRUN));
         c.AX = 0;
         dispatcher(scheduler());
      }
      /* Mirem si el proc�s a destruir est� en la cua de READY */
      else {
         PCBaux = BuscarProces(pcuaRDY, identificador);
         if (PCBaux != NIL) {
            PCBaux->Estat = NIL;
            PCBaux->Prioritat = NIL;
            Encuar(pcuaPCB, Extirpar(pcuaRDY, (struct item *) PCBaux));
            contREADY--;
            c.AX = 0;                              
         }
         else {
            /* Mirem si el proc�s a destruir est� en la cua de BLOCKED SLEEP */
            PCBaux = BuscarProces(pcuaBLK_SLEEP, identificador);
            if (PCBaux != NIL) {
               PCBaux->Estat = NIL;
               PCBaux->Prioritat = NIL;
               Encuar(pcuaPCB, Extirpar(pcuaBLK_SLEEP, (struct item *) PCBaux));
               contBLK_SLEEP--;
               c.AX = 0;
               // treure warning "parameter c is never used..."
               c.AX = c.AX;                              
            }
            else {
               /* Mirem si el proc�s a destruir est� en la cua de BLOCKED KEYB */
               PCBaux = BuscarProces(pcuaBLK_KEYB, identificador);
               if (PCBaux != NIL) {
                  PCBaux->Estat = NIL;
                  PCBaux->Prioritat = NIL;
                  Encuar(pcuaPCB, Extirpar(pcuaBLK_KEYB, (struct item *) PCBaux));
                  contBLK_SLEEP--;
                  c.AX = 0;
               }                                   
               else c.AX = -1;
            }
         }                          
      }           
   }                 
}

struct tPCB * BuscarProces(struct cua * cua, unsigned int identificador) {
         
   if (!Buida(cua)) {
      PCBaux = (struct tPCB *) cua->primer;
      while ((PCBaux->ID != identificador) && (PCBaux != (struct tPCB *) cua->ultim)) {
         PCBaux = (struct tPCB *) PCBaux->Elem.seguent;           
      }
      if (PCBaux->ID == identificador) return PCBaux;
      else return NIL;
   }             
   else return NIL;
}
   
                                  
/* #################################### [ INI SEM ] ###########################################################
   %%% Crida al sistema 6: Inicialitzem el sem�for amb el valor passat per par�metre %%%
     - Mirem que l'ID del sem�for no sigui el sem�for centinella (65535)
       - En cas afirmatiu recorrem la taula de sem�fors i busquem el primer sem�for lliure
       - Si trobem el sem�for lliure
         - Asignem els valors passats per par�metre a la estructura del sem�for
           - Asignem l'ID
           - Asignem el valor del sem�for
         - Inicialitzem la cua del sem�for
         - Retornem 0
       - Si el sem�for ja ha sigut previament asignat retornem -2
       - Si no trobem cap sem�for lliure retornem -1                                                            */ 
   
int IniSem_K(struct tCONTEXT c, unsigned int ID, int valor) {
   /* No fa falta que siguin variables est�tiques 
      no provoquem canvi de context               */ 
   WORD flags;
   int i, estat = 0;
   
   flags = Inhibir();
   if (ID != SEM_NIL) {
      for(i = 0; ((i < MAX_NUM_SEMS) && (estat == 0)); i++) {
         if (taulaSem[i].ID == SEM_NIL) {
            taulaSem[i].ID = ID;
            taulaSem[i].valor = valor;
            Inicialitzar_cua(&taulaSem[i].cuaBLK);
            estat = 1;
         }
         else if (taulaSem[i].ID == ID) estat = 2;
      }
   }
   /* S'ha recorregut tota la llista i no s'ha trobat cap sem�for lliure
      o s'ha intentat asignar el sem�for centinella (SEM_NIL)            */
   if (estat == 0) {
      Desinhibir(flags);
      c.AX = -2;
      return -2;         
   }
   // S'ha trobat un sem�for lliure
   else if (estat == 1) {
      Desinhibir(flags);
      c.AX = 0;
      // treure warning "parameter c is never used..."
      c.AX = c.AX;      
      return 0;
   }
   // estat = 2 -> El sem�for ja estava inicialitzat 
   else { Desinhibir(flags); c.AX = -1; return -1; }                                                       
} 


/* ######################################### [ WAIT SEMAFOR ] #################################################
   %%% Crida al sistema 7: Bloquejem el proc�s si el sem�for t� el valor 0, analogament el decrementem %%%
     - Mirem que l'ID del sem�for no sigui el sem�for centinella (65535)
       - En cas afirmatiu recorrem la taula de sem�fors i busquem el sem�for amb l'ID passat per par�metre
       - Si trobem el sem�for
         - Mirem el valor del sem�for per decidir si decrementem o bloquejem el proc�s
       - Retornem -1 en el cas de no trobar el sem�for o si el sem�for es el centinella
       - Retornem 0 en el cas contrari                                                                          */   
          
int WaitS_K(struct tCONTEXT c, unsigned int ID) {
   static int i, index = -1;
   static WORD flags;

   flags = Inhibir();
   if (ID != SEM_NIL) { 
      for(i = 0; i < MAX_NUM_SEMS; i++) { if (taulaSem[i].ID == ID) index = i; } 
   }
   // El sem�for no s'ha trobat o s'ha intentat fer el wait al sem�for centinella
   if (index == -1) { Desinhibir(flags); c.AX = -1; return -1; }
   // Bloquejem el proc�s
   if (taulaSem[index].valor == 0) {
      procesRUN->Estat = BLK;
      Encuar(&taulaSem[index].cuaBLK, Primer(pcuaRUN));
      dispatcher(scheduler());               
   }
   // Decrementem el valor del sem�for
   else taulaSem[index].valor--;
   Desinhibir(flags);
   c.AX = 0;
   // treure warning "parameter c is never used..."
   c.AX = c.AX;
   return 0;
}    


/* ######################################### [ SIGNAL SEMAFOR ] ###############################################
   %%% Crida al sistema 8: Desbloquejem el primer proc�s si la cua del sem�for no esta buida %%%
     - Mirem que l'ID del sem�for no sigui el sem�for centinella (65535)
       - En cas afirmatiu recorrem la taula de sem�fors i busquem el sem�for amb l'ID passat per par�metre
       - Si trobem el sem�for
         - Mirem si la cua del sem�for es buida per decidir si tenim que incrementar el sem�for 
           o desbloquejar el primer proc�s de la cua
       - Si no esta buida la cua, debloquejem el primer proc�s i mirem si es crea apropiaci� -> BLK a RUN
       - Retornem -1 si s'ha produ�t un error
       - Retornem 0 en cas contrari                                                                             */
       
int SignalS_K(struct tCONTEXT c, unsigned int ID) {
   static int i, index = -1;
   static WORD flags;
   
   flags = Inhibir();
   if (ID != SEM_NIL) { 
      for(i = 0; i < MAX_NUM_SEMS; i++) { if (taulaSem[i].ID == ID) index = i; } 
   }  
   // El sem�for no s'ha trobat o s'ha intentat fer el signal al sem�for centinella
   if (index == -1) { Desinhibir(flags); c.AX = -1; return -1; }
   if (!Buida(&taulaSem[index].cuaBLK)) {
      PCBaux = ((struct tPCB *) ((&taulaSem[index].cuaBLK)->primer));
      /* Comparem les prioritats per veure si el proc�s s'apropia de la CPU
         - Pasem de BLK -> RUN sense pasar per RDY                                 */
      if (procesRUN->Prioritat < PCBaux->Prioritat) {
         procesRUN->Estat = RDY;
         Inserir(pcuaRDY, Primer(pcuaRUN), procesRUN->Prioritat); contREADY++;
         dispatcher((struct tPCB *) Primer(&taulaSem[index].cuaBLK));    
      }
      else {
         PCBaux->Estat = RDY; 
         Inserir(pcuaRDY, Primer(&taulaSem[index].cuaBLK), PCBaux->Prioritat);
         contREADY++;
      }
   }
   // Incrementem el valor del sem�for
   else taulaSem[index].valor++; 
   Desinhibir(flags);
   c.AX = 0;
   // treure warning "parameter c is never used..."
   c.AX = c.AX;
   return 0;                                 
}


/* ######################################### [ ELIMINAR SEMAFOR ] #############################################
   %%% Crida al sistema 9: Eliminem el sem�for amb l'ID per par�metre %%%
     - Mirem que l'ID del sem�for no sigui el sem�for centinella (65535)
       - En cas afirmatiu recorrem la taula de sem�fors i busquem el sem�for amb l'ID passat per par�metre
       - Si trobem el sem�for 
         - Mirem si la cua del sem�for es buida per decidir si podem borrar el sem�for
       - Retornem -1 si s'ha produ�t l'error de no trobat el sem�for
       - Retornem -2 si el sem�for t� alg�n proc�s a la cua de bloquejats
       - Retornem 0 en cas contrari                                                                             */
       
int ElimSem_K(struct tCONTEXT c, unsigned int ID) {
   /* No fa falta que siguin variables est�tiques 
      no provoquem canvi de context               */ 
   int i, index = -1;
   WORD flags;
   
   flags = Inhibir();
   if (ID != SEM_NIL) {
      for(i = 0; i < MAX_NUM_SEMS; i++) { if (taulaSem[i].ID == ID) index = i; }
   }
   // El sem�for no s'ha trobat o s'ha intentat eliminar el sem�for centinella
   if (index == -1) { Desinhibir(flags); c.AX = -1; return -1; }
   if (!Buida(&taulaSem[index].cuaBLK)) { Desinhibir(flags); c.AX = -2; return -2; }
   taulaSem[i].ID = SEM_NIL;
   Desinhibir(flags);
   c.AX = 0;
   // treure warning "parameter c is never used..."
   c.AX = c.AX;
   return 0;       
}

/* ######################################### [ ESCRIURE PANTALLA ] ############################################ 
   %%% Crida al sistema 10: Escrivim en la pantalla l'string passat per par�metre %%%                           
     - Escrivim l'string comen�ant a la columna 0 i fila corresponent     
     - Retornem 0                                                                                               */  
     
int EscriurePantalla_K(struct tCONTEXT c, char * string, unsigned int longitud) {
   int i;
   
   num_linia = ((procesRUN->ID) % ROWS);
   num_linia += 4;
    
   for(i = 0; i < longitud; i++) {
      EscriureCar(num_linia, i, *(string + i), 7);
   }
   c.AX = 0;
   // treure warning "parameter c is never used..."
   c.AX = c.AX;
   
   return 0;
}

/* ######################################### [ LLEGIR TECLAT ] ############################################ 
   %%% Crida al sistema 11: Llegim el nombre de tecles especificat pel par�metre longitud %%%                       
     - En el cas que es desitji llegir m�s tecles que el nombre de tecles del buffer i 
       no s'hagi apretat el return el proc�s es bloquijar�            
     - Retornem el nombre de tecles llegides                                                                    */

int LlegirTeclat_K(struct tCONTEXT c, char * string, unsigned int longitud) {
   static charac; 
   static int i;
   static int Teclaret;
   
   charac = 0; Teclaret = 0;
   
   num_elems = bufferTeclat.fi - bufferTeclat.ini;
   if (num_elems > TAM_BUFFER) num_elems = TAM_BUFFER;
   
   for (i = 0;((i < num_elems) && (Teclaret == 0)); i++) { 
      if (bufferTeclat.buffer[(bufferTeclat.ini + i) % TAM_BUFFER] == RETURN) Teclaret = 1;
   }

   if ((num_elems >= longitud) || (Teclaret == 1)) {
      i = 0;           
      while ((i < longitud) && (charac != RETURN)) {
         charac = bufferTeclat.buffer[bufferTeclat.ini % TAM_BUFFER];
         string[i] = charac;
         bufferTeclat.ini++;
         i++;           
      }                                       
   }
   else {
      procesRUN->Estat = BLK;
      procesRUN->Ftec = longitud;
      procesRUN->BufferDesti = string;
      Encuar(pcuaBLK_KEYB, Primer(pcuaRUN));
      contBLK_KEYB++;
      dispatcher(scheduler());   
   }

   c.AX = i;
   // treure warning "parameter c is never used..."
   c.AX = c.AX;
   
   return i;                
}                              

/* ######################################### [ ESCRIURE PANTALLA2 ] ############################################ 
   %%% Crida al sistema 12: Escrivim en la pantalla l'string passat per par�metre %%%                           
     - Escrivim l'string comen�ant a la columna i fila corresponent     
     - Retornem 0                                                                                               */  
     
int EscriurePantalla2_K(struct tCONTEXT c, char * string, unsigned int fila, unsigned int col, unsigned int longitud, unsigned int atr) {
   int i;

   for(i = 0; i < longitud; i++) {
      EscriureCar(fila, col + i, *(string + i), atr);
   }
   c.AX = 0;
   // treure warning "parameter c is never used..."
   c.AX = c.AX;
   
   return 0;
}
      
               /*      #######################################################################
                      ##########                                                   ##########
                     ##########          PLANIFICADOR FCFS APROPIATIU             ##########
                    ##########                                                   ##########
                   #######################################################################      */
                   
/* ############################################# [ MULTIPLEXAR ] ##############################################
   %%% Interrupci� 0x8 (Interrupci� del rellotge) %%%   
     - Salvem el context del proces que esta executant-se
     - Decrementem el quantum 1 unitat a cada proc�s que esta BLOCKED per SLEEP
     - Mirem si es produeix apropiaci� => BLOCKED -> RUN quan el quantum es 0
     - Restaurem el context del proc�s                                                                          */
   
void multiplexar(void) {     

   procesRUN->SS_SP = salvar_Context();
   if (cont == 0) { EscriureCar(23, 22 + lencmd, QUADRAT, GROC); cont++; } 
   else { EscriureCar(23, 22 + lencmd, ' ', GROC); cont = 0; }

   if (cont2 != 6) { EscriureCar(23, 3 + (cont2 % 3), arrows[cont2], CARN); cont2++; }
   else { cont2 = 0; EscriureCar(23, 3 + (cont2 % 3), arrows[cont2], CARN); cont2++; } 
      
               /* Forma Gr�fica de la cua de READY */
   if (!Buida(pcuaRDY)) { BorrarGraphicRDY(); PaintCuaReady(); }
   else { 
      EscriureCar(17, 23, contREADY + 48, CARN);
      BorrarGraphicRDY();
   }

               /* Forma Gr�fica de la cua de BLK SLEEP */
   if (!Buida(pcuaBLK_SLEEP)) { BorrarGraphicBLK_SLEEP(); PaintCuaBLK_SLEEP(); }
   else { 
      EscriureCar(17, 44, contBLK_SLEEP + 48, CARN);
      BorrarGraphicBLK_SLEEP();
   }

               /* Forma Gr�fica de la cua de BLK KEYB */
   if (!Buida(pcuaBLK_KEYB)) { BorrarGraphicBLK_KEYB(); PaintCuaBLK_KEYB(); }
   else { 
      EscriureCar(17, 64, contBLK_KEYB + 48, CARN);
      BorrarGraphicBLK_KEYB();
   }
       
           /* Actualitzar Procesos Bloquejats per Sleep */             
   if (!Buida(pcuaBLK_SLEEP)) DespertarProcesos();                              
   EOI();
   restaurar_Context(procesRUN->SS_SP);
}

void PaintCuaReady(void) {
   static int i, j;
   
   PCBaux = (struct tPCB *) pcuaRDY->primer;
   EscriureCar(17, 23, contREADY + 48, CARN);
   
   for(i = 0, j = 0;((struct item *) PCBaux) != pcuaRDY->ultim; i++) {
      if ((35 + 3 * i) % 38 == 0) { i = -4; j++; }
      if (PCBaux->ID < 10) EscriureCar(17 + j, (35 + 3 * i), PCBaux->ID + 48, NORMAL);
      else { 
         EscriureCar(17 + j, (35 + 3 * i), (PCBaux->ID / 10) + 48, NORMAL);
         EscriureCar(17 + j, (36 + 3 * i), (PCBaux->ID % 10) + 48, NORMAL);
      }       
      PCBaux = (struct tPCB *) ((struct item *) PCBaux)->seguent;
   }
   if ((35 + 3 * i) % 38 == 0) { i = -4; j++; }
   if (PCBaux->ID < 10) EscriureCar(17 + j, (35 + 3 * i), PCBaux->ID + 48, NORMAL);
   else { 
      EscriureCar(17 + j, (35 + 3 * i), (PCBaux->ID / 10) + 48, NORMAL);
      EscriureCar(17 + j, (36 + 3 * i), (PCBaux->ID % 10) + 48, NORMAL);
   }    
}

void PaintCuaBLK_SLEEP(void) {
   static int i, j;
   
   PCBaux = (struct tPCB *) pcuaBLK_SLEEP->primer;
   EscriureCar(17, 44, contBLK_SLEEP + 48, CARN);
   
   for(i = 0, j = 0;((struct item *) PCBaux) != pcuaBLK_SLEEP->ultim; i++) {
      if ((56 + 3 * i) % 59 == 0) { i = -4; j++; }
      if (PCBaux->ID < 10) EscriureCar(17 + j, (56 + 3 * i), PCBaux->ID + 48, NORMAL);
      else { 
         EscriureCar(17 + j, (56 + 3 * i), (PCBaux->ID / 10) + 48, NORMAL);
         EscriureCar(17 + j, (57 + 3 * i), (PCBaux->ID % 10) + 48, NORMAL);
      }       
      PCBaux = (struct tPCB *) ((struct item *) PCBaux)->seguent;
   }
   if ((56 + 3 * i) % 59 == 0) { i = -4; j++; }
   if (PCBaux->ID < 10) EscriureCar(17 + j, (56 + 3 * i), PCBaux->ID + 48, NORMAL);
   else { 
      EscriureCar(17 + j, (56 + 3 * i), (PCBaux->ID / 10) + 48, NORMAL);
      EscriureCar(17 + j, (57 + 3 * i), (PCBaux->ID % 10) + 48, NORMAL);
   }     
}

void PaintCuaBLK_KEYB(void) {
   static int i, j;
   
   PCBaux = (struct tPCB *) pcuaBLK_KEYB->primer;
   EscriureCar(17, 64, contBLK_KEYB + 48, CARN);
   
   for(i = 0, j = 0;((struct item *) PCBaux) != pcuaBLK_KEYB->ultim; i++) {
      if ((76 + 3 * i) % 79 == 0) { i = -4; j++; }
      if (PCBaux->ID < 10) EscriureCar(17 + j, (76 + 3 * i), PCBaux->ID + 48, NORMAL);
      else { 
         EscriureCar(17 + j, (76 + 3 * i), (PCBaux->ID / 10) + 48, NORMAL);
         EscriureCar(17 + j, (77 + 3 * i), (PCBaux->ID % 10) + 48, NORMAL);
      }       
      PCBaux = (struct tPCB *) ((struct item *) PCBaux)->seguent;
   }
   if ((76 + 3 * i) % 79 == 0) { i = -4; j++; }
   if (PCBaux->ID < 10) EscriureCar(17 + j, (76 + 3 * i), PCBaux->ID + 48, NORMAL);
   else { 
      EscriureCar(17 + j, (76 + 3 * i), (PCBaux->ID / 10) + 48, NORMAL);
      EscriureCar(17 + j, (77 + 3 * i), (PCBaux->ID % 10) + 48, NORMAL);
   }   
}

void BorrarGraphicRDY(void) { 
   EscriureString(17, 35, "   ", NORMAL);
   EscriureString(18, 23, "                  ", NORMAL);
   EscriureString(19, 23, "                  ", NORMAL);
}

void BorrarGraphicBLK_SLEEP(void) {
   EscriureString(17, 56, "   ", NORMAL);
   EscriureString(18, 44, "                  ", NORMAL);
   EscriureString(19, 44, "                  ", NORMAL);
}

void BorrarGraphicBLK_KEYB(void) {
   EscriureString(17, 76, "   ", NORMAL);
   EscriureString(18, 64, "                ", NORMAL);
   EscriureString(19, 64, "                ", NORMAL);
}

void DespertarProcesos(void) {
   PCBaux = (struct tPCB *) pcuaBLK_SLEEP->primer;

   for(auxcont = contBLK_SLEEP; auxcont > 0; auxcont--) { 
      if (PCBaux->Quantum > 0) PCBaux->Quantum--;
      else {              
      /* Comparem les prioritats per veure si el proc�s s'apropia de la CPU
         - Pasem de BLK -> RUN sense pasar per RDY                                 */
         if (procesRUN->Prioritat < PCBaux->Prioritat) {
            procesRUN->Estat = RDY;
            Inserir(pcuaRDY, Primer(pcuaRUN), procesRUN->Prioritat);
            contBLK_SLEEP--; contREADY++;
            dispatcher((struct tPCB *) Extirpar(pcuaBLK_SLEEP, (struct item *) PCBaux));
         }
         else {
            PCBaux->Estat = RDY;
            contBLK_SLEEP--; contREADY++;
            Inserir(pcuaRDY, Extirpar(pcuaBLK_SLEEP, (struct item *) PCBaux), PCBaux->Prioritat);
         }         
      }
      PCBaux = (struct tPCB *) PCBaux->Elem.seguent;         
   }   
}

/* ############################################### [ TECLAT ] ################################################# 
   %%% Interrupci� 0x9 (Interrupci� del teclat) %%%                                                              
     - Salvem el context del proces que esta executant-se
     - Capturem la tecla (MAKE) i fem casting a ASCII
     - La insertem al buffer del teclat
     - Desbloquejem els procesos
     - Restaurem el context del proc�s                                                                          */
     
void teclat(void) {
   static char tecla;
   
   procesRUN->SS_SP = salvar_Context(); 
      
   if ((tecla = llegirTecla()) != 1) {
      tecla = ASCII(tecla);     
      bufferTeclat.buffer[bufferTeclat.fi % (TAM_BUFFER)] = tecla;
      bufferTeclat.fi++;

      if (((struct tPCB *) pcuaBLK_KEYB->primer)->ID == 1) {   
         if ((tecla != RETURN) && (lencmd < COLS - 23)) { 
            EscriureCar(23, 22 + lencmd, tecla, 7); 
            lencmd++;
         }
         else { EscriureString(23, 22, clean, 7); lencmd = 0; } 
      }
      
      if (!Buida(pcuaBLK_KEYB)) DesbloquejarProcesos();
   }
   
   EOI();
   restaurar_Context(procesRUN->SS_SP);
}   
   
void DesbloquejarProcesos(void) {
   static int i;
   static char AUXtecla;
   static int Teclaret;
   
   Teclaret = 0;
     
   PCBaux = (struct tPCB *) pcuaBLK_KEYB->primer;
   
   if ((PCBaux->Ftec > TAM_BUFFER) && (PCBaux->ID != 1)) PCBaux->Ftec = TAM_BUFFER;
   
   num_elems = bufferTeclat.fi - bufferTeclat.ini;
   if (num_elems > TAM_BUFFER) num_elems = TAM_BUFFER;     

   if ((num_elems == PCBaux->Ftec) || (bufferTeclat.buffer[(bufferTeclat.fi - 1) % (TAM_BUFFER)] == RETURN)) {
      for(i = 0;((PCBaux->Ftec > 0) && (Teclaret == 0)); PCBaux->Ftec--, i++) {
         AUXtecla = bufferTeclat.buffer[bufferTeclat.ini % TAM_BUFFER];
         PCBaux->BufferDesti[i] = AUXtecla; 
         if (AUXtecla == RETURN) Teclaret = 1;      
         bufferTeclat.ini++;
      }                                                             
      if (procesRUN->Prioritat < PCBaux->Prioritat) {
         procesRUN->Estat = RDY;
         Inserir(pcuaRDY, Primer(pcuaRUN), procesRUN->Prioritat);
         contBLK_KEYB--; contREADY++;
         dispatcher((struct tPCB *) Extirpar(pcuaBLK_KEYB, (struct item *) PCBaux));
      }
      else {
         PCBaux->Estat = RDY;
         contBLK_KEYB--; contREADY++;
         Inserir(pcuaRDY, Extirpar(pcuaBLK_KEYB, (struct item *) PCBaux), PCBaux->Prioritat);
      }
   }
}   
     
/* ############################################# [ SCHEDULER ] ################################################
   - Escolleix quin proces pasa a RUN                                                                           */
   
struct tPCB * scheduler(void) { contREADY--; return((struct tPCB *) Primer(pcuaRDY)); }

/* ############################################# [ DISPATCHER ] ###############################################
   - Posa el proces escollit per el scheduler a RUN                                                             */

void dispatcher(struct tPCB * pcbRUN) {
   Encuar(pcuaRUN, (struct item *) pcbRUN);
   pcbRUN->Estat = RUN;
   procesRUN = pcbRUN;
   
                 /* Forma Gr�fica de la cua de RUN */
   EscriureString(17, 13, "      ", NORMAL);
   EscriureString(17, 13, procesRUN->Proces, NORMAL);
   EscriureString(18, 11, "      ", NORMAL);
   if (procesRUN->ID < 10) EscriureCar(18, 11, procesRUN->ID + 48, NORMAL);
   else { 
      EscriureCar(18, 11, (procesRUN->ID / 10) + 48, NORMAL);
      EscriureCar(18, 12, (procesRUN->ID % 10) + 48, NORMAL);
   } 
   EscriureString(19, 14, "      ", NORMAL);
   EscriureCar(19, 14, procesRUN->Prioritat + 48, NORMAL);   
   EOI();
   restaurar_Context(pcbRUN->SS_SP);
}