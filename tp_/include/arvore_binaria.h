#ifndef ARVORE_BINARIA_H
#define ARVORE_BINARIA_H

#include "registro.h"
#include <stdio.h>
#include <stdlib.h>

typedef struct {
  int chave;
  long int dado1;
  char dado2[5000];
  int pDir; // Filho da direita
  int pEsq; // Filho da esquerda
  int posOriginal; // Posicao original
} tipoitemarvore;

void atualizaPonteiroAB(int posicaoFilho, FILE *arquivo, int *acessos, int *comparacoes);
int ProcuraAB(int chave, FILE *arquivo, int *acessos, int *comparacoes);
void manipulaArvoreBinaria(FILE *arq, int qtd, int chave, dados *cont);


#endif // ARVORE_BINARIA_H