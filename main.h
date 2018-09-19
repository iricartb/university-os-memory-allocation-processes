/* Fitxer: main.h
   Descripció: Agafem estructures, variables i funcions del fitxer main.c                    */
      
extern struct cua *pcuaRUN, *pcuaRDY, *pcuaBLK_SLEEP, *pcuaBLK_KEYB, *pcuaPCB;
extern struct tPCB *procesRUN;
extern struct tCIRCULAR bufferTeclat;
extern char clean[59];
extern void ClearScreen(void);