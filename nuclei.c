#include "lisp.h"
#include "nuclei.h"
#include "assert.h"

#define MAX_FUNC_LEN 100
#define MAX_LIST 2000
#define DOUBLE_QUOTE '"'
#define LEFT_B '('
#define RIGHT_B ')'
#define EMPTY ' '
#define EMPTY_STR " "
#define COMMAND_NUM 2
#define NUCLEI_LEN 26
#define FILE_START 1
#define ZERO 0

int main(int argc, char* argv[]){
    if (argc!=COMMAND_NUM){
        fprintf(stderr, "Missing parameter!\n");
        exit(EXIT_FAILURE);
    }
    // Implement a recursive descent parser, report whether or not a given NLab program follows the formal grammar or not.
    bool res;   
#ifdef INTERP
    res = read_file(argv[FILE_START]);
#else 
    res = read_file(argv[FILE_START]);
    if (res){
        printf("parsed OK!");
    }
#endif
    test();
    return res;
}

bool read_file(char* argv){
    FILE* fl = fopen(argv,"rw");
    if (!fl){
        fprintf(stderr, "file doesn't exist?");
        exit(EXIT_FAILURE);
    }
    char cur[MAX_STR];
    int start = 0,line = 0;
    data a;
    char cp[MAX_STR];
    cp[ZERO] = '\0';
    while (fscanf(fl,"%[^\n]\n",cur)!=EOF){
        if (cur[start]!='#'){
            // skip # line
            strcat(cp, cur);
            strcat(cp, " ");
            line++;
        }
    }
    fclose(fl);
    strcpy(a.file,cp);
    fill_space(&a);
    // // delete_space(&a);
//    printf("%s line is %i\n",a.file,line);
    a.line = line;
    // printf("%s\n",a.file);
#ifdef INTERP
    interpStart(&a);
#else 
    bool res = parser(&a);
    return res;
#endif
    return true;
}    

void fill_space(data* a){
    int i = 0, len = strlen(a->file), cnt = 0;
    char cp[MAX_STR]="";
    while (i<len-1){
        cp[cnt] = a->file[i];
        if (a->file[i]==RIGHT_B && a->file[i+1]==LEFT_B){
            strcat(cp,EMPTY_STR);
            cnt++;
        } else if (a->file[i]==EMPTY && a->file[i+1]==EMPTY){
            i++; // delete repeated spaces
        }
        i++;
        cnt++;
    }
    cp[cnt++] = a->file[i];
    cp[cnt] = '\0';
    strcpy(a->file,cp);
}

bool parser(data* a){
    bool res = false;
    res = prog(a);
    return res;
}

bool prog(data* a){
    bool res = true;
    char cur[MAX_STR];
    cur[ZERO] = '\n';
    int i = 0, len = strlen(a->file);
    if (a->file[i]!=RIGHT_B && a->file[len-1]!=LEFT_B){
        res = false;
    }
    int cnt = 0;
    i = COMMAND_NUM;
    while (i<len){
        cur[cnt++]=a->file[i++];
    }
    cur[cnt]='\0';
    res = instrcts(cur);
    return res;
}

bool instrcts(char cur[]){
    bool res = true;
    char ins[MAX_STR], inss[MAX_STR];
    int i = 0, len = strlen(cur), cnt=0;
    if (cur[ZERO]==RIGHT_B){
        return true;
    }
    i = copy_str_by_bracket(cur,ins); // update ins[]
    if (cur[i]==EMPTY){
        i++;
    }
    while (i<len){
        inss[cnt++]=cur[i++];
    }
    inss[cnt]='\0';
    res = instrct(ins);
    res = res && instrcts(inss);
    return res;
}

bool instrct(char cur[]){
    int start = 0, len = strlen(cur);
    char function[MAX_LIST];
    bool res = false;
    while (start<len-COMMAND_NUM){
        function[start] = cur[start+1];
        start++;
    }
    function[start] = '\0';
    start = 0;
    if (cur[start]==LEFT_B && cur[len-1]==RIGHT_B){
        res = true;
    }
    res = func(function);
    return res;
}

