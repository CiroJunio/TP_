#include <stdio.h>
#include <stdlib.h>
#include "../include/acesso_sequencial.h"
#include <stdio.h>
#include <stdlib.h>

void exibir_registros(const char *nome_arquivo, int qtd) {
    FILE *arq = fopen(nome_arquivo, "rb");
    if (arq == NULL) {
        printf("Erro ao abrir o arquivo %s\n", nome_arquivo);
        return;
    }

    tipoitem item;
    printf("Registros no arquivo:\n");
    for (int i = 0; i < qtd; i++) {
        fread(&item, sizeof(tipoitem), 1, arq);
        printf("Chave: %d | Dado1: %ld | Dado2: %s\n", item.chave, item.dado1, item.dado2);
    }
    fclose(arq);
}

// Função de ordenação usando Shell Sort
void ordenacao(const char *nome_arquivo, int n) {
    FILE *arq = fopen(nome_arquivo, "r+b"); // Abre o arquivo no modo leitura/escrita binário
    if (arq == NULL) {
        printf("Erro: Não foi possível abrir o arquivo %s\n", nome_arquivo);
        exit(1);
    }

    tipoitem ax, auxiliar;
    int desloc, h = 1, j;

    // Calcula o valor inicial de h para o algoritmo de ordenação Shell Sort
    while (h < n) {
        h = 3 * h + 1;
    }

    do {
        h = (h - 1) / 3; // Reduz o intervalo h
        for (int i = h; i < n; i++) {
            desloc = i * sizeof(tipoitem);
            fseek(arq, desloc, SEEK_SET); // Move para a posição do item no arquivo
            fread(&ax, sizeof(tipoitem), 1, arq); // Lê o item atual

            j = i;
            while (j >= h) {
                desloc = (j - h) * sizeof(tipoitem);
                fseek(arq, desloc, SEEK_SET); // Move para o item anterior com intervalo h
                fread(&auxiliar, sizeof(tipoitem), 1, arq); // Lê o item anterior

                if (auxiliar.chave <= ax.chave) {
                    break; // Sai do loop se o item estiver na ordem correta
                }

                // Move o item anterior para a posição atual
                desloc = j * sizeof(tipoitem);
                fseek(arq, desloc, SEEK_SET);
                fwrite(&auxiliar, sizeof(tipoitem), 1, arq);

                j -= h;
            }

            // Insere o item armazenado em ax na posição correta
            desloc = j * sizeof(tipoitem);
            fseek(arq, desloc, SEEK_SET);
            fwrite(&ax, sizeof(tipoitem), 1, arq);
        }
    } while (h > 1);

    fclose(arq); // Fecha o arquivo após a operação
}

// Gera a tabela de índices
int gerar_tabela_indices(FILE *arq, int *tab, int qtd, dados *cont) {
    tipoitem item;
    for (int i = 0; i < qtd; i++) {
        if (fread(&item, sizeof(tipoitem), 1, arq) == 1) {
            cont->acessos++;
            tab[i] = item.chave; // Armazena a chave na tabela de índices
        } else {
            return 0; // Falha ao ler o registro
        }
    }
    return 1; // Sucesso
}

// Busca sequencial indexada com ordenação e busca binária
int acesso_sequencial_indexado(int qtd, int situacao, int chave, dados *cont, const char *nome_arquivo) {
    if (situacao == 2 || situacao == 3) {
        ordenacao(nome_arquivo, qtd); // Ordena o arquivo se necessário
        printf("Arquivo ordenado.\n");
       // exibir_registros(nome_arquivo, qtd); // Exibe registros ordenados (para depuração)
    }

    tipoitem item;
    int *tab = malloc(qtd * sizeof(int)); // Alocação dinâmica para a tabela de índices
    if (tab == NULL) {
        printf("Erro ao alocar memória para a tabela de índices.\n");
        return 0;
    }

    // Abre o arquivo binário para leitura
    FILE *arq = fopen(nome_arquivo, "rb");
    if (arq == NULL) {
        printf("Erro ao abrir o arquivo: %s\n", nome_arquivo);
        free(tab);
        return 0;
    }

    // Gera a tabela sequencial de índices
    if (!gerar_tabela_indices(arq, tab, qtd, cont)) {
        printf("Erro ao gerar a tabela de índices a partir do arquivo.\n");
        free(tab);
        fclose(arq);
        return 0;
    }

    // Realiza a busca binária na tabela de índices
    int esquerda = 0, direita = qtd - 1;
    while (esquerda <= direita) {
        int meio = esquerda + (direita - esquerda) / 2; // Evita overflow ao calcular o meio
        cont->comparacoes++;

        if (tab[meio] == chave) {
            // Chave encontrada no índice, busca o registro correspondente no arquivo
            fseek(arq, meio * sizeof(tipoitem), SEEK_SET);
            fread(&item, sizeof(tipoitem), 1, arq);
            cont->acessos++;
            free(tab);
            fclose(arq);
            return 1;
        }

        if (tab[meio] < chave) {
            esquerda = meio + 1;
        } else {
            direita = meio - 1;
        }
    }

    // Caso a chave não seja encontrada
    printf("Chave não encontrada!\n");
    free(tab);
    fclose(arq);
    return 0;
}