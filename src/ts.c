/*
 * ts.c
 * Implementacao da Tabela de Simbolos para o analisador lexico MicroPascal.
 *
 * Estrutura de dados: vetor de structs (EntradaTS) com busca linear.
 * Todas as comparacoes sao case-insensitive pois a linguagem MicroPascal
 * nao diferencia maiusculas de minusculas.
 *
 * Funcoes implementadas:
 *   - inicializarTS: carrega as 11 palavras reservadas na tabela
 *   - buscarTS:      busca case-insensitive por lexema
 *   - inserirTS:     insere apenas se o lexema ainda nao existir
 *   - obterTipoTS:   retorna o tipo de token de uma entrada
 *   - imprimirTS:    imprime a tabela no console
 *   - gravarTS:      grava a tabela em um arquivo .ts
 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "ts.h"

/* Tabela de simbolos e contador de entradas */
static EntradaTS tabela[MAX_TS];
static int topo = 0;

/*
 * Funcao auxiliar para comparacao case-insensitive de strings.
 * Retorna 0 se iguais (como strcmp), valor diferente de 0 caso contrario.
 */
static int strcasecmp_local(const char *s1, const char *s2) {
    while (*s1 && *s2) {
        if (tolower((unsigned char)*s1) != tolower((unsigned char)*s2))
            return tolower((unsigned char)*s1) - tolower((unsigned char)*s2);
        s1++;
        s2++;
    }
    return tolower((unsigned char)*s1) - tolower((unsigned char)*s2);
}

/*
 * inicializarTS
 * Pre-carrega as 11 palavras reservadas da linguagem MicroPascal.
 * Deve ser chamada uma unica vez, antes de iniciar a analise lexica.
 */
void inicializarTS(void) {
    topo = 0;

    /* Palavras reservadas e seus respectivos tokens */
    const char *reservadas[][2] = {
        {"program",  "KW_PROGRAM"},
        {"var",      "KW_VAR"},
        {"integer",  "KW_INTEGER"},
        {"real",     "KW_REAL"},
        {"begin",    "KW_BEGIN"},
        {"end",      "KW_END"},
        {"if",       "KW_IF"},
        {"then",     "KW_THEN"},
        {"else",     "KW_ELSE"},
        {"while",    "KW_WHILE"},
        {"do",       "KW_DO"}
    };

    int total = sizeof(reservadas) / sizeof(reservadas[0]);

    for (int i = 0; i < total; i++) {
        strcpy(tabela[topo].lexema, reservadas[i][0]);
        strcpy(tabela[topo].tipo, reservadas[i][1]);
        topo++;
    }
}

/*
 * buscarTS
 * Busca um lexema na tabela de simbolos usando comparacao case-insensitive.
 * Retorna o indice da entrada encontrada, ou -1 se nao encontrado.
 */
int buscarTS(const char *lexema) {
    for (int i = 0; i < topo; i++) {
        if (strcasecmp_local(tabela[i].lexema, lexema) == 0)
            return i;
    }
    return -1;
}

/*
 * inserirTS
 * Insere um novo lexema na TS somente se ele ainda nao existir.
 * Retorna o indice da entrada (existente ou recem-inserida).
 */
int inserirTS(const char *lexema, const char *tipo) {
    int idx = buscarTS(lexema);
    if (idx != -1)
        return idx;

    if (topo >= MAX_TS) {
        fprintf(stderr, "Erro: Tabela de Simbolos cheia!\n");
        return -1;
    }

    strcpy(tabela[topo].lexema, lexema);
    strcpy(tabela[topo].tipo, tipo);
    idx = topo;
    topo++;
    return idx;
}

/*
 * obterTipoTS
 * Retorna o tipo de token da entrada no indice fornecido.
 * Retorna NULL se o indice for invalido.
 */
const char *obterTipoTS(int indice) {
    if (indice < 0 || indice >= topo)
        return NULL;
    return tabela[indice].tipo;
}

/*
 * imprimirTS
 * Imprime a tabela de simbolos completa no console (para depuracao).
 */
void imprimirTS(void) {
    printf("\n===== TABELA DE SIMBOLOS =====\n");
    printf("%-5s %-20s %-15s\n", "Idx", "Lexema", "Tipo de Token");
    printf("----------------------------------------------\n");
    for (int i = 0; i < topo; i++) {
        printf("%-5d %-20s %-15s\n", i, tabela[i].lexema, tabela[i].tipo);
    }
    printf("----------------------------------------------\n");
    printf("Total de entradas: %d\n", topo);
}

/*
 * gravarTS
 * Grava a tabela de simbolos em um arquivo .ts com formatacao tabular.
 */
void gravarTS(FILE *arq) {
    fprintf(arq, "===== TABELA DE SIMBOLOS =====\n");
    fprintf(arq, "%-5s %-20s %-15s\n", "Idx", "Lexema", "Tipo de Token");
    fprintf(arq, "----------------------------------------------\n");
    for (int i = 0; i < topo; i++) {
        fprintf(arq, "%-5d %-20s %-15s\n", i, tabela[i].lexema, tabela[i].tipo);
    }
    fprintf(arq, "----------------------------------------------\n");
    fprintf(arq, "Total de entradas: %d\n", topo);
}