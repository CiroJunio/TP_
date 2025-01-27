#ifndef ARVORE_B_STAR_H
#define ARVORE_B_STAR_H

#include "registro.h"
#include <stdio.h>
#include <stdlib.h>

typedef enum
{
    Interna,
    Externa
} TipoIntExt;

typedef struct TipoPagina2 *TipoApontador2;

typedef struct TipoPagina2
{
    TipoIntExt Pt;
    union
    {
        struct
        {
            int ni;
            int ri[MM];
            TipoApontador2 pi[MM + 1];
        } U0;
        struct
        {
            int ne;
            tipoitem re[MM2];
        } U1;
    } UU;
} TipoPagina2;

void InsereEstrela(tipoitem Reg, TipoApontador2 *Ap, int *comparacoes);
void InsEstrela(tipoitem Reg, TipoApontador2 Ap, short *Cresceu, 
                tipoitem *RegRetorno, TipoApontador2 *ApRetorno,
                int *comparacoes);

void InsereNaPaginaEstrela(TipoApontador2 Ap, int Reg, TipoApontador2 Apdir,
                           int *comparacoes);

void InsereNaPaginaFolhaEstrela(TipoApontador2 Ap, tipoitem Reg,
                                TipoApontador2 Apdir, int *comparacoes);

int PesquisaEstrela(tipoitem *x, TipoApontador2 *Ap, int *comparacoes);

#endif // ARVORE_B_STAR_H
