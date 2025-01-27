#include "../include/registro.h"
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/acesso_sequencial.h"
#include "../include/arvore_binaria.h"
#include "../include/arvore_b.h"
#include "../include/arvore_b_star.h"
#include "../include/registro.h"
#include "../include/utils.h"

// Função para gerar um arquivo binário com registros ordenados crescentemente
void gerarCrescente(int quantidade) {
    const char *nomeArquivo = "./data/crescente.bin";

    // Cria os registros na memória
    tipoitem *registros = malloc(quantidade * sizeof(tipoitem));
    if (!registros) {
        perror("Erro ao alocar memória para registros");
        exit(1);
    }

    for (int i = 0; i < quantidade; i++) {
        registros[i].chave = i + 1;
        registros[i].dado1 = (long int)(rand() % 10000);
        snprintf(registros[i].dado2, sizeof(registros[i].dado2), "Registro %d", registros[i].chave);
    }

    // Grava os registros no arquivo
    FILE *arquivo = fopen(nomeArquivo, "wb");
    if (!arquivo) {
        perror("Erro ao criar arquivo");
        free(registros);
        exit(1);
    }

    fwrite(registros, sizeof(tipoitem), quantidade, arquivo);

    free(registros);
    fclose(arquivo);
    printf("Arquivo gerado em ordem crescente: %s\n", nomeArquivo);
}

// Função para gerar um arquivo binário com registros ordenados decrescentemente
void gerarDecrescente(int quantidade) {
    const char *nomeArquivo = "./data/decrescente.bin";

    // Cria os registros na memória
    tipoitem *registros = malloc(quantidade * sizeof(tipoitem));
    if (!registros) {
        perror("Erro ao alocar memória para registros");
        exit(1);
    }

    for (int i = 0; i < quantidade; i++) {
        registros[i].chave = quantidade - i;
        registros[i].dado1 = (long int)(rand() % 10000);
        snprintf(registros[i].dado2, sizeof(registros[i].dado2), "Registro %d", registros[i].chave);
    }

    // Grava os registros no arquivo
    FILE *arquivo = fopen(nomeArquivo, "wb");
    if (!arquivo) {
        perror("Erro ao criar arquivo");
        free(registros);
        exit(1);
    }

    fwrite(registros, sizeof(tipoitem), quantidade, arquivo);

    free(registros);
    fclose(arquivo);
    printf("Arquivo gerado em ordem decrescente: %s\n", nomeArquivo);
}

// Função para gerar um arquivo binário com registros em ordem aleatória
void gerarAleatorio(int quantidade) {
    const char *nomeArquivo = "./data/aleatorio.bin";

    // Cria um array para as chaves (usado para embaralhamento)
    int *chaves = malloc(quantidade * sizeof(int));
    if (!chaves) {
        perror("Erro ao alocar memória para chaves");
        exit(1);
    }

    // Preenche as chaves com valores de 1 até `quantidade`
    for (int i = 0; i < quantidade; i++) {
        chaves[i] = i + 1;
    }

    // Embaralha as chaves
    for (int i = 0; i < quantidade; i++) {
        int j = rand() % quantidade;
        int temp = chaves[i];
        chaves[i] = chaves[j];
        chaves[j] = temp;
    }

    // Cria os registros na memória
    tipoitem *registros = malloc(quantidade * sizeof(tipoitem));
    if (!registros) {
        perror("Erro ao alocar memória para registros");
        free(chaves);
        exit(1);
    }

    for (int i = 0; i < quantidade; i++) {
        registros[i].chave = chaves[i];
        registros[i].dado1 = (long int)(rand() % 10000);
        snprintf(registros[i].dado2, sizeof(registros[i].dado2), "Registro %d", registros[i].chave);
    }

    // Grava os registros no arquivo
    FILE *arquivo = fopen(nomeArquivo, "wb");
    if (!arquivo) {
        perror("Erro ao criar arquivo");
        free(chaves);
        free(registros);
        exit(1);
    }

    fwrite(registros, sizeof(tipoitem), quantidade, arquivo);

    free(chaves);
    free(registros);
    fclose(arquivo);
    printf("Arquivo gerado em ordem aleatória: %s\n", nomeArquivo);
}


void criaDiretorio(const char *diretorio) {
    struct stat st = {0};
    if (stat(diretorio, &st) == -1) {
        if (mkdir(diretorio, 0700) != 0) {
            perror("Erro ao criar diretório");
            exit(EXIT_FAILURE);
        }
    }
}

void salvaResultados(const char *nomeArquivo, int quantidade, int situacao, double mediaAcessos, double mediaComparacoes, double tempo) {
     criaDiretorio("./resultados");
    FILE *arquivo = fopen(nomeArquivo, "a");
    if (arquivo == NULL) {
        perror("Erro ao criar arquivo de resultados");
        exit(EXIT_FAILURE);
    }

    fprintf(arquivo, "Quantidade: %d\n", quantidade);
    fprintf(arquivo, "Situação: %d\n", situacao);
    fprintf(arquivo, "Média de Acessos: %.2f\n", mediaAcessos);
    fprintf(arquivo, "Média de Comparações: %.2f\n", mediaComparacoes);
    fprintf(arquivo, "Tempo Médio: %.6f segundos\n", tempo);
    fprintf(arquivo, "---------------------------\n");

    fclose(arquivo);
}