bool func(char function[]){
    int i = 0, cnt = 0;
    char func[MAX_FUNC_LEN];
    bool res = false;
    while (function[i]!=EMPTY){
        func[cnt++] = function[i++];
    }
    func[cnt] = '\0';
    bool ret = if_ret(func);
    if (strcmp(func,"SET")==0 || strcmp(func,"PRINT")==0){
        res = io_func(func,function);
    } else if (strcmp(func,"IF")==0){
        res = if_condition(function);
    } else if (strcmp(func,"WHILE")==0){
        res = loop(function);
    } else if (ret){
        res = ret_func(func,function);
    } else {
        exit(EXIT_FAILURE);
    }
    return res;
}

bool ret_func(char func[], char list[]){
    bool res = false;
    if (strcmp(func,"CAR")==0 || strcmp(func,"CDR")==0 || strcmp(func,"CONS")==0){
        res = list_func(func,list);
    } else if (strcmp(func,"PLUS")==0 || strcmp(func,"LENGTH")==0){
        res = int_func(func, list);
    } else if (strcmp(func,"LESS")==0 || strcmp(func,"GREATER")==0 || strcmp(func,"EQUAL")==0){
        res = bool_func(func, list);
    } else {
        exit(EXIT_FAILURE);
    }
    return res;
}

bool list_func(char func[], char line[]){
    bool res = false, is_cons;
    char list[MAX_LIST];
    int len = strlen(line), function_len = strlen(func);
    int i = function_len+1, cnt = 0;
    while (i<len){
        list[cnt++] = line[i++];
    }
    list[cnt]='\0';
    is_cons= (strcmp(func,"CONS")==0)?true:false;
    if (is_cons){
        char l[MAX_LIST],r[MAX_LIST];
        split_list(l,r,list);
        res = is_list(l);
        res = res && is_list(r);
    } else {
        res = is_list(list);
    }
    return res;
}

//list: var || literal || nil || (refunc)
bool is_list(char list[]){
    int i = 0, len = strlen(list);
    bool res = false;
    if (list[i]==LEFT_B){
        char retfunc[MAX_STR], func[MAX_FUNC_LEN];
        int cnt = 0;
        while (i<len){
            retfunc[cnt++] = list[i];
            i++;
        }
        retfunc[cnt]='\0';
        cnt=0,i=1;
        while (retfunc[i]!=EMPTY){
            func[cnt++] = list[i++];
        }
        func[cnt] = '\0';
        char function[MAX_LIST];
        int j = 0;
        while (j<len-COMMAND_NUM){
            function[j] = retfunc[j+1];
            j++;
        }
        res = ret_func(func,function);
        if (list[ZERO]!=LEFT_B && list[len-1]!=RIGHT_B){
            return false;
        } else {
            return res;
        }
    }
    bool is_var = var(list);
    bool is_literal = literal(list);
    if (is_var || is_literal || strcmp(list,"NIL")==0){
        res = true;
    } 
    return res;
}

bool io_func(char func[], char line[]){
    bool res;
    if (strcmp(func,"SET")==0){
        res = is_set(func, line);
    } else {
        res = is_print(func, line);
    }
    return res;
}

bool is_set(char func[], char line[]){
    char VAR[COMMAND_NUM], list[MAX_LIST];
    bool res = true, is_var, check_list;
    int i=strlen(func)+1, len=strlen(line), cnt=0;
    VAR[ZERO] = line[i++];
    VAR[FILE_START] = '\0';
    i++;
    is_var = var(VAR);
    while (i<len){
        list[cnt++] = line[i++];
    }
    list[cnt] = '\0';
    check_list = is_list(list);
    res = is_var && check_list;
    return res;
}

bool is_print(char func[], char line[]){
    char list_str[MAX_LIST];
    int i=strlen(func)+1, len=strlen(line), cnt=0;
    bool res = false, check_list, is_str;
    while (i<len){
        list_str[cnt++] = line[i++];
    }
    list_str[cnt] = '\0';
    check_list = is_list(list_str);
    is_str = is_string(list_str);
    if (check_list||is_str){
        res = true;
    }
    return res;
}

bool int_func(char func[], char line[]){
    int i=strlen(func)+1, len=strlen(line), cnt=0;
    char list[MAX_LIST];
    bool res = false;
    while (i<len){
        list[cnt++] = line[i++];
    }
    list[cnt] = '\0';
    if (strcmp(func,"PLUS")==0){
        char l[MAX_LIST], r[MAX_LIST];
        split_list(l,r,list);
        res = is_list(l) && is_list(r);
    } else if (strcmp(func,"LENGTH")==0){
        res = is_list(list);
    } else {
        res = false;
    }
    return res;
}

