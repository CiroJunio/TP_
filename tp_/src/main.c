#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "../include/acesso_sequencial.h"
#include "../include/arvore_binaria.h"
#include "../include/arvore_b.h"
#include "../include/arvore_b_star.h"
#include "../include/registro.h"
#include "../include/utils.h"

// Arquivo decrescente vai de 1999999 até 0

// Definindo limites para a quantidade de registros
#define MAX_REGISTROS 2000000
#define MIN_REGISTROS 1

int main(int argc, char *argv[])
{
    // Verificando se o número correto de argumentos foi passado
    if (argc < 5 || argc > 6)
    {
        printf("Uso incorreto. Sintaxe esperada: pesquisa <metodo> <quantidade> <situacao> <chave> [-P]\n");
        return 0;
    }

    int metodo = atoi(argv[1]);
    int qtd = atoi(argv[2]);
    int situacao = atoi(argv[3]);
    int chave = atoi(argv[4]);

    // Validando as entradas fornecidas pelo usuário
    if ((metodo < 1 || metodo > 4) || (qtd < MIN_REGISTROS || qtd > MAX_REGISTROS) || (situacao < 1 || situacao > 3))
    {
        printf("Entrada inválida!\n\n");
        printf("Métodos:\n1: Pesquisa sequencialmente indexada (Apenas ordem crescente)\n");
        printf("2: Árvore binária externa\n3: Árvore B\n4: Árvore B*\n\n");
        printf("Quantidade permitida: Entre %d e %d itens\n", MIN_REGISTROS, MAX_REGISTROS);
        printf("Situações:\n1: Ordenado crescentemente\n2: Ordenado decrescentemente\n3: Ordenado aleatoriamente\n");
        return 0;
    }

    FILE *arq;
    tipoitem aux;
    int acessos = 0, comparacoes = 0, contarq = 0;
    dados cont = {0, 0};
    const char *nomeArquivo = NULL;
    int crescente = 1; // Variável para controlar a direção (1 para crescente, 0 para decrescente)
    // Abrindo o arquivo de acordo com a situação especificada
    if (situacao == 1)
    {
        nomeArquivo = "./data/crescente.bin"; 
        gerarCrescente(qtd);
        arq = fopen("./data/crescente.bin", "rb");
    }
    else if (situacao == 2)
    {
        nomeArquivo = "./data/decrescente.bin";
        gerarDecrescente(qtd);
        arq = fopen("./data/decrescente.bin", "rb");
        crescente = 0; // Se for decrescente, a direção muda para 0
    }
    else
    {
        nomeArquivo = "./data/aleatorio.bin";
        gerarAleatorio(qtd);
        arq = fopen("./data/aleatorio.bin", "rb");
    }

    if (arq == NULL)
    {
        printf("Erro na abertura do arquivo. Verifique o caminho e tente novamente.\n");
        return 0;
    }

    // Se o argumento -P for passado, imprime as chaves do arquivo
    if (argc == 6 && strcmp(argv[5], "-P") == 0)
    {
        while (fread(&aux, sizeof(tipoitem), 1, arq) > 0 && contarq < qtd)
        {
            printf("%d ", aux.chave);
            contarq++;
        }
        printf("\n");
        rewind(arq); // Volta ao início do arquivo após a impressão
    }

    // Iniciando a medição de tempo após a criação dos registros
    clock_t start, end;
    double elapsed;
    // realizaExperimentos();
    srand(time(NULL));
    start = clock();

    // Executando o método de pesquisa conforme a escolha do usuário
    switch (metodo)
    {
    case 1: // Pesquisa Sequencialmente Indexada
    {
        if (acesso_sequencial_indexado(qtd, situacao, chave, &cont, nomeArquivo) == 1) {
            printf("Registro de código %d foi localizado\n", chave);
        } else {
            printf("Registro de código %d não foi localizado\n", chave);
        }
        fclose(arq);
        break;
    }
    case 2: // Árvore Binária
    {
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
            acessos++;
        }

        free(indices);

        // Atualiza ponteiros
        for (int i = 1; i < qtd; i++) {
            atualizaPonteiroAB(i, arqArvore, &acessos, &comparacoes);
        }

        // Procura a chave
        int retorno = ProcuraAB(chave, arqArvore, &acessos, &comparacoes);
        if (retorno != -1) {
            printf("Registro de código %d foi localizado\n", chave);
        } else {
            printf("Registro de código %d não foi localizado\n", chave);
        }

        fclose(arqArvore);
        break;
        manipulaArvoreBinaria(arq, qtd, chave, &cont);
    }

    case 3: // Árvore B
    {
        tipoitem temp;
        TipoApontador Ap = NULL;
        Inicializa(&Ap);
        int cont = 0;

        while (fread(&temp, sizeof(tipoitem), 1, arq) == 1 && cont < qtd)
        {
            InsereAB(temp, &Ap, &comparacoes, crescente); // Passando a direção da ordenação
            cont++;
            acessos++;
        }
        fclose(arq);

        int retorno = PesquisaAB(chave, Ap, &comparacoes, crescente); // Passando a direção da pesquisa
        if (retorno > 0)
        {
            printf("Registro de código %d foi localizado\n", chave);
        }
        else
        {
            printf("Registro de código %d não foi localizado\n", chave);
        }
        limpar(Ap);
        break;
    }
    case 4: // Árvore B*
    {
        tipoitem temp, pesquisado;
        pesquisado.chave = chave;
        TipoApontador2 Ap = NULL;
        int cont = 0;

        while (fread(&temp, sizeof(tipoitem), 1, arq) == 1 && cont < qtd)
        {
            InsereEstrela(temp, &Ap, &comparacoes);
            cont++;
            acessos++;
        }
        rewind(arq);
        fclose(arq);

        int retorno = PesquisaEstrela(&pesquisado, &Ap, &comparacoes);
        if (retorno)
        {
            printf("Registro de código %d foi localizado\n", chave);
        }
        else
        {
            printf("Registro de código %d não foi localizado\n", chave);
        }
        break;
    }
    }

    // Calculando o tempo de execução com maior precisão
    end = clock();
    elapsed = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("\nTempo de execução: %f segundos\nTransferências: %d\nComparações: %d\n", elapsed, acessos, comparacoes);

    return 0;
}
