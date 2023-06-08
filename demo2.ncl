(
    (PRINT (CONS '1' (CONS '2' NIL)))
    (SET A '(5 1 2)')
    (PRINT A)
    (SET B (CDR A))
    (PRINT B)
    (SET C (CONS A B))
    (PRINT C)
)