bool bool_func(char func[], char line[]){
    bool res=false;
    int i=strlen(func)+1, len=strlen(line), cnt=0;
    char list[MAX_LIST];
    while (i<len){
        list[cnt++] = line[i++];
    }
    list[cnt] = '\0';
    char l[MAX_LIST], r[MAX_LIST];
    split_list(l,r,list);
    res = is_list(l) && is_list(r);
    return res;
}

bool if_condition(char function[]){
    char boolfunc[MAX_LIST], prog[MAX_LIST], trim_func_symbol[MAX_LIST];
    int len = strlen(function), i = 3, cnt = 0;
    bool res = false;
    while (i<len){
        trim_func_symbol[cnt++] = function[i++];
    }
    trim_func_symbol[cnt] = '\0';
    split_list(boolfunc,prog,trim_func_symbol);
    char bool_symbol[MAX_FUNC_LEN];
    i = 1;
    cnt = 0;
    while (boolfunc[i]!=EMPTY){
        bool_symbol[cnt++] = boolfunc[i++];
    }
    bool_symbol[cnt] = '\0';
    // substring()
    res = trim_brackets(boolfunc);
    res = res && bool_func(bool_symbol,boolfunc);
    res = res && check_trim_instrcts(prog,2);
    // printf("res of instrcts %i\n",res);
    return res;
}

bool loop(char function[]){
    char boolfunc[MAX_LIST], prog[MAX_LIST], trim_func_symbol[MAX_LIST];
    int len = strlen(function), i = 6, cnt = 0;
    bool res = false;
    while (i<len){
        trim_func_symbol[cnt++] = function[i++];
    }
    trim_func_symbol[cnt] = '\0';
    split_list(boolfunc,prog,trim_func_symbol);
    char bool_symbol[MAX_FUNC_LEN];
    i = 1;
    cnt = 0;
    while (boolfunc[i]!=EMPTY){
        bool_symbol[cnt++] = boolfunc[i++];
    }
    bool_symbol[cnt] = '\0';
    // substring()
    res = trim_brackets(boolfunc);
    res = res && bool_func(bool_symbol,boolfunc);
    res = res && check_trim_instrcts(prog,1);
//    printf("res of instrcts %i\n",res);
    return res;
}

bool check_trim_instrcts(char prog[],int cnt){
    bool res = false;
    if (cnt==2){
        char left[MAX_LIST],right[MAX_LIST];
        split_list(left,right,prog);
        substring(left,1,(int)strlen(left));
        substring(right,1,(int)strlen(right));
        res = instrcts(left);
        res = res && instrcts(right);
    } else {
        substring(prog,1,(int)strlen(prog));
//        printf("cnt==1 %s\n",prog);
        res = instrcts(prog);
//        printf("instrcts res is %i",res);
    }
    return res;
}



/* interpreter */
#ifdef INTERP
void interpStart(data* a){
    lisp* n_list[NUCLEI_LEN];
    for (int i=0;i<NUCLEI_LEN;i++){
        n_list[i] = NULL;
    }
    char line[MAX_STR] = "";
    int len = strlen(a->file);
    for (int i=0;i<len;i++){
        line[i] = a->file[i];
    }
    interpProg(line,n_list);
}

void interpProg(char* line, lisp* n_list[]){
    char cur[MAX_STR];
    cur[0] = '\0';
    int i = 2, len = strlen(line), cnt = 0;
    while (i<len){
        cur[cnt++]=line[i++];
    } // trim single '('
    cur[cnt]='\0';
    interpInstrcts(cur,n_list);
}

void interpInstrcts(char cur[],lisp* n_list[]){
    char ins[MAX_STR], inss[MAX_STR];
    int i = 0, len = strlen(cur), cnt=0;
    if (cur[0] ==' ' || cur[0]=='\0'){
        return;
    }
    i = copy_str_by_bracket(cur,ins); // update ins[]
    if (cur[i]==EMPTY){
        i++;
    }
    while (i<len){
        inss[cnt++]=cur[i++];
    }
    inss[cnt]='\0';
//    if (inss[0]!=')'){
//        interpInstrct(ins,n_list);
//        interpInstrcts(inss,n_list);
//    }
    interpInstrct(ins,n_list);
    interpInstrcts(inss,n_list);
}

