# Build-a-programming-language-translator-of-Lisp

The programming language LISP, developed in 1958, is one of the oldest languages still in common use. The langauage is famous for: being fully parenthesised (that is, every instruction is inside its own brackets), having a prefix notation (e.g. functions are written (PLUS 1 2) and not (1 PLUS 2)) and its efficent linked-list Car/Cdr structure for (de-)composing lists.
Here, I develop a very simple langauge inspired by these concepts called NUCLEI (Neill’s UnCommon Lisp Expression Interpreter) and a means to parse or interpret the instructions.

Examples
Parsing :
  (
        (SET A ’1’)
        (PRINT A)
  )
leads to the output :
Parsed OK
or with the interpreter :
1

The CONS instruction is used to construct lists : (
          (PRINT (CONS ’1’ (CONS ’2’ NIL)))
      )
Parsed OK
and when interpreted :
(1 2)