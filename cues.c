#define MODUL_CUES 
#include "cues.h"

/* Funcions per a inicialitzar i mantenir cues doblement encadenades */ 
/* Els elements han de ser estructures de manera que els primers camps siguin: */ 
/* struct item *seguent, *anterior; */ 
/* int ordre; criteri d'ordenacio (numero d'ordre) */ 
/* NOTA: NO es creen les instanciacions a memoria de les estructures */ 

/* Inicialitza una estructura cua, fa que el punter al primer i Ultim */ 
/* Item de la cua apuntin a l'estructura cua, o sigui, a cap element. */
struct cua *(Inicialitzar_cua (struct cua * c)) 
{   if (c!=NIL) { c->ultim = c->primer = (struct item *) c; } 
    return(c); 
}

/* Funcio que pregunta si la cua està buida, i si ho està retorna cert. */
int Buida(struct cua * c) 
{   return((c->primer == NIL) || (c->primer == (struct item *) c)); } 

/* Procediment que posa al final de la cua l'item especificat. */
struct item * (Encuar (struct cua * c, struct item * elem)) 
{   struct item * aux; 
    if (c==NIL) return (NIL);                   /* cua no creada */ 
    aux= c->ultim; 
    if (aux==NIL || elem==NIL) return(NIL);     /* cua no inic. o no elem */ 
    elem->seguent= aux->seguent;                /* al final de la cua */ 
    elem->anterior= aux;                        /* recorda darrera qui vas */ 
    (aux->seguent)->anterior= elem;             /* == c->ultim= elem; */ 
    aux->seguent= elem;                         /* el de davant el veu */ 
    return(elem); 
}

/* Procediment que insereix en una cua ordenada un item en un ordre donat */
struct item * (Inserir (struct cua * c, struct item * elem, int ordre)) 
{  struct item * ant, * post;
    if (c==NIL) return (NIL);                   /* cua no creada */ 
    ant= c->ultim; 
    if (ant==NIL || elem==NIL) return(NIL);     /* cua no inic. o no elem */ 
    elem->ordre= ordre; 
    if (ant == (struct item *) c)               /* cua buida : sera primer i darrer */ 
    {  elem->seguent= elem->anterior= (struct item *) c;
        c->primer= c->ultim= elem; 
        return(elem); 
    }

/* recorrem la cua des de darrera, doncs encuem ! */
    post= (struct item *) c; 
    while ( (ant != (struct item *) c)              /* !principi de cua i prioritat menor */ 
                    && (ant->ordre) < ordre)        /* ordre(0) < ordre(n) */ 
    {  post= ant;                                  /* ordre = ? => a la cua: RoundRobin */
        ant= ant->anterior; 
    } 

/* ja sabem on li toca: fer-ho efectiu */
    ant->seguent= elem;                             /* pel davant */ 
    elem->anterior= ant; 
    elem->seguent= post;                            /* i pel darrera */ 
    post->anterior= elem; 
    return(elem); 
} 

/* Funcio que extreu de la cua l'item desitjat. Si no el troba o la cua */ 
/* no esta inicialitzada retorna NIL. */
struct item *(Extirpar (struct cua *c, struct item *elem)) 
{ struct item * post, * ant; 
    if (c==NIL) return (NIL);                       /* cua no creada */ 
    post= c->primer; 
    if (post==NIL) return (NIL);                    /* cua no inicialitzada */ 
    ant= (struct item *) c;                         /* localitzem la seva posicio dins la cua */ 
    while ((post!=(struct item *) c) && (post!=elem)) /* !fi de cua i !trobat*/ 
    {  ant= post;
        post= post->seguent; 
    } 
    if (post == (struct item *) c) return(NIL);     /* no trobat */ 
    ant->seguent= post->seguent;                    /* treure'l */ 
    (post->seguent)->anterior= ant; 
    post->seguent= NIL;                             /* ara ja no es a la cua */ 
    post->anterior= NIL; 
    return (post); 
} 

/* Funcio que extreu de la cua ordenada un item amb el numero d'ordre indicat */ 
/* Si no el troba o la cua no està inicialitzada retorna NIL. */
struct item *(Extreure (struct cua *c, int ordre)) 
{  struct item * post, * ant;
    if (c==NIL) return (NIL);                       /* cua no creada */ 
    post= c->primer; 
    if (post==NIL) return (NIL);                    /* cua no inicialitzada */ 
    ant= (struct item *) c;                         /* localitzem la seva posicio dins la cua */ 
    while ((post!=(struct item *) c) 
                && (post->ordre > ordre))           /* !fi de cua i !trobat */ 
    {  ant= post;
        post= post->seguent; 
    } 
    if ((post == (struct item *) c) || (post->ordre != ordre)) 
    return(NIL);                                /* no trobat ningú amb aquest número d'ordre */ 
    ant->seguent= post->seguent; /* treure'l */ 
    (post->seguent)->anterior= ant; 
    post->seguent= NIL; /* ara ja no es a la cua */ 
    post->anterior= NIL; 
    return (post); 
} 

/* Funcio que retorna un punter al primer item de la cua i el desencua. */ 
/* Si la cua no esta inicialitzada o esta buida retorna NIL. */
struct item *(Primer (struct cua * c)) 
{  struct item * aux;
    if (c==NIL) return (NIL); /* cua no creada */ 
    aux= c->primer; 
    if (aux==NIL || aux==(struct item*)c) return (NIL); /* cua no inic. o no elem */ 
    c->primer= aux->seguent; 
    (aux->seguent)->anterior= (struct item *) c; 
    aux->seguent= NIL; /* ara ja no es a la cua */ 
    aux->anterior= NIL; 
    return(aux); 
}