void interpInstrct(char cur[],lisp* n_list[]){
    int start = 1, len = strlen(cur);
    substring(cur,start,len-1);
    if (cur[0]=='\0' || cur[0]==' '){
        return;
    }
    interpFunc(n_list,cur);
}

void interpFunc(lisp* n_l[], char function[]){
    char func[MAX_LIST], rest[MAX_LIST];
    token_get(function,func,rest);
    if (strcmp(func,"SET")==0 || strcmp(func,"PRINT")==0){
        interpIOfunc(func,function,n_l);
    }
    else if (strcmp(func,"IF")==0){
        interpIF(rest,n_l);
    } 
    else if (strcmp(func,"WHILE")==0){
        interpLoop(rest,n_l);
    }
}

lisp* interpRetFunc(char list[],lisp* n_list[]){
    int len = strlen(list);
    substring(list,1,len-1); // trim ()
    char func[MAX_LIST],rest[MAX_LIST];
    token_get(list,func,rest);
    if (strcmp(func,"CAR")==0 || strcmp(func,"CDR")==0 || strcmp(func,"CONS")==0){
        lisp* a = interpListFunc(func,rest,n_list);
        return a;
    } else if (strcmp(func,"PLUS")==0 || strcmp(func,"LENGTH")==0){
        int res = interpIntFunc(func,rest,n_list);
        // printf("test res [%i]",res);
        lisp* a = (lisp*)ncalloc(1,sizeof(lisp));
        a->atom = res;
        a->isInt = true;
        return a;
    } else {
        // bool
        bool res = interpBoolFunc(func,rest,n_list);
        lisp* a = (lisp*)ncalloc(1,sizeof(lisp));
        a->atom = res;
        a->isBool = true;
        return a;
    }
}

void interpIOfunc(char func[],char function[],lisp* n_list[]){
    char rest[MAX_LIST];
    int len = strlen(function), len_func = strlen(func);
    if (len_func==3){ // func_len==3:SET
        char var[MAX_LIST];
        substring(function,4,len);
        token_get(function,var,rest); // get var
        // <SET> ::= "SET" <VAR> <LIST>
        interpSet(var,rest,n_list);
    } else {
        // <PRINT>::= "PRINT" <LIST> | "PRINT" <STRING>
        substring(function,6,len);
        bool is_str = is_string(function);
        if (is_str){
            len = strlen(function);
            substring(function,1,len-1);
            printf("%s\n",function);
        } else {
            interpPrint(function,n_list);
        }
    }
}

void interpSet(char* var,char list[], lisp* n_list[]){
    // SET: "SET" <VAR> <LIST>
    int len = strlen(list);
    char quote[1] = {"'"};
    if (len==1){
        n_list[var[0]-'A'] = n_list[list[0]-'A'];
    } else if (list[0]==quote[0]){
        substring(list,1,len-1);
        lisp* a = lisp_fromstring(list);
        a->isSet = true;
        n_list[var[0]-'A'] = a;
    } else {
        // list is a retfunc, save the outcome of func to the var.atom
        lisp* a = interpRetFunc(list,n_list);
        n_list[var[0]-'A'] = a;
    }
}

void interpPrint(char list[],lisp* n_list[]){
    // ::= <VAR> | <LITERAL> | "NIL" | "(" <RETFUNC> ")"
    char quote[1] = {"'"};
    int len = strlen(list);
    if (strcmp(list,"NIL")==0){
        printf("%s\n",list);
    } else if (list[0]==quote[0]){
        substring(list,1,len-1);
        printf("%s\n",list);
    } else if (len==1){
        // VAR: print contents of VAR
        char print[MAX_LIST]="";
        lisp_tostring(n_list[list[0]-'A'],print);
        printf("%s\n",print);
    } else {
        // retfunc: print either list/int/bool
        int len = strlen(list);
        substring(list,1,len-1); // trim ()
        char func[MAX_LIST],rest[MAX_LIST];
        token_get(list,func,rest);
        char print[MAX_LIST];
        if (strcmp(func,"CAR")==0 || strcmp(func,"CDR")==0 || strcmp(func,"CONS")==0){
            lisp* a = interpListFunc(func,rest,n_list);
            lisp_tostring(a,print);
            printf("%s\n",print);
        } else if (strcmp(func,"PLUS")==0 || strcmp(func,"LENGTH")==0){
            int res = interpIntFunc(func,rest,n_list);
            printf("%i\n",res);
        } else {
            bool res = interpBoolFunc(func,rest,n_list);
            printf("%i\n",res);
        }
    }
}

