#include "lisp.h" 
#include <string.h>
#include <assert.h>
#include <ctype.h>

#define FORMATSTR "%i"
#define LISPIMPL "Linked"

struct lisp{
    atomtype a; 
    struct lisp* car;
    struct lisp* cdr;
};
