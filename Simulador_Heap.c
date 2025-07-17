#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define HEAP_SIZE 60
#define MAX_VARS 20

int heap[HEAP_SIZE];  // 0 = livre, 1 = ocupado

typedef enum { FIRST_FIT, WORST_FIT, BEST_FIT } Estrategia;

Estrategia estrategiaAtual = FIRST_FIT;

typedef struct no {
    int inicio;
    int tamanho;
    struct no *prox;
} No;

typedef struct var {
    char id[10];
    int inicio;
    int tamanho;
    char referencia[10];  // se x = y, então x.referencia = "y"
} Variavel;

No *listaLivres = NULL;
Variavel tabela[MAX_VARS];
int qtd_variaveis = 0;

void inicializaHeap() {
    for (int i = 0; i < HEAP_SIZE; i++) {
        heap[i] = 0;
    }

    while (listaLivres != NULL) {
        No *temp = listaLivres;
        listaLivres = listaLivres->prox;
        free(temp);
    }

    listaLivres = malloc(sizeof(No));
    if (listaLivres != NULL) {
        listaLivres->inicio = 0;
        listaLivres->tamanho = HEAP_SIZE;
        listaLivres->prox = NULL;
    }
}

int UsaFirstFit(const char *id, int tamanho) {
    No *atual = listaLivres;
    No *anterior = NULL;

    while (atual != NULL) {
        if (atual->tamanho >= tamanho) {
            int inicioAlocacao = atual->inicio;
            for (int i = inicioAlocacao; i < inicioAlocacao + tamanho; i++) {
                heap[i] = 1;
            }

            strcpy(tabela[qtd_variaveis].id, id);
            tabela[qtd_variaveis].inicio = inicioAlocacao;
            tabela[qtd_variaveis].tamanho = tamanho;
            strcpy(tabela[qtd_variaveis].referencia, "");
            qtd_variaveis++;

            if (atual->tamanho == tamanho) {
                if (anterior == NULL)
                    listaLivres = atual->prox;
                else
                    anterior->prox = atual->prox;
                free(atual);
            } else {
                atual->inicio += tamanho;
                atual->tamanho -= tamanho;
            }
            return 1;
        }
        anterior = atual;
        atual = atual->prox;
    }
    printf("Memoria insuficiente para alocar %s\n", id);
    return 0;
}

int UsaBestFit(const char *id, int tamanho) {
    No *melhor = NULL;
    No *antMelhor = NULL;
    No *atual = listaLivres;
    No *anteriorTemp = NULL;

    while (atual != NULL) {
        if (atual->tamanho >= tamanho) {
            if (melhor == NULL || atual->tamanho < melhor->tamanho) {
                melhor = atual;
                antMelhor = anteriorTemp;
            }
        }
        anteriorTemp = atual;
        atual = atual->prox;
    }

    if (melhor == NULL) {
        printf("Memoria insuficiente para alocar %s\n", id);
        return 0;
    }

    int inicioAloc = melhor->inicio;
    for (int i = inicioAloc; i < inicioAloc + tamanho; i++) {
        heap[i] = 1;
    }

    strcpy(tabela[qtd_variaveis].id, id);
    tabela[qtd_variaveis].inicio = inicioAloc;
    tabela[qtd_variaveis].tamanho = tamanho;
    strcpy(tabela[qtd_variaveis].referencia, "");
    qtd_variaveis++;

    if (melhor->tamanho == tamanho) {
        if (antMelhor == NULL)
            listaLivres = melhor->prox;
        else
            antMelhor->prox = melhor->prox;
        free(melhor);
    } else {
        melhor->inicio += tamanho;
        melhor->tamanho -= tamanho;
    }
    return 1;
}

int UsaWorstFit(const char *id, int tamanho) {
    No *pior = NULL;
    No *antPior = NULL;
    No *atual = listaLivres;
    No *anteriorTemp = NULL;

    while (atual != NULL) {
        if (atual->tamanho >= tamanho) {
            if (pior == NULL || atual->tamanho > pior->tamanho) {
                pior = atual;
                antPior = anteriorTemp;
            }
        }
        anteriorTemp = atual;
        atual = atual->prox;
    }

    if (pior == NULL) {
        printf("Memoria insuficiente para alocar %s\n", id);
        return 0;
    }

    int inicioAloc = pior->inicio;
    for (int i = inicioAloc; i < inicioAloc + tamanho; i++) {
        heap[i] = 1;
    }

    strcpy(tabela[qtd_variaveis].id, id);
    tabela[qtd_variaveis].inicio = inicioAloc;
    tabela[qtd_variaveis].tamanho = tamanho;
    strcpy(tabela[qtd_variaveis].referencia, "");
    qtd_variaveis++;

    if (pior->tamanho == tamanho) {
        if (antPior == NULL)
            listaLivres = pior->prox;
        else
            antPior->prox = pior->prox;
        free(pior);
    } else {
        pior->inicio += tamanho;
        pior->tamanho -= tamanho;
    }
    return 1;
}

