

### &#x20;   RELATÓRIO TÉCNICO — ANALISADOR LÉXICO MICROPASCAL (µ-Pascal)



&#x20;   Universidade Católica de Brasília

&#x20;   Linguagens Formais, Autômatos e Compiladores — 1º semestre de 2026

&#x20;   Professor: MARCELO EUSTÁQUIO

&#x20;   Alunos: Abner Rafael, Felipe Araújo, Gabriel de Souza, Gustavo Alves, Tarso Hebert



##### 1\. INTRODUÇÃO



Este relatório descreve a implementação de um analisador léxico para a linguagem

MicroPascal (µ-Pascal). O analisador foi implementado em linguagem C, sem o uso

de ferramentas de geração automática, utilizando um Autômato Finito

Determinístico (AFD) como base para o reconhecimento de tokens.



O analisador léxico é capaz de:

&#x20; - Reconhecer todos os tokens da linguagem MicroPascal

&#x20; - Identificar e reportar erros léxicos com linha e coluna

&#x20; - Manter uma Tabela de Símbolos (TS) com palavras reservadas e identificadores

&#x20; - Gerar arquivos de saída .lex, .ts e .err



##### 2\. ESTRUTURAS DE DADOS





###### 2.1. Token (token.h)

\---------------------

Estrutura que representa um token reconhecido pelo analisador:



&#x20;   typedef struct {

&#x20;       char tipo\[30];     // Nome do token (ex: KW\_PROGRAM, ID, OP\_ASS)

&#x20;       char lexema\[100];  // Sequência de caracteres reconhecida

&#x20;       int linha;         // Linha onde o token foi encontrado

&#x20;       int coluna;        // Coluna onde o token inicia

&#x20;   } Token;



###### 2.2. EntradaTS (ts.h)

\---------------------

Estrutura que representa uma entrada na Tabela de Símbolos:



&#x20;   typedef struct {

&#x20;       char lexema\[100];  // Lexema armazenado (em minúsculo)

&#x20;       char tipo\[30];     // Tipo do token (ex: KW\_PROGRAM, ID)

&#x20;   } EntradaTS;



A Tabela de Símbolos é implementada como um vetor estático de EntradaTS com

capacidade máxima de 1000 entradas (MAX\_TS). A busca é linear e

case-insensitive.



###### 2.3. ErroLexico (main.c)

\-------------------------

Estrutura para armazenamento temporário de erros léxicos:



&#x20;   typedef struct {

&#x20;       char tipo\[30];     // Tipo do erro (ex: ERR\_CHAR\_INVALIDO)

&#x20;       char lexema\[100];  // Caractere ou lexema que causou o erro

&#x20;       int linha;         // Linha do erro

&#x20;       int coluna;        // Coluna do erro

&#x20;   } ErroLexico;



###### 2.4. Estado (scanner.c)

\-----------------------

Enumeração dos estados do AFD:



&#x20;   typedef enum {

&#x20;       q0,              // Estado inicial

&#x20;       q1,              // Lendo identificador/palavra reservada

&#x20;       q2,              // Aceitação de ID ou KW

&#x20;       q3,              // Lendo número inteiro

&#x20;       q4,              // Aceitação de NUM\_INT

&#x20;       q5,              // Leu ':'

&#x20;       q6,              // Aceitação de OP\_ASS (:=)

&#x20;       q7,              // Aceitação de SMB\_COL (:)

&#x20;       q8,              // Aceitação de SMB\_SEM (;)

&#x20;       q9,              // Fim de arquivo

&#x20;       q\_err,           // Erro léxico

&#x20;       q\_maior,         // Leu '>'

&#x20;       q\_maior\_igual,   // Aceitação de OP\_GE (>=)

&#x20;       q\_maior\_aceita,  // Aceitação de OP\_GT (>)

&#x20;       q\_menor,         // Leu '<'

&#x20;       q\_menor\_igual,   // Aceitação de OP\_LE (<=)

&#x20;       q\_menor\_difr,    // Aceitação de OP\_NE (<>)

&#x20;       q\_menor\_aceita,  // Aceitação de OP\_LT (<)

&#x20;       q\_real\_ponto,    // Ponto decimal lido

&#x20;       q\_real\_aceita    // Aceitação de NUM\_REAL

&#x20;   } Estado;





##### **3. DESCRIÇÃO DAS FUNÇÕES IMPLEMENTADAS**





###### 3.1. Tabela de Símbolos (ts.c)

\-------------------------------



&#x20; inicializarTS()

&#x20;   Pré-carrega as 11 palavras reservadas da linguagem (program, var, integer,