void realizaExperimentos() {
    // Configurações dos experimentos
    int quantidades[] = {100, 200, 2000, 20000, 200000, 2000000};
    int numQuantidades = sizeof(quantidades) / sizeof(quantidades[0]);
    int situacoes[] = {1, 2, 3}; // 1: crescente, 2: decrescente, 3: aleatório
    int numSituacoes = sizeof(situacoes) / sizeof(situacoes[0]);
    const char *metodos[] = {"sequencial", "arvore_binaria", "arvore_b", "arvore_b_star"};

    // Para armazenar estatísticas médias
    double mediaAcessos[NUM_METODOS][numQuantidades][numSituacoes];
    double mediaComparacoes[NUM_METODOS][numQuantidades][numSituacoes];
    double mediaTempo[NUM_METODOS][numQuantidades][numSituacoes];

    // Para cada quantidade de registros
    for (int q = 0; q < numQuantidades; q++) {
        int qtd = quantidades[q];

        // Para cada situação (ordem do arquivo)
        for (int s = 0; s < numSituacoes; s++) {
            int situacao = situacoes[s];

            // Gerar arquivo correspondente
            const char *nomeArquivo;
            if (situacao == 1) {
                nomeArquivo = "./data/crescente.bin";
                gerarCrescente(qtd);
            } else if (situacao == 2) {
                nomeArquivo = "./data/decrescente.bin";
                gerarDecrescente(qtd);
            } else {
                nomeArquivo = "./data/aleatorio.bin";
                gerarAleatorio(qtd);
            }

            // Abrir o arquivo gerado
            FILE *arq = fopen(nomeArquivo, "rb");
            if (arq == NULL) {
                perror("Erro ao abrir o arquivo gerado");
                continue;
            }

            // Gerar 20 chaves de pesquisa distintas
            int chaves[NUM_CHAVES];
            for (int i = 0; i < NUM_CHAVES; i++) {
                chaves[i] = i * (qtd / NUM_CHAVES);
            }

            // Para cada método de pesquisa
            for (int m = 0; m < NUM_METODOS; m++) {
                dados cont; // Contador para estatísticas
                int totalAcessos = 0;
                int totalComparacoes = 0;
                double totalTempo = 0.0;

                // Nome do arquivo de saída para este método e quantidade
                char nomeSaida[100];
                snprintf(nomeSaida, sizeof(nomeSaida), "./resultados/%s_%d.txt", metodos[m], qtd);

                // Executar para cada chave
                for (int c = 0; c < NUM_CHAVES; c++) {
                    int chave = chaves[c];
                    cont.acessos = 0;
                    cont.comparacoes = 0;
                    int chaveEncontrada = 0; // Variável para indicar se a chave foi encontrada

                    clock_t inicio = clock();

                    switch (m + 1) {
                        case 1: // Pesquisa Sequencialmente Indexada
                            acesso_sequencial_indexado(qtd, situacao, chave, &cont, nomeArquivo);
                            // Verificar se a chave foi encontrada
                            chaveEncontrada = (cont.acessos > 0 && cont.comparacoes > 0) ? 1 : 0;
                            break;

                        case 2: // Árvore Binária
                            manipulaArvoreBinaria(arq, qtd, chave, &cont);
                            chaveEncontrada = (cont.acessos > 0 && cont.comparacoes > 0) ? 1 : 0;
                            break;

                        case 3: // Árvore B
                            processaArvoreB(nomeArquivo, qtd, chave, &cont);
                            chaveEncontrada = (cont.acessos > 0 && cont.comparacoes > 0) ? 1 : 0;
                            break;

                        case 4: // Árvore B*
                            processaArvoreEstrela(nomeArquivo, qtd, chave, &cont);
                            chaveEncontrada = (cont.acessos > 0 && cont.comparacoes > 0) ? 1 : 0;
                            break;

                        default:
                            printf("Método inválido\n");
                            continue;
                    }

                    clock_t fim = clock();
                    double tempoBusca = ((double)(fim - inicio)) / CLOCKS_PER_SEC;

                    totalAcessos += cont.acessos;
                    totalComparacoes += cont.comparacoes;
                    totalTempo += tempoBusca;

                    // Salvar a saída detalhada no arquivo de resultados
                    FILE *arquivo = fopen(nomeSaida, "a");
                    if (arquivo == NULL) {
                        perror("Erro ao abrir arquivo para salvar resultados");
                        continue;
                    }
                    fprintf(arquivo, "Chave: %d | Encontrada: %s | Acessos: %d | Comparações: %d | Tempo: %.6f segundos\n",
                            chave, chaveEncontrada ? "Sim" : "Não", cont.acessos, cont.comparacoes, tempoBusca);
                    fclose(arquivo);
                }

                // Calcular médias
                double mediaAcessosMetodo = totalAcessos / (double)NUM_CHAVES;
                double mediaComparacoesMetodo = totalComparacoes / (double)NUM_CHAVES;
                double mediaTempoMetodo = totalTempo / NUM_CHAVES;

                mediaAcessos[m][q][s] = mediaAcessosMetodo;
                mediaComparacoes[m][q][s] = mediaComparacoesMetodo;
                mediaTempo[m][q][s] = mediaTempoMetodo;

                // Salvar resultados no arquivo de resumo
                salvaResultados(nomeSaida, qtd, situacao, mediaAcessosMetodo, mediaComparacoesMetodo, mediaTempoMetodo);
            }

            fclose(arq);
        }
    }

    // Imprimir resultados finais no console
    for (int m = 0; m < NUM_METODOS; m++) {
        printf("\nResultados para o método: %s\n", metodos[m]);
        printf("Quantidade\tSituação\tMédia Acessos\tMédia Comparações\tMédia Tempo\n");

        for (int q = 0; q < numQuantidades; q++) {
            for (int s = 0; s < numSituacoes; s++) {
                printf("%d\t\t%d\t\t%.2f\t\t%.2f\t\t%.6f\n",
                       quantidades[q], situacoes[s],
                       mediaAcessos[m][q][s], mediaComparacoes[m][q][s], mediaTempo[m][q][s]);
            }
        }
    }
}