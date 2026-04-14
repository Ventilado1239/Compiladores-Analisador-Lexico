PROGRAM CaseTest;
VAR
    Valor, TOTAL : INTEGER;
    Media : REAL;
BEGIN
    Valor := 100;
    TOTAL := 200;
    Media := 3.14;
    IF Valor > TOTAL THEN
        Valor := Valor - 1
    ELSE
        TOTAL := TOTAL + 1;
    WHILE Media <= 99.99 DO
    BEGIN
        Media := Media * 1.01;
        Valor := Valor + 1
    END
END.
