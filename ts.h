/*
 * ts.h
 * Interface da Tabela de Simbolos para o analisador lexico MicroPascal.
 *
 * A tabela armazena lexemas e seus tipos de token.
 * Deve ser inicializada com as palavras reservadas antes da analise.
 * A linguagem e case-insensitive: todas as comparacoes sao feitas
 * convertendo para minusculo.
 */

#ifndef TS_H
#define TS_H

#include <stdio.h>

#define MAX_TS 1000

/* Estrutura de cada entrada na Tabela de Simbolos */
typedef struct {
    char lexema[100];
    char tipo[30];
} EntradaTS;

/* Inicializa a TS com as palavras reservadas da linguagem */
void inicializarTS(void);

/* Busca um lexema na TS (case-insensitive). Retorna o indice ou -1 */
int buscarTS(const char *lexema);

/* Insere um lexema na TS se ainda nao existir. Retorna o indice */
int inserirTS(const char *lexema, const char *tipo);

/* Retorna o tipo de token de uma entrada ja existente na TS */
const char *obterTipoTS(int indice);

/* Imprime a TS no console */
void imprimirTS(void);

/* Grava a TS em um arquivo */
void gravarTS(FILE *arq);

#endif