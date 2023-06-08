#pragma once

#include "lisp.h"

#include <string.h>
#include <ctype.h>

#define MAX_STR 2000


struct data {
    int line;
    char file[MAX_STR];
};

typedef struct data data;



/*  read file and store; works for seperate each line with ' ' and concat with'+' */
bool read_file(char* argv);
void fill_space(data* a);
void delete_space(data* a);

/* parser */
bool parser(data* a);
bool prog(data* a);
bool instrcts(char cur[]);
bool instrct(char cur[]);
bool func(char function[]); // parse_func
bool ret_func(char func[], char list[]);
bool list_func(char func[], char list[]);
bool is_list(char list[]);
bool var(char list[]);
bool literal(char list[]);
bool io_func(char func[], char line[]);
bool is_set(char func[], char line[]);
bool is_print(char func[], char line[]);
bool is_string(char str[]);
bool int_func(char func[], char line[]);
bool bool_func(char func[], char line[]);
bool if_condition(char function[]);
bool check_trim_instrcts(char prog[],int cnt);
bool loop(char function[]);

/* interp */
void interpStart(data* a);
void interpProg(char* line, lisp* n_list[]);
void interpInstrcts(char cur[],lisp* n_list[]);
void interpInstrct(char cur[], lisp* n_list[]);
void interpFunc(lisp* n_l[], char function[]);
lisp* interpRetFunc(char list[],lisp* n_list[]);
void interpIOfunc(char func[],char function[],lisp* n_list[]);
void interpSet(char* var,char list[], lisp* n_list[]);
void interpPrint(char list[],lisp* n_list[]);
lisp* interpListFunc(char func[],char list[],lisp* n_l[]);
lisp* interpList(char list[],lisp* n_l[]);
int interpIntFunc(char func[],char list[],lisp* n_list[]);
bool interpBoolFunc(char func[], char list[], lisp* n_list[]);
void interpIF(char list[],lisp* n_list[]);
void interpLoop(char list[],lisp* n_list[]);


/* helper function */
int copy_str_by_bracket(char* cur, char* ins);
bool if_ret(char func[]);
void split_list(char* left, char* right, char list[]);
bool trim_brackets(char* function);
void substring(char* function, int start, int end);
void token_get(char* cur, char* token, char* rest);
int get_number(char num[]);

// test
void test(void);
