/*
 * scanner.c
 * Implementacao do analisador lexico (scanner) para a linguagem MicroPascal.
 *
 * O scanner e implementado como um Automato Finito Deterministico (AFD)
 * com os seguintes estados:
 *
 *   q0             - Estado inicial: consome espacos, tabulacoes, quebras
 *                    de linha e comentarios. Despacha para o estado
 *                    apropriado com base no primeiro caractere significativo.
 *
 *   q1             - Leitura de identificador/palavra reservada:
 *                    consome letras e digitos (isalnum).
 *   q2             - Aceitacao de identificador/palavra reservada:
 *                    consulta a TS para classificar como KW ou ID.
 *
 *   q3             - Leitura de numero inteiro: consome digitos.
 *   q4             - Aceitacao de NUM_INT.
 *   q_real_ponto   - Ponto decimal lido: exige pelo menos 1 digito.
 *   q_real_aceita  - Aceitacao de NUM_REAL.
 *
 *   q5             - Leitura de ':': verifica se proximo e '=' (OP_ASS)
 *                    ou se e apenas SMB_COL.
 *   q6             - Aceitacao de OP_ASS (:=).
 *   q7             - Aceitacao de SMB_COL (:).
 *
 *   q8             - Aceitacao de SMB_SEM (;).
 *
 *   q_maior        - Leitura de '>': verifica '=' para OP_GE ou OP_GT.
 *   q_maior_igual  - Aceitacao de OP_GE (>=).
 *   q_maior_aceita - Aceitacao de OP_GT (>).
 *
 *   q_menor        - Leitura de '<': verifica '=' (OP_LE), '>' (OP_NE)
 *                    ou apenas OP_LT.
 *   q_menor_igual  - Aceitacao de OP_LE (<=).
 *   q_menor_difr   - Aceitacao de OP_NE (<>).
 *   q_menor_aceita - Aceitacao de OP_LT (<).
 *
 *   q9             - Fim de arquivo (EOF).
 *   q_err          - Estado de erro lexico.
 *
 * Funcoes implementadas:
 *   - initScanner:             configura o arquivo-fonte
 *   - proximoToken:            retorna o proximo token (AFD principal)
 *   - classificaIdentificador: consulta a TS para classificar um lexema
 */

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "scanner.h"
#include "ts.h"

/* Variaveis globais do scanner */
static FILE *arquivoFonte = NULL;
static int linha = 1;
static int coluna = 0;

/* Estados do AFD */
typedef enum {
    q0,              /* Estado inicial */
    q1,              /* Lendo identificador/palavra reservada */
    q2,              /* Aceitacao de ID ou KW */
    q3,              /* Lendo numero inteiro */
    q4,              /* Aceitacao de NUM_INT */
    q5,              /* Leu ':' */
    q6,              /* Aceitacao de OP_ASS (:=) */
    q7,              /* Aceitacao de SMB_COL (:) */
    q8,              /* Aceitacao de SMB_SEM (;) */
    q9,              /* Fim de arquivo */
    q_err,           /* Erro lexico */
    q_maior,         /* Leu '>' */
    q_maior_igual,   /* Aceitacao de OP_GE (>=) */
    q_maior_aceita,  /* Aceitacao de OP_GT (>) */
    q_menor,         /* Leu '<' */
    q_menor_igual,   /* Aceitacao de OP_LE (<=) */
    q_menor_difr,    /* Aceitacao de OP_NE (<>) */
    q_menor_aceita,  /* Aceitacao de OP_LT (<) */
    q_real_ponto,    /* Leu digitos e '.', aguardando parte decimal */
    q_real_aceita    /* Aceitacao de NUM_REAL */
} Estado;

/*
 * initScanner
 * Inicializa o scanner com o arquivo-fonte.
 * Deve ser chamado antes de qualquer chamada a proximoToken().
 */
void initScanner(FILE *fonte) {
    arquivoFonte = fonte;
    linha = 1;
    coluna = 0;
}

/*
 * lerCaractere
 * Le o proximo caractere do arquivo-fonte e atualiza a coluna.
 */
static int lerCaractere(void) {
    int c = fgetc(arquivoFonte);
    if (c != EOF) {
        coluna++;
    }
    return c;
}

/*
 * devolverCaractere
 * Devolve um caractere ao fluxo de entrada e decrementa a coluna.
 * Equivalente ao ungetc mas mantendo a contagem de coluna sincronizada.
 */
static void devolverCaractere(int c) {
    if (c != EOF) {
        ungetc(c, arquivoFonte);
        coluna--;
    }
}

