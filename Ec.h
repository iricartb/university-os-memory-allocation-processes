/* Fitxer: Ec.h
   Implementació: RUTS_SO.lib
   Descripció: Capçalera de funcions relacionades amb la pràctica en general                 */ 
                  
#define INT_CK      0x8
#define INT_TR      0x40
#define INT_KB      0x9
#define TAM_BUFFER  100
#define RETURN      13
#define ESPACIO     32
#define BACKSPACE   8

/* Estructura del buffer circular del teclat */

typedef struct tCIRCULAR {
   char buffer[TAM_BUFFER];
   unsigned int ini;
   unsigned int fi;
};
   

typedef unsigned int WORD;
typedef unsigned char BYTE;
typedef void * PTR;

extern void exit(int exit);

extern BYTE Peek (PTR Adr);
extern void Poke (PTR Adr, BYTE valor);
extern BYTE InPort (WORD Port);
extern void OutPort (WORD Port, BYTE Valor);

extern PTR Ptr (WORD Segment, WORD Desplacament);
extern WORD Segment (PTR Adreca);
extern WORD Desplacament (PTR Adreca);
extern WORD Word (BYTE msb, BYTE lsb);
extern BYTE MSB (WORD Valor);
extern BYTE LSB (WORD Valor);

extern WORD Inhibir (void);
extern void Desinhibir (WORD Flags);
extern void EOI (void);
extern void *Salvar (void);
extern void Restaurar (void);
extern void (*CanviarVectInt (BYTE NumInt, void (*NovaRSI)() ));

extern PTR BufferPantalla (void);
extern void EscriureCar (BYTE fila, BYTE columna, BYTE caracter, BYTE atribut);
extern void EscriureString (BYTE fila, BYTE columna, BYTE * string, BYTE atribut);
extern void EsborrarPantalla (BYTE atribut);
extern WORD QuinCarAtr (BYTE fila, BYTE columna);

extern void Strobe (WORD AdrPort, BYTE Bit0_7);
extern void IniTeclatINT (void);
extern void RSI_teclat (void);
extern BYTE LlegirTeclatINT (void);
extern void FiTeclatINT (void);

extern BYTE ASCII (BYTE ScanCode);

extern void So (WORD frequencia);