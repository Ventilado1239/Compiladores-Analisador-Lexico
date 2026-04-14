{ Este programa testa comentarios, todos os operadores e numeros reais }
program Operadores;
var
    a, b : integer;
    resultado : real;
begin
    a := 50;
    b := 25;
    resultado := a + b - 10 * 2.0 / 4.5;
    if a = b then
        a := 0;
    if a <> b then
        b := 0;
    if a < b then
        a := a + 1;
    if a > b then
        b := b - 1;
    if a <= b then
        a := a * 2;
    if a >= b then
        b := b / 3
end.