/*
 * classificaIdentificador
 * Consulta a Tabela de Simbolos para determinar se o lexema e uma
 * palavra reservada ou um identificador comum.
 *
 * Se o lexema ja estiver na TS (como palavra reservada), copia o tipo
 * correspondente (ex: KW_PROGRAM). Caso contrario, insere o lexema
 * como ID e retorna "ID".
 */
static void classificaIdentificador(const char *lexema, char *tipo) {
    int idx = buscarTS(lexema);

    if (idx != -1) {
        /* Ja existe na TS (palavra reservada ou ID previamente inserido) */
        strcpy(tipo, obterTipoTS(idx));
    } else {
        /* Novo identificador: inserir na TS */
        inserirTS(lexema, "ID");
        strcpy(tipo, "ID");
    }
}

/*
 * proximoToken
 * Funcao principal do analisador lexico.
 *
 * Implementa o AFD que le caracteres do arquivo-fonte e retorna o
 * proximo token reconhecido. Espacos, tabulacoes, quebras de linha
 * e comentarios {..} sao ignorados.
 *
 * Quando o arquivo termina, retorna um token com tipo[0] == '\0'.
 */
Token proximoToken(void) {
    Token token;
    Estado estado = q0;
    int c;
    char buffer[MAX_LEXEMA];
    int buf_idx = 0;
    int tokenLinha = 0;
    int tokenColuna = 0;

    /* Limpa o token */
    memset(&token, 0, sizeof(Token));

    while (1) {
        switch (estado) {

            /* ============================================================
             * q0 - Estado Inicial
             * Ignora espacos, tabs, \r, \n e comentarios.
             * Despacha para o estado correto conforme o caractere lido.
             * ============================================================ */
            case q0:
                c = lerCaractere();

                /* Espacos e tabulacoes: ignora */
                if (c == ' ' || c == '\t' || c == '\r') {
                    estado = q0;
                }
                /* Quebra de linha: incrementa linha, zera coluna */
                else if (c == '\n') {
                    linha++;
                    coluna = 0;
                    estado = q0;
                }
                /* Comentario: consome tudo ate '}' ou EOF */
                else if (c == '{') {
                    int linhaInicioComment = linha;
                    int colunaInicioComment = coluna;
                    /* Consome caracteres ate encontrar '}' ou EOF */
                    while (1) {
                        c = lerCaractere();
                        if (c == '}') {
                            break; /* Comentario fechado corretamente */
                        }
                        if (c == EOF) {
                            /* Erro: comentario nao fechado */
                            strcpy(token.tipo, "ERR_CMT_NAO_FECHADO");
                            strcpy(token.lexema, "{");
                            token.linha = linhaInicioComment;
                            token.coluna = colunaInicioComment;
                            return token;
                        }
                        if (c == '\n') {
                            linha++;
                            coluna = 0;
                        }
                    }
                    estado = q0; /* Comentario consumido, voltar a q0 */
                }
                /* Fim de arquivo */
                else if (c == EOF) {
                    estado = q9;
                }
                /* Letra: inicio de identificador ou palavra reservada */
                else if (isalpha(c)) {
                    buf_idx = 0;
                    tokenLinha = linha;
                    tokenColuna = coluna;
                    buffer[buf_idx++] = tolower(c);
                    estado = q1;
                }
                /* Digito: inicio de numero inteiro (ou real) */
                else if (isdigit(c)) {
                    buf_idx = 0;
                    tokenLinha = linha;
                    tokenColuna = coluna;
                    buffer[buf_idx++] = c;
                    estado = q3;
                }
                /* Dois-pontos: pode ser ':' ou ':=' */
                else if (c == ':') {
                    tokenLinha = linha;
                    tokenColuna = coluna;
                    estado = q5;
                }
                /* Ponto-e-virgula */
                else if (c == ';') {
                    tokenLinha = linha;
                    tokenColuna = coluna;
                    estado = q8;
                }
                /* Maior: pode ser '>' ou '>=' */
                else if (c == '>') {
                    tokenLinha = linha;
                    tokenColuna = coluna;
                    estado = q_maior;
                }
                /* Menor: pode ser '<', '<=' ou '<>' */
                else if (c == '<') {
                    tokenLinha = linha;
                    tokenColuna = coluna;
                    estado = q_menor;
                }
                /* Simbolos e operadores de caractere unico */
                else {
                    tokenLinha = linha;
                    tokenColuna = coluna;
                    buffer[0] = c;
                    buffer[1] = '\0';
                    token.linha = tokenLinha;
                    token.coluna = tokenColuna;
                    strcpy(token.lexema, buffer);

                    switch (c) {
                        case '+':
                            strcpy(token.tipo, "OP_AD");
                            return token;
                        case '-':
                            strcpy(token.tipo, "OP_MIN");
                            return token;
                        case '*':
                            strcpy(token.tipo, "OP_MUL");
                            return token;
                        case '/':
                            strcpy(token.tipo, "OP_DIV");
                            return token;
                        case '=':
                            strcpy(token.tipo, "OP_EQ");
                            return token;
                        case '.':
                            strcpy(token.tipo, "SMB_DOT");
                            return token;
                        case ',':
                            strcpy(token.tipo, "SMB_COM");
                            return token;
                        case '(':
                            strcpy(token.tipo, "SMB_OPA");
                            return token;
                        case ')':
                            strcpy(token.tipo, "SMB_CPA");
                            return token;
                        default:
                            /* Caractere invalido */
                            strcpy(token.tipo, "ERR_CHAR_INVALIDO");
                            return token;
                    }
                }
                break;

            /* ============================================================
             * q1 - Lendo identificador/palavra reservada
             * Consome letras e digitos (isalnum).
             * ============================================================ */
            case q1:
                c = lerCaractere();
                if (isalnum(c) && buf_idx < MAX_LEXEMA - 1) {
                    buffer[buf_idx++] = tolower(c);
                    estado = q1;
                } else {
                    /* Caractere nao alfanumerico: transicao para q2 */
                    estado = q2;
                }
                break;

            /* ============================================================
             * q2 - Aceitacao de identificador ou palavra reservada
             * Devolve o ultimo caractere lido (lookahead), classifica
             * o lexema consultando a TS e retorna o token.
             * ============================================================ */
            case q2:
                buffer[buf_idx] = '\0';
                devolverCaractere(c);

                classificaIdentificador(buffer, token.tipo);
                strcpy(token.lexema, buffer);
                token.linha = tokenLinha;
                token.coluna = tokenColuna;
                return token;

            /* ============================================================
             * q3 - Lendo numero inteiro
             * Consome digitos. Se encontrar '.', vai para q_real_ponto.
             * ============================================================ */
            case q3:
                c = lerCaractere();
                if (isdigit(c) && buf_idx < MAX_LEXEMA - 1) {
                    buffer[buf_idx++] = c;
                    estado = q3;
                } else if (c == '.' && buf_idx < MAX_LEXEMA - 1) {
                    /* Verificar se o proximo char e um digito para
                       distinguir "10." (NUM_INT + SMB_DOT) de "10.5" (NUM_REAL) */
                    int peek = fgetc(arquivoFonte);
                    if (peek != EOF && isdigit(peek)) {
                        /* E um numero real: 10.5 */
                        buffer[buf_idx++] = '.';
                        buffer[buf_idx++] = peek;
                        coluna += 2; /* o '.' e o digito */
                        estado = q_real_aceita;
                    } else {
                        /* Nao e real: devolver peek e o '.',
                           aceitar como NUM_INT, o '.' sera lido depois */
                        if (peek != EOF) {
                            ungetc(peek, arquivoFonte);
                        }
                        devolverCaractere('.'); /* devolver o ponto */
                        estado = q4;
                    }
                } else {
                    estado = q4;
                }
                break;

            /* ============================================================
             * q4 - Aceitacao de NUM_INT
             * ============================================================ */
            case q4:
                buffer[buf_idx] = '\0';
                devolverCaractere(c);

                strcpy(token.tipo, "NUM_INT");
                strcpy(token.lexema, buffer);
                token.linha = tokenLinha;
                token.coluna = tokenColuna;
                return token;

            /* ============================================================
             * q_real_ponto - Ponto decimal lido, lendo parte decimal
             * NOTA: este estado agora so e alcancado via caminhos alternativos.
             * O caminho principal para numeros reais vai direto de q3 para
             * q_real_aceita apos verificar que ha digito apos o ponto.
             * ============================================================ */
            case q_real_ponto:
                c = lerCaractere();
                if (isdigit(c) && buf_idx < MAX_LEXEMA - 1) {
                    buffer[buf_idx++] = c;
                    estado = q_real_aceita;
                } else {
                    /* Numero real malformado (ex: "10." sem digitos depois) */
                    buffer[buf_idx] = '\0';
                    devolverCaractere(c);
                    strcpy(token.tipo, "ERR_REAL_INVALIDO");
                    strcpy(token.lexema, buffer);
                    token.linha = tokenLinha;
                    token.coluna = tokenColuna;
                    return token;
                }
                break;

            /* ============================================================
             * q_real_aceita - Lendo digitos da parte decimal
             * ============================================================ */
            case q_real_aceita:
                c = lerCaractere();
                if (isdigit(c) && buf_idx < MAX_LEXEMA - 1) {
                    buffer[buf_idx++] = c;
                    estado = q_real_aceita;
                } else {
                    buffer[buf_idx] = '\0';
                    devolverCaractere(c);
                    strcpy(token.tipo, "NUM_REAL");
                    strcpy(token.lexema, buffer);
                    token.linha = tokenLinha;
                    token.coluna = tokenColuna;
                    return token;
                }
                break;

            /* ============================================================
             * q5 - Leu ':' — verifica se e ':=' (OP_ASS) ou ':' (SMB_COL)
             * ============================================================ */
            case q5:
                c = lerCaractere();
                if (c == '=') {
                    estado = q6;
                } else {
                    estado = q7;
                }
                break;

            /* ============================================================
             * q6 - Aceitacao de OP_ASS (:=)
             * ============================================================ */
            case q6:
                strcpy(token.tipo, "OP_ASS");
                strcpy(token.lexema, ":=");
                token.linha = tokenLinha;
                token.coluna = tokenColuna;
                return token;

            /* ============================================================
             * q7 - Aceitacao de SMB_COL (:)
             * ============================================================ */
            case q7:
                devolverCaractere(c);
                strcpy(token.tipo, "SMB_COL");
                strcpy(token.lexema, ":");
                token.linha = tokenLinha;
                token.coluna = tokenColuna;
                return token;

            /* ============================================================
             * q8 - Aceitacao de SMB_SEM (;)
             * ============================================================ */
            case q8:
                strcpy(token.tipo, "SMB_SEM");
                strcpy(token.lexema, ";");
                token.linha = tokenLinha;
                token.coluna = tokenColuna;
                return token;

            /* ============================================================
             * q_maior - Leu '>' — verifica se e '>=' (OP_GE) ou '>' (OP_GT)
             * ============================================================ */
            case q_maior:
                c = lerCaractere();
                if (c == '=') {
                    estado = q_maior_igual;
                } else {
                    estado = q_maior_aceita;
                }
                break;

            /* ============================================================
             * q_maior_igual - Aceitacao de OP_GE (>=)
             * ============================================================ */
            case q_maior_igual:
                strcpy(token.tipo, "OP_GE");
                strcpy(token.lexema, ">=");
                token.linha = tokenLinha;
                token.coluna = tokenColuna;
                return token;

            /* ============================================================
             * q_maior_aceita - Aceitacao de OP_GT (>)
             * ============================================================ */
            case q_maior_aceita:
                devolverCaractere(c);
                strcpy(token.tipo, "OP_GT");
                strcpy(token.lexema, ">");
                token.linha = tokenLinha;
                token.coluna = tokenColuna;
                return token;

            /* ============================================================
             * q_menor - Leu '<' — verifica '=' (OP_LE), '>' (OP_NE)
             *           ou apenas '<' (OP_LT)
             * ============================================================ */
            case q_menor:
                c = lerCaractere();
                if (c == '=') {
                    estado = q_menor_igual;
                } else if (c == '>') {
                    estado = q_menor_difr;
                } else {
                    estado = q_menor_aceita;
                }
                break;

            /* ============================================================
             * q_menor_igual - Aceitacao de OP_LE (<=)
             * ============================================================ */
            case q_menor_igual:
                strcpy(token.tipo, "OP_LE");
                strcpy(token.lexema, "<=");
                token.linha = tokenLinha;
                token.coluna = tokenColuna;
                return token;

            /* ============================================================
             * q_menor_difr - Aceitacao de OP_NE (<>)
             * ============================================================ */
            case q_menor_difr:
                strcpy(token.tipo, "OP_NE");
                strcpy(token.lexema, "<>");
                token.linha = tokenLinha;
                token.coluna = tokenColuna;
                return token;

            /* ============================================================
             * q_menor_aceita - Aceitacao de OP_LT (<)
             * ============================================================ */
            case q_menor_aceita:
                devolverCaractere(c);
                strcpy(token.tipo, "OP_LT");
                strcpy(token.lexema, "<");
                token.linha = tokenLinha;
                token.coluna = tokenColuna;
                return token;

            /* ============================================================
             * q9 - Fim de arquivo
             * Retorna token com tipo vazio para sinalizar EOF.
             * ============================================================ */
            case q9:
                token.tipo[0] = '\0';
                token.lexema[0] = '\0';
                token.linha = linha;
                token.coluna = coluna;
                return token;

            /* ============================================================
             * q_err - Estado de erro lexico
             * ============================================================ */
            case q_err:
                token.linha = tokenLinha;
                token.coluna = tokenColuna;
                return token;
        }
    }
}