lisp* interpListFunc(char func[],char list[],lisp* n_l[]){
    lisp* a = (lisp*)ncalloc(1,sizeof(lisp));
    lisp* b = (lisp*)ncalloc(1,sizeof(lisp));
    if (strcmp(func,"CAR")==0){
        a = interpList(list,n_l);
        b = a->car;
    } else if (strcmp(func,"CDR")==0){
        a = interpList(list,n_l);
        b = a->cdr;
    } else {
        // CONS <list><list>
        char l[MAX_LIST],r[MAX_LIST];
        split_list(l,r,list);
        a = interpList(l,n_l);
        b = interpList(r,n_l);
        lisp* c = lisp_cons(a,b);
        return c;
    }
    return b;
}

lisp* interpList(char list[],lisp* n_l[]){
// ::= <VAR> | <LITERAL> | "NIL" | "(" <RETFUNC> ")"
    bool is_var = var(list);
    bool is_literal = literal(list);
    bool is_nil = (strcmp(list,"NIL")==0);
    lisp* a = (lisp*)ncalloc(1,sizeof(lisp));
    if (is_var){
        a = n_l[list[0]-'A'];
    } else if (is_literal){
        int len = strlen(list);
        substring(list,1,len-1);// trim ''
        a = lisp_fromstring(list);
    } else if (is_nil){
        a = NULL;
    } else {
        int len = strlen(list);
        substring(list,1,len-1);
        char func[MAX_FUNC_LEN],sub_list[MAX_LIST];
        split_list(func,sub_list,list);
        a = interpListFunc(func,sub_list,n_l);
    }
    return a;
}

int interpIntFunc(char func[],char list[],lisp* n_list[]){
    int res = 0,left,right;
    if (strcmp(func,"PLUS")==0){
        char l[MAX_LIST],r[MAX_LIST];
        split_list(l,r,list);
        if (var(l)){
            left = n_list[l[0]-'A']->atom;
        } else {
            // left is minus number
            left = get_number(l);
        }
        if (var(r)){
            right = n_list[r[0]-'A']->atom;
        } else {
            right = get_number(r);
        }
        res = left+right;
    } else if (strcmp(func,"LENGTH")==0){
        if (var(list)){
            res = lisp_length(n_list[list[0]-'A']);
        } else {
            int len = strlen(list);
            substring(list,1,len-1);// literal
            lisp* a = lisp_fromstring(list);
            res = lisp_length(a);
        }
    }
    return res;
}

int get_number(char num[]){
    int res = 0,flag = 1;
    int i=0, len = strlen(num);
    if (num[0]=='-'){
        flag = -1;
        i++;
    }
    while (i < len){
        res = 10*res+(num[i]-'0');
    }
    if (flag == -1){
        res = -res;
    }
    return res;
}

bool interpBoolFunc(char func[],char list[],lisp* n_list[]){
    bool res = false;
    char l[MAX_LIST],r[MAX_LIST];
    int left = 0,right = 0;
    split_list(l,r,list);
    if (var(l)){
        char tmp[MAX_LIST];
        lisp_tostring(n_list[l[0]-'A'],tmp);
        left = tmp[0]-'0';
    } else {
        left = l[1]-'0';
    }
    if (var(r)){
        char tmp[MAX_LIST];
        lisp_tostring(n_list[r[0]-'A'],tmp);
        right = tmp[0]-'0';
        right = n_list[r[0]-'A']->atom;
    } else {
        right = r[1]-'0';
    }
    if (strcmp(func,"LESS")==0){
        res = (left<right)?true:false;
    } else if (strcmp(func,"GREATER")==0){
        res = (left>right)?true:false;
    } else if (strcmp(func,"EQUAL")==0){
        res = (left==right)?true:false;
    }
    return res;
}