void del(const char *id) {
    int nova_qtd = 0;
    int liberouAlgo = 0;

    for (int i = 0; i < qtd_variaveis; i++) {
        int deveRemover = 0;

        if (strcmp(tabela[i].id, id) == 0)
            deveRemover = 1;

        if (strcmp(tabela[i].referencia, id) == 0)
            deveRemover = 1;

        if (deveRemover) {
            for (int j = tabela[i].inicio; j < tabela[i].inicio + tabela[i].tamanho; j++)
                heap[j] = 0;
            liberouAlgo = 1;

            No *novo = malloc(sizeof(No));
            novo->inicio = tabela[i].inicio;
            novo->tamanho = tabela[i].tamanho;
            novo->prox = NULL;

            if (listaLivres == NULL || novo->inicio < listaLivres->inicio) {
                novo->prox = listaLivres;
                listaLivres = novo;
            } else {
                No *aux = listaLivres;
                while (aux->prox != NULL && aux->prox->inicio < novo->inicio)
                    aux = aux->prox;
                novo->prox = aux->prox;
                aux->prox = novo;
            }
            continue;
        }
        tabela[nova_qtd++] = tabela[i];
    }

    qtd_variaveis = nova_qtd;

    if (!liberouAlgo)
        printf("\nVariavel %s nao encontrada.\n", id);

    No *atual = listaLivres;
    while (atual != NULL && atual->prox != NULL) {
        if (atual->inicio + atual->tamanho == atual->prox->inicio) {
            atual->tamanho += atual->prox->tamanho;
            No *temp = atual->prox;
            atual->prox = temp->prox;
            free(temp);
        } else {
            atual = atual->prox;
        }
    }
}

void Atribuicao(const char *x, const char *y) {
    int i;
    for (i = 0; i < qtd_variaveis; i++) {
        if (strcmp(tabela[i].id, y) == 0)
            break;
    }
    if (i == qtd_variaveis) {
        printf("\nVariavel %s nao encontrada\n", y);
        return;
    }

    int tamanho = tabela[i].tamanho;
    int alocado = 0;

    if (estrategiaAtual == FIRST_FIT)
        alocado = UsaFirstFit(x, tamanho);
    else if (estrategiaAtual == WORST_FIT)
        alocado = UsaWorstFit(x, tamanho);
    else
        alocado = UsaBestFit(x, tamanho);


    if (!alocado) {
        printf("Erro ao alocar memoria para copia de %s\n", y);
        return;
    }

    strcpy(tabela[qtd_variaveis - 1].referencia, y);
}

void exibe() {
    system("pause");
    system("cls");
    printf("\tSimulador de Heap\n");
    printf("\nEstrategias: first | best| worst\n\n");
    printf("\tComandos validos:\n");
    printf("\theap <estrategia>\n\tnew <var> <tam>\n\tdel <var>\n\t<var> = <var>\n\texibe\n\tsair\n\n");

    printf("\n\t\t\t Heap \n");
    for (int i = 0; i < HEAP_SIZE; i++) {
        if (heap[i] == 0)
            printf("| "); // livre
        else
            printf("|X"); // ocupado
    }
    printf("\n");

    printf("\n\t Areas Livres\n");
    No *atual = listaLivres;
    while (atual != NULL) {
        printf("[Inicio: %d | Tamanho: %d]\n", atual->inicio, atual->tamanho);
        atual = atual->prox;
    }

    printf("\n\t Variaveis Alocadas \n");
    for (int i = 0; i < qtd_variaveis; i++) {
        printf("%s: inicio = %d, tamanho = %d\n",
               tabela[i].id, tabela[i].inicio, tabela[i].tamanho);
    }
    printf("\n");
}

void InterpretarComando(char *linha) {
    char cmd[20], arg1[10], arg2[10];
    int tam;

    if (sscanf(linha, "heap %s", cmd) == 1) {
        if (strcmp(cmd, "first") == 0)
            estrategiaAtual = FIRST_FIT;
        else if (strcmp(cmd, "best") == 0)
            estrategiaAtual = BEST_FIT;
        else if (strcmp(cmd, "worst") == 0)
            estrategiaAtual = WORST_FIT;
        else
            printf("Estrategia invalida\n");
    } else if (sscanf(linha, "new %s %d", arg1, &tam) == 2) {
        if (estrategiaAtual == FIRST_FIT)
            UsaFirstFit(arg1, tam);
        else if (estrategiaAtual == WORST_FIT)
            UsaWorstFit(arg1, tam);
        else
            UsaBestFit(arg1, tam);
    } else if (sscanf(linha, "del %s", arg1) == 1) {
        del(arg1);
    } else if (sscanf(linha, "%s = %s", arg1, arg2) == 2) {
        Atribuicao(arg1, arg2);
    } else if (strcmp(linha, "exibe") == 0) {
        exibe();
    } else {
        printf("Comando invalido\n");
    }
}

int main() {
    inicializaHeap();
    char linha[20];

    printf("\tSimulador de Heap\n");
    printf("\nEstrategias: first | best | worst\n\n");
    printf("\tComandos validos:\n");
    printf("\theap <estrategia>\n\tnew <var> <tam>\n\tdel <var>\n\t<var> = <var>\n\texibe\n\tsair\n\n");

    while (1) {
        printf(">>> ");
        fgets(linha, sizeof(linha), stdin);
        linha[strcspn(linha, "\n")] = '\0';

        if (strcmp(linha, "sair") == 0)
            break;

        InterpretarComando(linha);
    }

    return 0;
}
