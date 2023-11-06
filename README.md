# Build-a-programming-language-translator-of-Lisp

The programming language LISP, developed in 1958, is one of the oldest languages still in common use. The language is famous for: being fully parenthesized (that is, every instruction is inside its own brackets), having a prefix notation (e.g. functions are written (PLUS 1 2) and not (1 PLUS 2)) and its efficient linked-list Car/Cdr structure for (de-)composing lists.
Here, I develop a very simple language inspired by these concepts and a means to parse or interpret the instructions.


### Examples
Parsing :
```bash
(
  (SET A ’1’)
  (PRINT A)
)
```
leads to the output :
```bash
Parsed OK
```
or with the interpreter :
```bash
1
```

The CONS instruction is used to construct lists : 
```bash
(
  (PRINT (CONS ’1’ (CONS ’2’ NIL)))
)
```
Output :
```bash
Parsed OK
(1 2)
```

The CAR instruction is used to deconstruct lists : 
```bash
(
  (SET A ’(5 (1 2 3))’)
  (PRINT (CAR A))
)
```
```bash
Parsed OK
and when interpreted :
5
```


