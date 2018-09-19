/* Fitxer: semafors.h
   Descripció: Estructura de dades relacionades amb els semàfors                             */
                   
#define SEM_NIL         65535U
#define MAX_NUM_SEMS        25

typedef struct tSEMAFOR {    
   struct cua cuaBLK;
   INT ID;
   INT valor;
};