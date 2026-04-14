# Analisador Léxico - MicroPascal

Projeto Prático de Analisador Léxico para a linguagem MicroPascal (µ-Pascal), desenvolvido para a disciplina de Linguagens Formais, Autômatos e Compiladores.

## Estrutura do Projeto

- `/src` : Código-fonte em C (Scanner, Tabela de Símbolos, Principal).
- `/testes` : Arquivos fonte `.pas` para serem submetidos ao compilador.
- `/relatorio` : Diagramas do AFD (Autômato Finito Determinístico) e o relatório em texto.
- `/output` : Pasta destinada a receber os outputs de execução (`.lex`, `.ts`, `.err`).

## Como Compilar

No terminal, na raiz do projeto, execute o GCC com os arquivos que estão dentro do `/src`:

```bash
gcc -o lexico.exe src/main.c src/scanner.c src/ts.c
```

## Como Executar

Passe o caminho do arquivo de teste desejado que se encontra dentro da pasta `/testes`:

```bash
.\lexico.exe testes\teste_correto1.pas
```

Os resultados de saída aparecerão na mesma pasta do executável (ou caso tenha direcionado para `/output`).
