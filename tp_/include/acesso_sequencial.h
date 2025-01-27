#ifndef ACESSO_SEQUENCIAL_H
#define ACESSO_SEQUENCIAL_H

#include "registro.h"
#include <stdio.h>
#include <stdlib.h>
int acesso_sequencial_indexado(int qtd,int situacao, int chave, dados *cont, const char *nome_arquivo);
void ordenacao(const char *nome_arquivo, int n);
int gerar_tabela_indices(FILE *arq, int *tab, int qtd, dados *cont);
void exibir_registros(const char *nome_arquivo, int qtd);
#endif // ACESSO_SEQUENCIAL_H