&#x20;   real, begin, end, if, then, else, while, do) na tabela com seus respectivos

&#x20;   tokens KW\_\*.



&#x20; buscarTS(lexema)

&#x20;   Busca um lexema na tabela usando comparação case-insensitive. Retorna o

&#x20;   índice da entrada encontrada ou -1 se não existir. Utiliza função auxiliar

&#x20;   strcasecmp\_local() para portabilidade.



&#x20; inserirTS(lexema, tipo)

&#x20;   Insere um novo lexema na tabela somente se ainda não existir (evita

&#x20;   duplicação). Retorna o índice da entrada.



&#x20; obterTipoTS(indice)

&#x20;   Retorna o tipo de token de uma entrada dado seu índice na tabela.



&#x20; imprimirTS()

&#x20;   Imprime a tabela completa no console com formatação tabular.



&#x20; gravarTS(arq)

&#x20;   Grava a tabela completa em um arquivo .ts com formatação tabular.



##### 3.2. Scanner (scanner.c)

\-------------------------



&#x20; initScanner(fonte)

&#x20;   Inicializa o scanner com o ponteiro do arquivo-fonte. Reseta contadores

&#x20;   de linha e coluna.



&#x20; lerCaractere()

&#x20;   Função auxiliar que lê o próximo caractere do arquivo e incrementa a

&#x20;   coluna. Encapsula fgetc().



&#x20; devolverCaractere(c)

&#x20;   Função auxiliar que devolve um caractere ao fluxo (ungetc) e decrementa

&#x20;   a coluna. Usada para o mecanismo de lookahead do AFD.



&#x20; classificaIdentificador(lexema, tipo)

&#x20;   Consulta a Tabela de Símbolos para determinar se o lexema é uma palavra

&#x20;   reservada (já pré-carregada) ou um identificador novo. Se for novo,

&#x20;   insere automaticamente na TS como "ID".



&#x20; proximoToken()

&#x20;   Função principal do analisador léxico. Implementa o AFD completo com

&#x20;   switch/case sobre os estados. A cada chamada, retorna o próximo token

&#x20;   reconhecido. Quando o arquivo termina, retorna um token com tipo vazio.



###### 3.3. Programa Principal (main.c)

\---------------------------------



&#x20; extrairNomeBase(caminho, base)

&#x20;   Extrai o nome do arquivo sem extensão para gerar os nomes dos arquivos

&#x20;   de saída (.lex, .ts, .err).



&#x20; ehErro(tipo)

&#x20;   Verifica se um tipo de token é um erro léxico (prefixo "ERR\_").



&#x20; main(argc, argv)

&#x20;   Função principal que:

&#x20;   1. Recebe o nome do arquivo-fonte como argumento (ou usa "teste.pas")

&#x20;   2. Inicializa a TS com palavras reservadas

&#x20;   3. Inicializa o scanner com o arquivo-fonte

&#x20;   4. Loop: chama proximoToken() até EOF

&#x20;      - Tokens válidos são escritos no .lex

&#x20;      - Erros são armazenados e exibidos no console

&#x20;   5. Grava a TS no .ts

&#x20;   6. Grava os erros no .err

&#x20;   7. Imprime resumo no console





###### 4\. EXPLICAÇÃO DO AFD





O Autômato Finito Determinístico (AFD) implementado possui 20 estados e

reconhece todos os tokens da linguagem MicroPascal.



Estado Inicial (q0):

&#x20; - Ignora espaços em branco (' ', '\\t', '\\r')

&#x20; - Ignora quebras de linha ('\\n'), incrementando o contador de linhas

&#x20; - Ignora comentários delimitados por { e }

&#x20; - Despacha para o estado apropriado conforme o caractere lido



Identificadores e Palavras Reservadas (q0 → q1 → q2):

&#x20; - q0 transita para q1 ao ler uma letra (isalpha)

&#x20; - q1 consome letras e dígitos (isalnum), convertendo para minúsculo

&#x20; - q1 transita para q2 ao ler caractere não-alfanumérico

&#x20; - q2 devolve o último caractere (ungetc) e consulta a TS para classificar



Números Inteiros (q0 → q3 → q4):

&#x20; - q0 transita para q3 ao ler um dígito (isdigit)

&#x20; - q3 consome dígitos consecutivos

&#x20; - q3 transita para q4 ao ler caractere não-dígito (aceita NUM\_INT)



Números Reais (q3 → q\_real\_ponto → q\_real\_aceita):

&#x20; - q3 transita via '.' verificando se há dígito após o ponto

