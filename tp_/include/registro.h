#ifndef REGISTRO_H
#define REGISTRO_H

#include <stdio.h>
#include <stdlib.h>

#define ITENSPAGINA 1000
#define MAXTABELA 20000
#define M 4
#define MM (M * 2)
#define MM2 (M * 2)
#define NUM_CHAVES 20
#define NUM_METODOS 4

// Estrutura para representar um registro
typedef struct {
  int chave;
  long int dado1;
  char dado2[5000];
} tipoitem;

typedef struct {
  int acessos, comparacoes;
} dados;

#endif // REGISTRO