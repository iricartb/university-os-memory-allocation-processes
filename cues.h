#ifndef CUES-H 
#define CUES-H 
#define NIL 0

typedef struct item 
{  struct item *seguent;
    struct item *anterior; 
    int ordre; 
};

typedef struct cua 
{  struct item *primer;
    struct item *ultim; 
    int ordre;                          /* no usat, sols per coherencia */ 
}; 

#ifdef MODUL_CUES

#else

extern struct cua *(Inicialitzar_cua(struct cua *c)); 
extern int Buida(struct cua *c);
extern struct item *(Encuar (struct cua *c, struct item *i)); 
extern struct item *(Inserir (struct cua *c, struct item *i, int ordre)); 
extern struct item *(Primer (struct cua *c)); 
extern struct item *(Extirpar (struct cua *c, struct item *e)); 
extern struct item *(Extreure (struct cua *c, int ordre)); 
#endif 
#endif