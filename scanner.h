/*
 * scanner.h
 * Interface do analisador lexico (scanner) para a linguagem MicroPascal.
 *
 * O scanner le caracteres de um arquivo-fonte e retorna tokens
 * um a um, baseado em um Automato Finito Deterministico (AFD).
 *
 * Funcoes:
 *   - initScanner: inicializa o scanner com o ponteiro do arquivo-fonte
 *   - proximoToken: retorna o proximo token reconhecido
 */

#ifndef SCANNER_H
#define SCANNER_H

#include <stdio.h>
#include "token.h"

/* Inicializa o scanner com o arquivo-fonte a ser analisado */
void initScanner(FILE *fonte);

/* Retorna o proximo token do arquivo-fonte.
 * Quando o arquivo termina, retorna um token com tipo vazio (tipo[0] == '\0').
 */
Token proximoToken(void);

#endif