void interpIF(char list[],lisp* n_list[]){
    char function[MAX_LIST],l_ins[MAX_LIST],r_ins[MAX_LIST];
    char rest[MAX_LIST];
    split_list(function,rest,list);
    int len = strlen(function);
    substring(function,1,len-1); // trim brackets
    char func[MAX_LIST],func_list[MAX_LIST];
    token_get(function,func,func_list);//get func;
    bool res = interpBoolFunc(func,func_list,n_list);
    split_list(l_ins,r_ins,rest);
    int len_l = strlen(l_ins), len_r = strlen(r_ins);
    substring(l_ins,1,len_l);
    substring(r_ins,1,len_r);
    if (res){
        interpInstrcts(l_ins,n_list);
    } else {
        interpInstrcts(r_ins,n_list);
    }
}

void interpLoop(char list[],lisp* n_list[]){
    char function[MAX_LIST],rest[MAX_LIST];
    split_list(function,rest,list);
    int len = strlen(function);
    substring(function,1,len-1);// trim brackets
    char func[MAX_FUNC_LEN],func_list[MAX_LIST];
    token_get(function,func,func_list);//get func;
    bool res = true;
    int len_i = strlen(rest);
    substring(rest,1,len_i); // trim (
    while (res){
        interpInstrcts(rest,n_list);
        res = interpBoolFunc(func,func_list,n_list);
    }
}
#endif


/* Helper Function */
void token_get(char* cur, char* token, char* rest){
    int i = 0, len = strlen(cur), start = 0;
    while (cur[i]=='('){
        i++;
    }    
    while (cur[i]!=')' && cur[i]!=' '){
        token[start++] = cur[i++];
    }
    token[start]='\0';
    // skip space and ')'
    while (cur[i]==')'||cur[i]==' '){
        i++;
    }
    start = 0;
    while (i<len){
        rest[start++] = cur[i++];
    }
    rest[start]='\0';
}

int copy_str_by_bracket(char* cur, char* ins){
    bool finish = false;
    int i = 0, cnt_quote = 0;
    while (!finish){
        if (cur[i]=='('){
            cnt_quote++;
        } else if (cur[i]==')'){
            cnt_quote--;
        }
        ins[i] = cur[i];
        if (cnt_quote==0){
            finish = true;
        }
        i++;
    }
    ins[i]='\0';
    return i;
}

bool if_ret(char func[]){
    bool res = false;
    if (strcmp(func,"CAR")==0 || strcmp(func,"CDR")==0 || strcmp(func,"CONS")==0){
        res = true;
    } else if (strcmp(func,"PLUS")==0 || strcmp(func,"LENGTH")==0){
        res = true;
    } else if (strcmp(func,"LESS")==0 || strcmp(func,"GREATER")==0 || strcmp(func,"EQUAL")==0){
        res = true;
    }
    return res;
}

bool var(char list[]){
    bool res = false;
    char character = list[0];
    if (isupper(character)){
        res = true;
    }
    return res;
}

bool literal(char list[]){
    int i=0, len=strlen(list);
    bool res = false;
    char quote[1] = "'";
    if (list[i]==quote[0] && list[len-1]==quote[0]){
        res = true;
    }
    return res;
}

bool is_string(char str[]){
    int i=0, len=strlen(str);
    bool res = false;
    if (str[i]==str[len-1] && str[i]==DOUBLE_QUOTE){
        res = true;
    } 
    return res;
}

void split_list(char* left, char* right, char list[]){
    bool finish = false;
    int start = 0, i = 0, len = strlen(list), cnt = 0;
    int cnt_symbol = 0;
    char quote[1] = "'";
    if (list[start]=='('){
        while (!finish){
            if (list[i]=='('){
                cnt_symbol++;
            } else if (list[i]==')'){
                cnt_symbol--;
            }
            left[i] = list[i];
            i++;
            if (cnt_symbol==0){
                finish = true;
            }
        }
    } else if (list[start]==quote[0]){
        while (!finish){
            if (list[i]==quote[0] && i==0){
                cnt_symbol++;
            } else if (list[i]==quote[0] && i!=0){
                cnt_symbol--;
            }
            left[i] = list[i];
            i++;
            if (cnt_symbol==0){
                finish = true;
            }
        }
    } else {
        while (list[i]!=' '){
            left[i] = list[i];
            i++;
        }
    }
    left[i] = '\0';
    i++;
    while (i<len){
        right[cnt++] = list[i++];
    }
    right[cnt] = '\0';
}

