#ifndef UTILS_H
#define UTILS_H

#include "registro.h"
#include <time.h>
void gerarCrescente(int quantidade);
void gerarAleatorio(int quantidade);
void gerarDecrescente(int quantidade);
void realizaExperimentos();
void criaDiretorio(const char *diretorio);
void salvaResultados(const char *nomeArquivo, int quantidade, int situacao, double mediaAcessos, double mediaComparacoes, double tempo);
#endif // UTILS_H