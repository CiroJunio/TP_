#include "../include/arvore_b.h"

// Inicializa a árvore B como vazia
void Inicializa(TipoApontador *Arvore) {
    *Arvore = NULL;
}

// Insere um registro na árvore B
void InsereAB(tipoitem Reg, TipoApontador *Ap, int *comparacoes, int crescente) {
    short Cresceu;
    tipoitem RegRetorno;
    TipoPagina *ApRetorno, *ApTemp;

    // Chama a função recursiva de inserção
    Ins(Reg, *Ap, &Cresceu, &RegRetorno, &ApRetorno, comparacoes, crescente);

    // Se a raiz cresceu, cria uma nova raiz
    if (Cresceu) {
        ApTemp = malloc(sizeof(TipoPagina));
        ApTemp->n = 1;
        ApTemp->r[0] = RegRetorno;
        ApTemp->p[1] = ApRetorno;
        ApTemp->p[0] = *Ap;
        *Ap = ApTemp;
    }
}

// Função recursiva de inserção
void Ins(tipoitem Reg, TipoApontador Ap, short *Cresceu, tipoitem *RegRetorno, 
         TipoApontador *ApRetorno, int *comparacoes, int crescente) {
    long i = 1;
    TipoApontador ApTemp;

    // Se a página for NULL, estamos em uma folha
    if (Ap == NULL) {
        *Cresceu = 1;
        *RegRetorno = Reg;
        *ApRetorno = NULL;
        return;
    }

    // Localiza a posição correta para inserção (crescente ou decrescente)
    while (i < Ap->n && (crescente ? Reg.chave > Ap->r[i - 1].chave : Reg.chave < Ap->r[i - 1].chave)) {
        (*comparacoes)++;
        i++;
    }

    // Verifica se a chave já existe
    if (i > 0 && Reg.chave == Ap->r[i - 1].chave) {
        (*comparacoes)++;
        *Cresceu = 0;
        return;
    }

    // Continua a busca recursiva na subárvore correta
    Ins(Reg, Ap->p[i], Cresceu, RegRetorno, ApRetorno, comparacoes, crescente);

    // Se a subárvore não cresceu, retorna
    if (!(*Cresceu)) 
        return;

    // Caso a página tenha espaço, insere o registro
    if (Ap->n < MM) {
        InsereNaPagina(Ap, *RegRetorno, *ApRetorno, comparacoes, crescente);
        *Cresceu = 0;
        return;
    }

    // Divide a página se estiver cheia
    ApTemp = malloc(sizeof(TipoPagina));
    ApTemp->n = 0;
    ApTemp->p[0] = NULL;

    // Insere o registro na página apropriada
    if (i < (M + 1)) {
        InsereNaPagina(ApTemp, Ap->r[MM - 1], Ap->p[MM], comparacoes, crescente);
        Ap->n--;
        InsereNaPagina(Ap, *RegRetorno, *ApRetorno, comparacoes, crescente);
    } else {
        InsereNaPagina(ApTemp, *RegRetorno, *ApRetorno, comparacoes, crescente);
    }

    // Move os registros excedentes para a nova página
    for (int j = M + 2; j <= MM; j++) {
        InsereNaPagina(ApTemp, Ap->r[j - 1], Ap->p[j], comparacoes, crescente);
    }

    // Atualiza as informações das páginas
    Ap->n = M;
    ApTemp->p[0] = Ap->p[M + 1];
    *RegRetorno = Ap->r[M];
    *ApRetorno = ApTemp;
}

// Insere um registro em uma página específica
void InsereNaPagina(TipoApontador Ap, tipoitem Reg, TipoApontador Apdir, int *comparacoes, int crescente) {
    int k = Ap->n;

    // Localiza a posição correta e move os elementos
    while (k > 0 && (crescente ? Reg.chave < Ap->r[k - 1].chave : Reg.chave > Ap->r[k - 1].chave)) {
        (*comparacoes)++;
        Ap->r[k] = Ap->r[k - 1];
        Ap->p[k + 1] = Ap->p[k];
        k--;
    }

    // Insere o registro na posição correta
    Ap->r[k] = Reg;
    Ap->p[k + 1] = Apdir;
    Ap->n++;
}

// Libera a memória da árvore
void limpar(TipoApontador arvore) {
    if (arvore == NULL) return;
    for (int i = 0; i <= arvore->n; i++) {
        limpar(arvore->p[i]);
    }
    free(arvore);
}

// Pesquisa um registro na árvore B
int PesquisaAB(int chave, TipoApontador Ap, int *comparacoes, int crescente) {
    long i = 1;

    // Verifica se a página é nula
    if (Ap == NULL) return 0;

    // Localiza a posição do registro ou do seu pai
    while (i < Ap->n && (crescente ? chave > Ap->r[i - 1].chave : chave < Ap->r[i - 1].chave)) {
        (*comparacoes)++;
        i++;
    }

    // Verifica se encontrou o registro
    if (chave == Ap->r[i - 1].chave) {
        (*comparacoes)++;
        return 1;
    }

    // Continua a busca na subárvore apropriada
    if (crescente ? chave < Ap->r[i - 1].chave : chave > Ap->r[i - 1].chave) {
        (*comparacoes)++;
        return PesquisaAB(chave, Ap->p[i - 1], comparacoes, crescente);
    } else {
        (*comparacoes)++;
        return PesquisaAB(chave, Ap->p[i], comparacoes, crescente);
    }
}


void processaArvoreB(const char *nomeArquivo, int qtd, int chave, dados *cont, int crescente) {
    FILE *arq = fopen(nomeArquivo, "rb");  // Abre o arquivo no modo leitura binária
    if (arq == NULL) {
        printf("Erro ao abrir o arquivo: %s\n", nomeArquivo);
        return;
    }

    tipoitem temp;
    TipoApontador Ap = NULL;
    Inicializa(&Ap);  // Inicializa a árvore B
    int contreg = 0;

    // Insere os registros na árvore B
    while (fread(&temp, sizeof(tipoitem), 1, arq) == 1 && contreg < qtd) {
        InsereAB(temp, &Ap, &cont->comparacoes, crescente);  // Insere na árvore com ordenação definida
        contreg++;
        cont->acessos++;  // Conta os acessos
    }

    fclose(arq);  // Fecha o arquivo após a inserção

    // Pesquisa o registro na árvore B
    int retorno = PesquisaAB(chave, Ap, &cont->comparacoes, crescente);  // Pesquisa pelo registro
    if (retorno > 0) {
        printf("Registro de código %d foi localizado\n", chave);
    } else {
        printf("Registro de código %d não foi localizado\n", chave);
    }

    limpar(Ap);  // Limpa a árvore B após o uso
}