bool trim_brackets(char* function){
    int len = strlen(function);
    int start = 0 , end = len;
    if (function[0] == '('){
        start ++;
    }
    if (function[len-1] == ')'){
        end --;
    }
    substring(function,start,end);
    return true;
}

void substring(char* function, int start, int end){
    char cp[MAX_LIST];
    int i = 0;
    while (start<end){
        cp[i++] = function[start++];
    }
    cp[i] = '\0';
    strcpy(function,cp);
}


void test(void){
#ifdef INTERP
    char c[MAX_LIST] = {"( (SET A '5') (SET B '(1 2 3)') )"};
    lisp* test_list[NUCLEI_LEN];
    interpInstrcts(c,test_list);
    // printf("%i",res);linked.c
    assert(test_list[0]->atom==5);
    assert(test_list[1]->isSet==true);
    assert(test_list[1]->car->atom==1);

    char d[MAX_LIST] = {"( (SET A (CAR '(6 7 8)')) (SET B (CONS A NIL)))"};
    char e[MAX_LIST] = "";
    interpInstrcts(d,test_list);
    lisp* f = (lisp*)ncalloc(1,sizeof(lisp));
    f = test_list[0];
    assert(lisp_isatomic(test_list[0]));
    lisp_tostring(f,e);
    assert(strcmp(e,"((6 7 8))"));

    char g[MAX_LIST] = {"( (SET A (PLUS '1' '2')) (SET B (LENGTH '((1 2 3) 4 5)')) )"};
    e[0] = '\0';
    interpInstrcts(g,test_list);
    assert(test_list[0]->isInt);
    assert(test_list[1]->isInt);
    assert(test_list[0]->atom==3);
    assert(test_list[1]->atom==3);

    char h[MAX_LIST] = {"( (SET A (LESS A '6')) (SET B (GREATER A B)) )"};
    // A B are updated
    interpInstrcts(h,test_list);
    assert(test_list[0]->isBool);
    assert(test_list[1]->isBool);
    assert(test_list[0]->atom==1);
    assert(test_list[1]->atom==0);

    char I[MAX_LIST] = {"( (IF (LESS A B)) (SET A '1') (SET A '0') )"};
    interpInstrcts(I,test_list);
    assert(test_list[0]->atom==0);

    char j[MAX_LIST] = {"( (WHILE (EQUAL A B)) (SET A (PLUS '1' A)))"};
    interpInstrcts(j,test_list);
    assert(test_list[0]->atom==1);
#else
// test 1
    data b;
    char parse_a[MAX_LIST] = {"( (SET A '1')(PRINT \"YES\") )"};
    strcpy(b.file,parse_a);
    b.line = 3;
    fill_space(&b);
    assert(strcmp(b.file,"( (SET A '1') (PRINT \"YES\") )")==0);
    assert(prog(&b));
// test 2
    assert(parser(&b));
    assert(prog(&b));
    char parse_b[MAX_LIST] = ("((PRINT A)");
    strcpy(b.file,parse_b);
    assert(!prog(&b));
// test 3
    strcpy(b.file,parse_a);
    fill_space(&b);
    prog(&b);
    strcpy(parse_b,b.file);
// test 4
    assert(!instrcts(parse_b));
    char parse_c[MAX_LIST] = {"(PRINT A) )"};
    assert(instrcts(parse_c));
// test 5
    char parse_d[MAX_LIST] = {"(CAR A) (CDR B) (CONS A B) )"};    
    assert(!func(parse_d));
    char parse_e[MAX_LIST] = {"(PLUS A '1') (PRINT (LENGTH A) )"};
    assert(func(parse_e));
    char parse_f[MAX_LIST] = {"(LESS A B) (GREATER A B) )"};
    assert(func(parse_f));
// test 6
    // char parse_g[MAX_LIST] = {"(IF (EQUAL A B) ((PRINT A)) ((PRINT B)) )"};
    // assert();
#endif
}