&#x20; - Se há dígito: consome como NUM\_REAL

&#x20; - Se não há dígito: devolve o '.' e aceita como NUM\_INT



Atribuição e Dois-Pontos (q0 → q5 → q6/q7):

&#x20; - q0 transita para q5 ao ler ':'

&#x20; - q5 → q6 se próximo é '=' (OP\_ASS)

&#x20; - q5 → q7 se outro (SMB\_COL, com ungetc)



Operadores Relacionais de Maior (q0 → q\_maior → q\_maior\_igual/q\_maior\_aceita):

&#x20; - q\_maior verifica se próximo é '=' para OP\_GE, senão OP\_GT



Operadores Relacionais de Menor (q0 → q\_menor → q\_menor\_igual/q\_menor\_difr/q\_menor\_aceita):

&#x20; - q\_menor verifica '=' para OP\_LE, '>' para OP\_NE, senão OP\_LT



Símbolos de Caractere Único:

&#x20; - Reconhecidos diretamente em q0: + - \* / = . , ( )

&#x20; - Tratados via switch/case sem mudança de estado intermediária



Erros Léxicos:

&#x20; - Caractere inválido: caractere não pertence à linguagem (ERR\_CHAR\_INVALIDO)

&#x20; - Comentário não fechado: '{' sem '}' até EOF (ERR\_CMT\_NAO\_FECHADO)



O diagrama visual completo do AFD encontra-se no arquivo diagrama\_afd.dot

(formato Graphviz) e diagrama\_afd.md (formato Mermaid).



##### 5\. TESTES REALIZADOS





###### 5.1. Teste Correto 1 (teste\_correto1.pas) — Programa Exemplo da Especificação

\-------------------------------------------------------------------------------



Entrada:

&#x20;   program Exemplo;

&#x20;   var

&#x20;       x, y : integer;

&#x20;       z : real;

&#x20;   begin

&#x20;       x := 10;

&#x20;       y := 20;

&#x20;       z := x + y \* 2.5;

&#x20;       if x > y then

&#x20;           x := x - 1

&#x20;       else

&#x20;           y := y + 1;

&#x20;       while z <= 100 do

&#x20;       begin

&#x20;           z := z \* 1.5;

&#x20;           x := x + 2

&#x20;       end

&#x20;   end.



Resultado: 68 tokens reconhecidos, 0 erros léxicos.



Primeiros tokens da saída (.lex):

&#x20;   <KW\_PROGRAM, program> 1 1

&#x20;   <ID, exemplo> 1 9

&#x20;   <SMB\_SEM, ;> 1 16

&#x20;   <KW\_VAR, var> 2 1

&#x20;   <ID, x> 3 5

&#x20;   <SMB\_COM, ,> 3 6

&#x20;   <ID, y> 3 8

&#x20;   <SMB\_COL, :> 3 10

&#x20;   <KW\_INTEGER, integer> 3 12

&#x20;   ...



Tabela de Símbolos final:

&#x20;   0  program   KW\_PROGRAM

&#x20;   1  var       KW\_VAR

&#x20;   2  integer   KW\_INTEGER

&#x20;   3  real      KW\_REAL

&#x20;   4  begin     KW\_BEGIN

&#x20;   5  end       KW\_END

&#x20;   6  if        KW\_IF

&#x20;   7  then      KW\_THEN

&#x20;   8  else      KW\_ELSE

&#x20;   9  while     KW\_WHILE

&#x20;   10 do        KW\_DO

&#x20;   11 exemplo   ID

&#x20;   12 x         ID

&#x20;   13 y         ID

&#x20;   14 z         ID



###### 5.2. Teste Correto 2 (teste\_correto2.pas) — Todos os Operadores

\----------------------------------------------------------------



Programa com comentário, todos os operadores relacionais (=, <>, <, <=, >, >=)

e aritméticos (+, -, \*, /), além de números reais.



Resultado: 98 tokens reconhecidos, 0 erros léxicos.

###### 

###### 5.3. Teste Correto 3 (teste\_correto3.pas) — Case-Insensitivity

\---------------------------------------------------------------



Programa com palavras reservadas em maiúsculo (PROGRAM, VAR, INTEGER, BEGIN,

etc.) e identificadores em CamelCase (Valor, TOTAL, Media).



Resultado: 64 tokens reconhecidos, 0 erros léxicos.

Todas as palavras reservadas foram corretamente reconhecidas independente

da capitalização. Identificadores foram normalizados para minúsculo.



###### 5.4. Teste com Erro 1 (teste\_erro1.pas) — Caracteres Inválidos

