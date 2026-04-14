/*
 * token.h
 * Definicao da estrutura Token para o analisador lexico MicroPascal.
 *
 * Cada token contem:
 *   - tipo:   nome do token (ex: KW_PROGRAM, ID, OP_ASS, etc.)
 *   - lexema: sequencia de caracteres reconhecida
 *   - linha:  linha onde o token foi encontrado
 *   - coluna: coluna onde o token inicia
 */

#ifndef TOKEN_H
#define TOKEN_H

#define MAX_LEXEMA 100
#define MAX_TIPO   30

typedef struct {
    char tipo[MAX_TIPO];
    char lexema[MAX_LEXEMA];
    int linha;
    int coluna;
} Token;

#endif
