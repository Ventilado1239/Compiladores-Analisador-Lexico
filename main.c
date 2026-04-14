/*
 * main.c
 * Programa principal do analisador lexico para a linguagem MicroPascal.
 *
 * Uso: lexico <arquivo.pas>
 *   ou: lexico              (usa "teste.pas" como padrao)
 *
 * Gera tres arquivos de saida:
 *   - <nome>.lex : tokens reconhecidos no formato <TOKEN, lexema> linha coluna
 *   - <nome>.ts  : tabela de simbolos completa
 *   - <nome>.err : lista de erros lexicos encontrados
 *
 * O programa:
 *   1. Inicializa a Tabela de Simbolos com as palavras reservadas
 *   2. Le o arquivo-fonte caractere a caractere (via scanner/AFD)
 *   3. Para cada token reconhecido, escreve no .lex
 *   4. Erros lexicos sao armazenados e escritos no .err
 *   5. Ao final, grava a TS no .ts
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "token.h"
#include "scanner.h"
#include "ts.h"

#define MAX_ERROS 500
#define MAX_NOME  512

/* Estrutura para armazenar erros lexicos */
typedef struct {
    char tipo[MAX_TIPO];
    char lexema[MAX_LEXEMA];
    int linha;
    int coluna;
} ErroLexico;

/*
 * extrairNomeBase
 * Extrai o nome do arquivo sem a extensao.
 * Ex: "teste.pas" -> "teste"
 */
static void extrairNomeBase(const char *caminho, char *base) {
    const char *inicio = caminho;
    const char *ultimaBarra = NULL;
    const char *p;

    /* Encontrar a ultima barra (/ ou \) para pegar so o nome do arquivo */
    for (p = caminho; *p; p++) {
        if (*p == '/' || *p == '\\')
            ultimaBarra = p;
    }
    if (ultimaBarra)
        inicio = ultimaBarra + 1;

    strcpy(base, inicio);

    /* Remover extensao */
    char *ponto = strrchr(base, '.');
    if (ponto)
        *ponto = '\0';
}

/*
 * ehErro
 * Verifica se o tipo do token e um erro lexico.
 * Erros comecam com "ERR_".
 */
static int ehErro(const char *tipo) {
    return strncmp(tipo, "ERR_", 4) == 0;
}

int main(int argc, char *argv[]) {
    const char *nomeEntrada;
    char nomeBase[MAX_NOME];
    char nomeLex[MAX_NOME];
    char nomeTS[MAX_NOME];
    char nomeErr[MAX_NOME];

    ErroLexico erros[MAX_ERROS];
    int totalErros = 0;

    /* Determina o arquivo de entrada */
    if (argc >= 2) {
        nomeEntrada = argv[1];
    } else {
        nomeEntrada = "teste.pas";
    }

    /* Gera nomes dos arquivos de saida */
    extrairNomeBase(nomeEntrada, nomeBase);
    snprintf(nomeLex, MAX_NOME, "%s.lex", nomeBase);
    snprintf(nomeTS,  MAX_NOME, "%s.ts",  nomeBase);
    snprintf(nomeErr, MAX_NOME, "%s.err", nomeBase);

    /* Abre arquivo-fonte */
    FILE *entrada = fopen(nomeEntrada, "r");
    if (!entrada) {
        printf("Erro: nao foi possivel abrir o arquivo '%s'\n", nomeEntrada);
        return 1;
    }

    /* Abre arquivo de saida .lex */
    FILE *arqLex = fopen(nomeLex, "w");
    if (!arqLex) {
        printf("Erro: nao foi possivel criar o arquivo '%s'\n", nomeLex);
        fclose(entrada);
        return 1;
    }

    /* Inicializa a Tabela de Simbolos com as palavras reservadas */
    inicializarTS();

    /* Inicializa o scanner com o arquivo-fonte */
    initScanner(entrada);

    printf("Analisador Lexico MicroPascal\n");
    printf("Arquivo de entrada: %s\n", nomeEntrada);
    printf("Processando...\n\n");

    /* Loop principal: obtem tokens ate o fim do arquivo */
    Token t;
    int totalTokens = 0;

    while (1) {
        t = proximoToken();

        /* Token vazio = fim do arquivo */
        if (t.tipo[0] == '\0')
            break;

        /* Verifica se e um erro lexico */
        if (ehErro(t.tipo)) {
            /* Armazena o erro */
            if (totalErros < MAX_ERROS) {
                strcpy(erros[totalErros].tipo, t.tipo);
                strcpy(erros[totalErros].lexema, t.lexema);
                erros[totalErros].linha = t.linha;
                erros[totalErros].coluna = t.coluna;
                totalErros++;
            }
            /* Imprime o erro no console tambem */
            printf("ERRO LEXICO: <%s, %s> linha %d coluna %d\n",
                   t.tipo, t.lexema, t.linha, t.coluna);
        } else {
            /* Token valido: escreve no .lex */
            fprintf(arqLex, "<%s, %s> %d %d\n",
                    t.tipo, t.lexema, t.linha, t.coluna);
            totalTokens++;
        }
    }

    fclose(arqLex);
    fclose(entrada);

    /* Gera arquivo .ts (Tabela de Simbolos) */
    FILE *arqTS = fopen(nomeTS, "w");
    if (arqTS) {
        gravarTS(arqTS);
        fclose(arqTS);
    }

    /* Gera arquivo .err (Erros Lexicos) */
    FILE *arqErr = fopen(nomeErr, "w");
    if (arqErr) {
        if (totalErros == 0) {
            fprintf(arqErr, "Nenhum erro lexico encontrado.\n");
        } else {
            fprintf(arqErr, "===== ERROS LEXICOS =====\n");
            fprintf(arqErr, "%-25s %-15s %-6s %-6s\n",
                    "Tipo do Erro", "Lexema", "Linha", "Coluna");
            fprintf(arqErr, "------------------------------------------------------\n");
            for (int i = 0; i < totalErros; i++) {
                fprintf(arqErr, "%-25s %-15s %-6d %-6d\n",
                        erros[i].tipo, erros[i].lexema,
                        erros[i].linha, erros[i].coluna);
            }
            fprintf(arqErr, "------------------------------------------------------\n");
            fprintf(arqErr, "Total de erros: %d\n", totalErros);
        }
        fclose(arqErr);
    }

    /* Imprime a TS no console */
    imprimirTS();

    /* Resumo final */
    printf("\n===== RESUMO =====\n");
    printf("Tokens reconhecidos: %d\n", totalTokens);
    printf("Erros lexicos:       %d\n", totalErros);
    printf("\nArquivos gerados:\n");
    printf("  - %s (tokens)\n", nomeLex);
    printf("  - %s (tabela de simbolos)\n", nomeTS);
    printf("  - %s (erros lexicos)\n", nomeErr);

    return 0;
}