\---------------------------------------------------------------



Entrada:

&#x20;   program Teste;

&#x20;   var

&#x20;       x : integer;

&#x20;   begin

&#x20;       x := 10 @ 2;

&#x20;       x := x $ 5;

&#x20;       x := x % 3

&#x20;   end.



Resultado: 25 tokens reconhecidos, 3 erros léxicos.



Erros detectados:

&#x20;   ERR\_CHAR\_INVALIDO    @    linha 5   coluna 13

&#x20;   ERR\_CHAR\_INVALIDO    $    linha 6   coluna 12

&#x20;   ERR\_CHAR\_INVALIDO    %    linha 7   coluna 12



###### 5.5. Teste com Erro 2 (teste\_erro2.pas) — Comentário Não Fechado

\-----------------------------------------------------------------



Entrada:

&#x20;   program Teste;

&#x20;   var

&#x20;       x : integer;

&#x20;   begin

&#x20;       { este comentario nao fecha

&#x20;       x := 10;

&#x20;   end.



Resultado: 9 tokens reconhecidos, 1 erro léxico.



Erros detectados:

&#x20;   ERR\_CMT\_NAO\_FECHADO  {    linha 5   coluna 5



O analisador detectou que o comentário aberto com '{' na linha 5 nunca

foi fechado com '}' antes do fim do arquivo.



###### 5.6. Teste com Erro 3 (teste\_erro3.pas) — Caracteres Inválidos (#, \&)

\----------------------------------------------------------------------



Entrada:

&#x20;   program Teste;

&#x20;   var

&#x20;       x : integer;

&#x20;   begin

&#x20;       x := 42;

&#x20;       x := x + 1;

&#x20;       x := x # 3;

&#x20;       x := x \& 2

&#x20;   end.



Resultado: 30 tokens reconhecidos, 2 erros léxicos.



Erros detectados:

&#x20;   ERR\_CHAR\_INVALIDO    #    linha 7   coluna 12

&#x20;   ERR\_CHAR\_INVALIDO    \&    linha 8   coluna 12





##### 6\. FORMATO DE SAÍDA





###### 6.1. Arquivo .lex

\-----------------

Formato: <nome\_token, lexema> linha coluna



Exemplo:

&#x20;   <KW\_PROGRAM, program> 1 1

&#x20;   <ID, exemplo> 1 9

&#x20;   <SMB\_SEM, ;> 1 16



###### 6.2. Arquivo .ts

\----------------

Formato tabular com índice, lexema e tipo de token.

###### 

###### 6.3. Arquivo .err

\-----------------

Formato tabular com tipo do erro, lexema, linha e coluna.

Quando não há erros: "Nenhum erro lexico encontrado."





##### 7\. INSTRUÇÕES DE COMPILAÇÃO E EXECUÇÃO





Compilação:

&#x20;   gcc -o lexico.exe main.c scanner.c ts.c



Execução:

&#x20;   lexico.exe <arquivo.pas>         (com argumento)

&#x20;   lexico.exe                       (usa teste.pas como padrão)



O programa gera automaticamente os três arquivos de saída:

&#x20;   <nome>.lex    - tokens reconhecidos

&#x20;   <nome>.ts     - tabela de símbolos

&#x20;   <nome>.err    - erros léxicos





##### 8\. ARQUIVOS DO PROJETO





&#x20;   token.h             - Definição da estrutura Token

&#x20;   ts.h                - Interface da Tabela de Símbolos  

&#x20;   ts.c                - Implementação da Tabela de Símbolos

&#x20;   scanner.h           - Interface do scanner (analisador léxico)

&#x20;   scanner.c           - Implementação do scanner com AFD

&#x20;   main.c              - Programa principal

&#x20;   diagrama\_afd.dot    - Diagrama do AFD (Graphviz)

&#x20;   diagrama\_afd.md     - Diagrama do AFD (Mermaid) com documentação

&#x20;   teste\_correto1.pas  - Teste correto 1 (programa exemplo da especificação)

&#x20;   teste\_correto2.pas  - Teste correto 2 (todos os operadores)

&#x20;   teste\_correto3.pas  - Teste correto 3 (case-insensitivity)

&#x20;   teste\_erro1.pas     - Teste com erro 1 (caracteres inválidos @, $, %)

&#x20;   teste\_erro2.pas     - Teste com erro 2 (comentário não fechado)

&#x20;   teste\_erro3.pas     - Teste com erro 3 (caracteres inválidos #, \&)

&#x20;   Relatório.txt       - Este relatório técnico



================================================================================

