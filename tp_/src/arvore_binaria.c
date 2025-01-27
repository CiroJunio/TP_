#include "../include/arvore_binaria.h"
#include <string.h>
#include <time.h>


void atualizaPonteiroAB(int posicaoFilho, FILE *arquivo, int *acessos, int *comparacoes) {
    tipoitemarvore filho, pai;
    int posicaoPai = 0;

    // Inicializa os campos de pai para evitar o uso não inicializado
    pai.pEsq = -1;
    pai.pDir = -1;
    pai.posOriginal = -1;
    pai.chave = -1;

    // Obtém o registro do filho
    fseek(arquivo, posicaoFilho * sizeof(tipoitemarvore), SEEK_SET);
    fread(&filho, sizeof(tipoitemarvore), 1, arquivo);
    (*acessos)++;

    // Obtém a raiz da árvore (raiz está na posição 0)
    fseek(arquivo, 0, SEEK_SET);
    fread(&pai, sizeof(tipoitemarvore), 1, arquivo);
    (*acessos)++;

    // Percorre a árvore para encontrar a posição correta do pai
    while (1) {
        (*comparacoes)++;
        if (filho.chave < pai.chave) {
            if (pai.pEsq == -1) break; // Achou a posição
            posicaoPai = pai.pEsq;
        } else {
            if (pai.pDir == -1) break; // Achou a posição
            posicaoPai = pai.pDir;
        }

        // Move para o próximo nó na árvore
        fseek(arquivo, posicaoPai * sizeof(tipoitemarvore), SEEK_SET);
        fread(&pai, sizeof(tipoitemarvore), 1, arquivo);
        (*acessos)++;
    }

    // Atualiza o ponteiro do pai para o filho
    if (filho.chave < pai.chave) {
        pai.pEsq = posicaoFilho;
    } else {
        pai.pDir = posicaoFilho;
    }

    // Grava a atualização no arquivo
    fseek(arquivo, posicaoPai * sizeof(tipoitemarvore), SEEK_SET);
    fwrite(&pai, sizeof(tipoitemarvore), 1, arquivo);
    (*acessos)++;
}

int ProcuraAB(int chave, FILE *arquivo, int *acessos, int *comparacoes) {
    tipoitemarvore noAtual;

    // Inicializa os campos de noAtual para evitar o uso não inicializado
    noAtual.pEsq = -1;
    noAtual.pDir = -1;
    noAtual.posOriginal = -1;
    noAtual.chave = -1;

    // Começa na raiz
    fseek(arquivo, 0, SEEK_SET);

    while (fread(&noAtual, sizeof(tipoitemarvore), 1, arquivo) == 1) {
        (*acessos)++;
        (*comparacoes)++;

        if (chave == noAtual.chave) {
            return noAtual.posOriginal; // Retorna a posição do item
        } else if (chave < noAtual.chave) {
            if (noAtual.pEsq == -1) return -1; // Não encontrado
            fseek(arquivo, noAtual.pEsq * sizeof(tipoitemarvore), SEEK_SET);
        } else {
            if (noAtual.pDir == -1) return -1; // Não encontrado
            fseek(arquivo, noAtual.pDir * sizeof(tipoitemarvore), SEEK_SET);
        }
    }
    return -1;
}

void manipulaArvoreBinaria(FILE *arq, int qtd, int chave, dados *cont) {
    tipoitem temp;
    tipoitemarvore itemcompleto;
    
    FILE *arqArvore = fopen("./data/arvore_binaria.bin", "wb+");
    if (arqArvore == NULL) {
        fprintf(stderr, "Erro ao abrir o arquivo da arvore binaria.\n");
        exit(EXIT_FAILURE);
    }

    // Randomiza ordem de insercao
    srand(time(NULL));
    int *indices = malloc(qtd * sizeof(int));
    if (indices == NULL) {
        fprintf(stderr, "Erro ao alocar memoria para os indices.\n");
        fclose(arqArvore);
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < qtd; i++) {
        indices[i] = i;
    }

    for (int i = qtd - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int temp = indices[i];
        indices[i] = indices[j];
        indices[j] = temp;
    }

    // Insere os elementos em ordem aleatoria
    for (int i = 0; i < qtd; i++) {
        fseek(arq, indices[i] * sizeof(tipoitem), SEEK_SET);
        fread(&temp, sizeof(tipoitem), 1, arq);

        itemcompleto.chave = temp.chave;
        itemcompleto.dado1 = temp.dado1;
        strcpy(itemcompleto.dado2, temp.dado2);
        itemcompleto.pEsq = -1;
        itemcompleto.pDir = -1;
        itemcompleto.posOriginal = i;

        fseek(arqArvore, i * sizeof(tipoitemarvore), SEEK_SET);
        fwrite(&itemcompleto, sizeof(tipoitemarvore), 1, arqArvore);
        cont->acessos++;
    }

    free(indices);

    // Atualiza ponteiros
    for (int i = 1; i < qtd; i++) {
        atualizaPonteiroAB(i, arqArvore, &cont->acessos, &cont->comparacoes);
    }

    // Procura a chave
    int retorno = ProcuraAB(chave, arqArvore, &cont->acessos, &cont->comparacoes);
    if (retorno != -1) {
        printf("Registro de código %d foi localizado\n", chave);
    } else {
        printf("Registro de código %d não foi localizado\n", chave);
    }

    fclose(arqArvore);
}