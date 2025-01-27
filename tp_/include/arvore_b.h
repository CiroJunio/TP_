#ifndef ARVORE_B_H
#define ARVORE_B_H

#include "registro.h"
#include <stdio.h>
#include <stdlib.h>

// Definição de um ponteiro para a estrutura da página da árvore B
typedef struct TipoPagina *TipoApontador;

// Estrutura da página da árvore B
typedef struct TipoPagina {
    short n;                 // Número de registros na página
    tipoitem r[MM];          // Vetor de registros
    TipoApontador p[MM + 1]; // Vetor de ponteiros para as páginas filhas
} TipoPagina;

// Funções principais da árvore B
void Inicializa(TipoApontador *Arvore);                              // Inicializa a árvore B
void InsereAB(tipoitem Reg, TipoApontador *Ap, int *comparacoes, int crescente);    // Insere um registro na árvore B (agora com parâmetro 'crescente')
int PesquisaAB(int chave, TipoApontador Ap, int *comparacoes, int crescente);       // Pesquisa um registro na árvore B (agora com parâmetro 'crescente')
void Ins(tipoitem Reg, TipoApontador Ap, short *Cresceu, tipoitem *RegRetorno,
         TipoApontador *ApRetorno, int *comparacoes, int crescente);  // Função recursiva de inserção (agora com parâmetro 'crescente')
void InsereNaPagina(TipoApontador Ap, tipoitem Reg, TipoApontador Apdir,
                    int *comparacoes, int crescente);                               // Insere um registro em uma página (agora com parâmetro 'crescente')
void limpar(TipoApontador arvore);                                  // Libera a memória da árvore

void processaArvoreB(const char *nomeArquivo, int qtd, int chave, dados *cont, int crescente);

#endif // ARVORE_B_H
