/* compiler.cpp
   Lexer original (mantido) + Parser LL(1) (tabela/pilha)
   Compile: g++ -std=c++11 compiler.cpp -o compiler
   Run: ./compiler  (ele lê programa.txt)
*/

#include <vector>
#include <exception>
#include <iostream>
#include <stdio.h>
#include <ctype.h>
#include <cstring>
#include <cstdlib>
#include <string>
#include <map>
#include <stack>

// ==== TOKENS ====
#define INICIO 255
#define IF  256
#define THEN  257
#define ELSE  258
#define RELOP  259
#define ID  260
#define NUM  261
#define FIM 268
#define FLOAT 269
#define STRING_TYPE 270
#define WHILE 271
#define READ 272
#define PRINT 273
#define INT 274
#define ATR 275
#define SUB 276
#define ADD 277
#define MUL 278
#define DIV 279
#define PONTO_VIRGULA 280
#define VIRGULA 281
#define ABRE_PARENTESES 282
#define FECHA_PARENTESES 283
#define ABRE_CHAVES 284
#define FECHA_CHAVES 285

// Relop attrs
#define LT 262
#define LE 263
#define EQ 264
#define NE 265
#define GT 266
#define GE 267

std::map<std::string,int> lista_ids; // lista de identificadores

struct Token{
    int nome_token;
    union{
        int i_atributo;
        float f_atributo;
        char * s_atributo;
    }atributo;
};

// Lexer globals
int estado = 0;
int partida = -1;
int cont_sim_lido = 0;
int valor_lexico;
char *code;

// readFile (unchanged)
char *readFile(const char *fileName)
{
    FILE *file = fopen(fileName, "r");
    int n = 0;
    int c;

    if(file == NULL) return NULL;

    fseek(file, 0, SEEK_END);
    long f_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    if (f_size < 0) {
        fclose(file);
        return NULL;
    }

    code = new char [f_size + 1];

    while (((c = fgetc(file))!= EOF))
    {
        if(n<f_size){
            code[n++] = (char) c;
        }
    }
    code[n] = '\0';
    fclose(file);
    return code;
}

int falhar()
{
    switch(estado)
    {
    case 0: partida = 0; printf("Eu não fui identificado aa\n"); break; exit(EXIT_FAILURE);

    case 9:{
        partida = 9;
        printf("Erro: identificador muito longo\n");
        }
        break;

    case 12: partida = 20; break;

    case 14: partida = 14;
        printf("Comentário longo não fechado\n");
        break;
    case 20: partida = 25; break;

    case 25:
        printf("Erro encontrado no código\n");
        cont_sim_lido++;
        break;

    default: printf("Erro do compilador");
    }
    return(partida);
}

// proximo_token (mantido, com pequenas correções locais)
Token proximo_token()
{
    static int count_id = 0;
    Token token;
    char c;
    static int estado = 0;
    static int cont_sim_lido = 0;

    while(code[cont_sim_lido] != '\0')
    {
        switch(estado)
        {
            case 0:{
                c = code[cont_sim_lido];
                if (isspace(c) || c == '\n')
                {
                    estado = 25;
                }
                else if(c == '<') estado = 1;
                else if(c == '=') estado = 5;
                else if(c == '>') estado = 6;
                else if(c == '"') estado = 10;
                else if(c == ';') estado = 21;
                else if(c == ',') estado = 22;
                else if(c == '(') estado = 23;
                else if(c == ')') estado = 24;
                else if( c == '{') estado = 26;
                else if( c == '}') estado = 27;
                else if((c == '-') && code[cont_sim_lido + 1] == ' '){
                    estado = 15;
                }
                else if((c == '+') && code[cont_sim_lido + 1] == ' '){
                    estado = 16;
                }
                else if((c == '*') && code[cont_sim_lido + 1] == ' '){
                    estado = 17;
                }
                else if((c == '/') && code[cont_sim_lido + 1] == ' '){
                    estado = 18;
                }
                else if( c == '-') {
                    int avante = cont_sim_lido + 1;
                    if(code[avante] == '-'){
                        avante++;
                        if((code[avante] == '[') && code[avante+1] == '['){
                            estado = 14;
                        }
                        else{
                            estado = 13;
                        }
                    }
                }
                else if(isdigit(c) || (c=='-')) estado = 12;
                else if(isalpha(c) || (c == '_')) estado = 9;
                else
                {
                    estado = falhar();
                    break;
                }
            }
            break;

            case 1:{
                cont_sim_lido++;
                c = code[cont_sim_lido];
                if(c == '=') estado = 2;
                else if(c == '>') estado = 3;
                else estado = 4;
            }
            break;

            case 2:{
                cont_sim_lido++;
                printf("<relop, LE>\n");
                token.nome_token = RELOP;
                token.atributo.i_atributo = LE;
                estado = 0;
                return(token);
            }
            break;

            case 3:{
                cont_sim_lido++;
                printf("<relop, NE>\n");
                token.nome_token = RELOP;
                token.atributo.i_atributo = NE;
                estado = 0;
                return(token);
            }
            break;

            case 4:{
                cont_sim_lido++;
                printf("<relop, LT>\n");
                token.nome_token = RELOP;
                token.atributo.i_atributo = LT;
                estado = 0;
                return(token);
            }
            break;

            case 5:{
                cont_sim_lido++;
                c=code[cont_sim_lido];
                if(c == '='){
                    estado = 11;
                }
                else{
                    cont_sim_lido++;
                    printf("<ATR, >\n");
                    token.nome_token = ATR;
                    token.atributo.i_atributo = -1;
                    estado = 0;
                    return(token);
                }
            }
            break;

            case 6:{
                cont_sim_lido++;
                c = code[cont_sim_lido];
                if(c == '=') estado = 7;
                else estado = 8;
            }
            break;

            case 7:{
                cont_sim_lido++;
                printf("<relop, GE>\n");
                token.nome_token = RELOP;
                token.atributo.i_atributo = GE;
                estado = 0;
                return(token);
            }
            break;

            case 8:{
                cont_sim_lido++;
                printf("<relop, GT>\n");
                token.nome_token = RELOP;
                token.atributo.i_atributo = GT;
                estado = 0;
                return(token);
            }
            break;

            case 9:{
                c = code[cont_sim_lido];
                char palavra[32];
                int i = 0;

                while (isalpha(code[cont_sim_lido]) || isdigit(code[cont_sim_lido]) || code[cont_sim_lido]=='_') {
                    if( i<31){
                        palavra[i] = code[cont_sim_lido];
                    }
                    else{
                        partida = 9;
                        falhar();
                        break;
                    }
                    i++;
                    cont_sim_lido++;
                }
                palavra[i] = '\0';

                if (strcmp(palavra, "if") == 0) {
                    printf("<IF>\n");
                    token.nome_token = IF;
                }
                else if (strcmp(palavra, "then") == 0) {
                    printf("<THEN>\n");
                    token.nome_token = THEN;
                }
                else if (strcmp(palavra, "else") == 0) {
                    printf("<ELSE>\n");
                    token.nome_token = ELSE;
                }
                else if (strcmp(palavra, "inicio") == 0){
                    printf("<INICIO>\n");
                    token.nome_token = INICIO;
                }
                else if (strcmp(palavra, "fim") == 0){
                    token.nome_token = FIM;
                    printf("<FIM>\n");
                }
                else if (strcmp(palavra, "int") == 0){
                    token.nome_token = INT;
                    printf("<INT>\n");
                }
                else if (strcmp(palavra, "float") == 0){
                    token.nome_token = FLOAT;
                    printf("<FLOAT>\n");
                }
                else if (strcmp(palavra, "string")==0){
                    token.nome_token = STRING_TYPE;
                    printf("<STRING>\n");
                }
                else if (strcmp(palavra, "while")==0) {
                    token.nome_token = WHILE;
                    printf("<WHILE>\n");
                }
                else if (strcmp(palavra, "read")==0) {
                    token.nome_token = READ;
                    printf("<READ>\n");
                }
                else if (strcmp(palavra, "print")==0) {
                    token.nome_token = PRINT;
                    printf("<PRINT>\n");
                }
                else {
                    if(partida != 9){
                        std::string lexema(palavra);
                        if(lista_ids.count(lexema) > 0){
                            int id_atributo = lista_ids[lexema];
                            printf("<id, %d>,\n", id_atributo);
                            token.nome_token = ID;
                            token.atributo.i_atributo = id_atributo;
                            estado = 0;
                        }
                        else{
                            count_id++;
                            lista_ids[lexema] = count_id;
                            token.nome_token = ID;
                            token.atributo.i_atributo = count_id;
                            printf("<id, %d>\n", count_id);
                            estado = 0;
                        }
                        return token;
                    }
                    else{
                        estado = 9;
                        falhar();
                    }
                }

                token.atributo.i_atributo = -1;
                estado = 0;
                return token;
            }
            break;

            case 10:{
                char string_texto[400];
                int j = 0;
                int cont_asp = 1;
                string_texto[j++] = code[cont_sim_lido++];
                while(true){
                    if(cont_asp == 2){
                        break;
                    }
                    if(code[cont_sim_lido] == '"'){
                        cont_asp++;
                    }
                    string_texto[j++] = code[cont_sim_lido++];
                }
                string_texto[j] = '\0';
                token.nome_token = STRING_TYPE;
                token.atributo.s_atributo = strdup(string_texto);
                printf("<STRING_TYPE, %s>\n", token.atributo.s_atributo);
                estado = 0;
                return token;
            }
            break;

            case 11:{
                cont_sim_lido++;
                printf("<relop, EQ>\n");
                token.nome_token = RELOP;
                token.atributo.i_atributo = EQ;
                estado = 0;
                return(token);
            }
            break;

            case 12:{
                char numero[30];
                int j = 0;
                int cont_ponto = 0;
                numero[j++] = code[cont_sim_lido++];
                while(true){
                    char c = code[cont_sim_lido];
                    if(isdigit(c)){
                        numero[j++] = c;
                        cont_sim_lido++;
                    }
                    else if(c == '.'){
                        cont_ponto++;
                        if(cont_ponto > 1){
                            break;
                        }
                        numero[j++] = c;
                        cont_sim_lido++;
                    }
                    else{
                        break;
                    }
                    if(j>=29) break;
                }
                numero[j] = '\0';
                if( cont_ponto==1){
                    token.nome_token = NUM;
                    token.atributo.f_atributo = atof(numero);
                    printf("<float, %f>\n", token.atributo.f_atributo);
                }
                else if(cont_ponto==0){
                    token.nome_token = NUM;
                    token.atributo.i_atributo = atoi(numero);
                    printf("<int, %d>\n", token.atributo.i_atributo);
                }
                estado = 0;
                return token;
            }
            break;

            case 13:{
                while(code[cont_sim_lido]!= '\n'){
                    cont_sim_lido++;
                }
                estado = 0;
            }
            break;

            case 14:{
                cont_sim_lido += 4;
                while(code[cont_sim_lido] != '\0'){
                    if((code[cont_sim_lido]==']') && (code[cont_sim_lido+1]==']')){
                        cont_sim_lido+= 2;
                        estado = 0;
                        break;
                    }
                    cont_sim_lido++;
                }
                if(estado == 14){
                    partida = 14;
                    falhar();
                }
            }
            break;

            case 15:{
                cont_sim_lido++;
                printf("<-,>\n");
                token.nome_token = SUB;
                token.atributo.i_atributo = -1;
                estado = 0;
                return token;
            }
            break;

            case 16:{
                cont_sim_lido++;
                printf("<+,>\n");
                token.nome_token = ADD;
                token.atributo.i_atributo = -1;
                estado = 0;
                return token;
            }
            break;

            case 17:{
                cont_sim_lido++;
                printf("<*,>\n");
                token.nome_token = MUL;
                token.atributo.i_atributo = -1;
                estado = 0;
                return token;
            }
            break;

            case 18:{
                cont_sim_lido++;
                printf("</,>\n");
                token.nome_token = DIV;
                token.atributo.i_atributo = -1;
                estado = 0;
                return token;
            }
            break;

            case 20:{
                c = code[cont_sim_lido];
                if((c == ' ')||(c == '\n'))
                {
                    estado = 0;
                    cont_sim_lido++;
                }
                else
                {
                    estado = falhar();
                }
            }
            break;

            case 21:{
                cont_sim_lido++;
                printf("<;,>\n");
                token.nome_token = PONTO_VIRGULA;
                token.atributo.i_atributo = -1;
                estado = 0;
                return token;
            }
            break;

            case 22:{
                cont_sim_lido++;
                printf("<',',>\n");
                token.nome_token = VIRGULA;
                token.atributo.i_atributo = -1;
                estado = 0;
                return token;
            }
            break;

            case 23:{
                cont_sim_lido++;
                printf("<(,>\n");
                token.nome_token = ABRE_PARENTESES;
                token.atributo.i_atributo = -1;
                estado = 0;
                return token;
            }
            break;

            case 24:{
                cont_sim_lido++;
                printf("<),>\n");
                token.nome_token = FECHA_PARENTESES;
                token.atributo.i_atributo = -1;
                estado = 0;
                return token;
            }
            break;

            case 25:{
                c = code[cont_sim_lido];
                if((c == ' '))
                {
                    estado = 0;
                    cont_sim_lido++;
                }
                else if(c == '\n'){
                    estado = 0;
                    cont_sim_lido++;
                }
                else
                {
                    estado = 0;
                }
            }
            break;

            case 26:{
                cont_sim_lido++;
                printf("<{,>\n");
                token.nome_token = ABRE_CHAVES;
                token.atributo.i_atributo = -1;
                estado = 0;
                return(token);
            }
            break;

            case 27:{
                cont_sim_lido++;
                printf("<},>\n");
                token.nome_token = FECHA_CHAVES;
                token.atributo.i_atributo = -1;
                estado = 0;
                return(token);
            }
            break;

            default:
                falhar();
        }
    }
    token.nome_token = EOF;
    token.atributo.i_atributo = -1;
    return(token);
}

/* ===========================
   PARSER LL(1) (tabela/pilha)
   =========================== */

// Não-terminais (valores >= 1000)
enum NonTerm {
    NT_PROGRAM = 1000,
    NT_STMT_LIST,
    NT_STMT,
    NT_DECL,
    NT_DECL_TAIL,
    NT_TYPE,
    NT_ASSIGN,
    NT_IF,
    NT_IF_TAIL,
    NT_WHILE,
    NT_READ,
    NT_PRINT,
    NT_PRINT_ARG,
    NT_BLOCK,
    NT_EXPR,
    NT_EXPR_REL,
    NT_ADD,
    NT_ADD_TAIL,
    NT_TERM,
    NT_TERM_TAIL,
    NT_FACTOR
};

inline bool is_nonterm(int s) { return s >= 1000; }

std::string token_name(int t){
    switch(t){
        case INICIO: return "INICIO";
        case IF: return "IF";
        case THEN: return "THEN";
        case ELSE: return "ELSE";
        case RELOP: return "RELOP";
        case ID: return "ID";
        case NUM: return "NUM";
        case FIM: return "FIM";
        case FLOAT: return "FLOAT";
        case STRING_TYPE: return "STRING_TYPE";
        case WHILE: return "WHILE";
        case READ: return "READ";
        case PRINT: return "PRINT";
        case INT: return "INT";
        case ATR: return "ATR";
        case SUB: return "SUB";
        case ADD: return "ADD";
        case MUL: return "MUL";
        case DIV: return "DIV";
        case PONTO_VIRGULA: return "PONTO_VIRGULA";
        case VIRGULA: return "VIRGULA";
        case ABRE_PARENTESES: return "ABRE_PARENTESES";
        case FECHA_PARENTESES: return "FECHA_PARENTESES";
        case ABRE_CHAVES: return "ABRE_CHAVES";
        case FECHA_CHAVES: return "FECHA_CHAVES";
        case EOF: return "EOF";
        default: return "TOKEN_DESCONHECIDO";
    }
}

std::string nonterm_to_string(int nt){
    switch(nt){
        case NT_PROGRAM: return "Program";
        case NT_STMT_LIST: return "StmtList";
        case NT_STMT: return "Stmt";
        case NT_DECL: return "Decl";
        case NT_DECL_TAIL: return "DeclTail";
        case NT_TYPE: return "Type";
        case NT_ASSIGN: return "Assign";
        case NT_IF: return "If";
        case NT_IF_TAIL: return "IfTail";
        case NT_WHILE: return "While";
        case NT_READ: return "Read";
        case NT_PRINT: return "Print";
        case NT_PRINT_ARG: return "PrintArg";
        case NT_BLOCK: return "Block";
        case NT_EXPR: return "Expr";
        case NT_EXPR_REL: return "ExprRel";
        case NT_ADD: return "Add";
        case NT_ADD_TAIL: return "AddTail";
        case NT_TERM: return "Term";
        case NT_TERM_TAIL: return "TermTail";
        case NT_FACTOR: return "Factor";
        default: return "NT_UNKNOWN";
    }
}

using RHS = std::vector<int>;
std::map<int, std::map<int, RHS>> parse_table;

void build_parse_table()
{
    // Program -> INICIO StmtList FIM
    parse_table[NT_PROGRAM][INICIO] = { INICIO, NT_STMT_LIST, FIM };

    // StmtList -> Stmt StmtList | ε
    std::vector<int> stmt_starters = { INT, FLOAT, STRING_TYPE, ID, IF, WHILE, READ, PRINT, ABRE_CHAVES };
    for(int t : stmt_starters){
        parse_table[NT_STMT_LIST][t] = { NT_STMT, NT_STMT_LIST };
    }
    parse_table[NT_STMT_LIST][FIM] = { };
    parse_table[NT_STMT_LIST][FECHA_CHAVES] = { };
    parse_table[NT_STMT_LIST][EOF] = { };

    // Stmt -> Decl | Assign | If | While | Read | Print | Block
    parse_table[NT_STMT][INT] = { NT_DECL };
    parse_table[NT_STMT][FLOAT] = { NT_DECL };
    parse_table[NT_STMT][STRING_TYPE] = { NT_DECL };
    parse_table[NT_STMT][ID] = { NT_ASSIGN };
    parse_table[NT_STMT][IF] = { NT_IF };
    parse_table[NT_STMT][WHILE] = { NT_WHILE };
    parse_table[NT_STMT][READ] = { NT_READ };
    parse_table[NT_STMT][PRINT] = { NT_PRINT };
    parse_table[NT_STMT][ABRE_CHAVES] = { NT_BLOCK };

    // Decl -> Type ID DeclTail ;
    parse_table[NT_DECL][INT] = { NT_TYPE, ID, NT_DECL_TAIL, PONTO_VIRGULA };
    parse_table[NT_DECL][FLOAT] = { NT_TYPE, ID, NT_DECL_TAIL, PONTO_VIRGULA };
    parse_table[NT_DECL][STRING_TYPE] = { NT_TYPE, ID, NT_DECL_TAIL, PONTO_VIRGULA };

    // DeclTail -> ATR Expr | ε
    parse_table[NT_DECL_TAIL][ATR] = { ATR, NT_EXPR };
    parse_table[NT_DECL_TAIL][PONTO_VIRGULA] = { };

    // Type -> INT | FLOAT | STRING_TYPE
    parse_table[NT_TYPE][INT] = { INT };
    parse_table[NT_TYPE][FLOAT] = { FLOAT };
    parse_table[NT_TYPE][STRING_TYPE] = { STRING_TYPE };

    // Assign -> ID ATR Expr ;
    parse_table[NT_ASSIGN][ID] = { ID, ATR, NT_EXPR, PONTO_VIRGULA };

    // If -> IF ( Expr ) THEN Stmt IfTail
    parse_table[NT_IF][IF] = { IF, ABRE_PARENTESES, NT_EXPR, FECHA_PARENTESES, THEN, NT_STMT, NT_IF_TAIL };
    // IfTail -> ELSE Stmt | ε   (epsilon on many follow tokens)
    parse_table[NT_IF_TAIL][ELSE] = { ELSE, NT_STMT };
    // allow epsilon in many follow contexts:
    std::vector<int> iftail_follows = { INT, FLOAT, STRING_TYPE, ID, IF, WHILE, READ, PRINT, ABRE_CHAVES, FIM, FECHA_CHAVES, EOF };
    for(int t: iftail_follows) parse_table[NT_IF_TAIL][t] = { };

    // While -> WHILE ( Expr ) Stmt
    parse_table[NT_WHILE][WHILE] = { WHILE, ABRE_PARENTESES, NT_EXPR, FECHA_PARENTESES, NT_STMT };

    // Read -> READ ( ID ) ;
    parse_table[NT_READ][READ] = { READ, ABRE_PARENTESES, ID, FECHA_PARENTESES, PONTO_VIRGULA };

    // Print -> PRINT ( PrintArg ) ;
    parse_table[NT_PRINT][PRINT] = { PRINT, ABRE_PARENTESES, NT_PRINT_ARG, FECHA_PARENTESES, PONTO_VIRGULA };
    parse_table[NT_PRINT_ARG][STRING_TYPE] = { STRING_TYPE };
    parse_table[NT_PRINT_ARG][NUM] = { NT_EXPR };
    parse_table[NT_PRINT_ARG][ID] = { NT_EXPR };
    parse_table[NT_PRINT_ARG][ABRE_PARENTESES] = { NT_EXPR };

    // Block -> { StmtList }
    parse_table[NT_BLOCK][ABRE_CHAVES] = { ABRE_CHAVES, NT_STMT_LIST, FECHA_CHAVES };

    // Expr -> Add ExprRel
    parse_table[NT_EXPR][NUM] = { NT_ADD, NT_EXPR_REL };
    parse_table[NT_EXPR][ID] = { NT_ADD, NT_EXPR_REL };
    parse_table[NT_EXPR][ABRE_PARENTESES] = { NT_ADD, NT_EXPR_REL };
    parse_table[NT_EXPR][STRING_TYPE] = { NT_ADD, NT_EXPR_REL };

    // ExprRel -> RELOP Add | ε
    parse_table[NT_EXPR_REL][RELOP] = { RELOP, NT_ADD };
    parse_table[NT_EXPR_REL][FECHA_PARENTESES] = { };
    parse_table[NT_EXPR_REL][PONTO_VIRGULA] = { };
    parse_table[NT_EXPR_REL][THEN] = { };

    // Add -> Term AddTail
    parse_table[NT_ADD][NUM] = { NT_TERM, NT_ADD_TAIL };
    parse_table[NT_ADD][ID] = { NT_TERM, NT_ADD_TAIL };
    parse_table[NT_ADD][ABRE_PARENTESES] = { NT_TERM, NT_ADD_TAIL };
    parse_table[NT_ADD][STRING_TYPE] = { NT_TERM, NT_ADD_TAIL };

    // AddTail -> ADD|SUB Term AddTail | ε
    parse_table[NT_ADD_TAIL][ADD] = { ADD, NT_TERM, NT_ADD_TAIL };
    parse_table[NT_ADD_TAIL][SUB] = { SUB, NT_TERM, NT_ADD_TAIL };
    // epsilon cases
    std::vector<int> addtail_eps = { RELOP, FECHA_PARENTESES, PONTO_VIRGULA, THEN, EOF, FIM, FECHA_CHAVES };
    for(int t: addtail_eps) parse_table[NT_ADD_TAIL][t] = { };

    // Term -> Factor TermTail
    parse_table[NT_TERM][NUM] = { NT_FACTOR, NT_TERM_TAIL };
    parse_table[NT_TERM][ID] = { NT_FACTOR, NT_TERM_TAIL };
    parse_table[NT_TERM][ABRE_PARENTESES] = { NT_FACTOR, NT_TERM_TAIL };
    parse_table[NT_TERM][STRING_TYPE] = { NT_FACTOR, NT_TERM_TAIL };

    // TermTail -> MUL|DIV Factor TermTail | ε
    parse_table[NT_TERM_TAIL][MUL] = { MUL, NT_FACTOR, NT_TERM_TAIL };
    parse_table[NT_TERM_TAIL][DIV] = { DIV, NT_FACTOR, NT_TERM_TAIL };
    // epsilon cases
    std::vector<int> termtail_eps = { ADD, SUB, RELOP, FECHA_PARENTESES, PONTO_VIRGULA, THEN, EOF, FIM, FECHA_CHAVES };
    for(int t: termtail_eps) parse_table[NT_TERM_TAIL][t] = { };

    // Factor -> NUM | ID | ( Expr ) | STRING_TYPE
    parse_table[NT_FACTOR][NUM] = { NUM };
    parse_table[NT_FACTOR][ID] = { ID };
    parse_table[NT_FACTOR][ABRE_PARENTESES] = { ABRE_PARENTESES, NT_EXPR, FECHA_PARENTESES };
    parse_table[NT_FACTOR][STRING_TYPE] = { STRING_TYPE };
}

void parse_ll1()
{
    build_parse_table();

    std::stack<int> st;
    st.push(EOF);
    st.push(NT_PROGRAM);

    Token cur = proximo_token(); // primeiro token

    while(!st.empty()){
        int top = st.top();
        if(!is_nonterm(top)){
            // terminal
            if(top == cur.nome_token){
                st.pop();
                cur = proximo_token();
            } else {
                printf("\nErro sintático: token esperado '%s', encontrado '%s' (pos %d)\n",
                    token_name(top).c_str(), token_name(cur.nome_token).c_str(), cont_sim_lido);
                exit(EXIT_FAILURE);
            }
        } else {
            auto itNT = parse_table.find(top);
            if(itNT == parse_table.end()){
                printf("Erro interno: não terminal sem entrada na tabela: %s\n", nonterm_to_string(top).c_str());
                exit(EXIT_FAILURE);
            }
            auto &row = itNT->second;
            auto itProd = row.find(cur.nome_token);
            if(itProd == row.end()){
                // tentativa de mostrar erro e contexto
                printf("\nErro sintático: não há produção para não-terminal %s com token %s (pos %d)\n",
                    nonterm_to_string(top).c_str(), token_name(cur.nome_token).c_str(), cont_sim_lido);
                exit(EXIT_FAILURE);
            }
            RHS rhs = itProd->second;
            st.pop();
            for(int i = (int)rhs.size() - 1; i >= 0; --i){
                st.push(rhs[i]);
            }
        }
    }

    printf("Parse LL(1) concluído com sucesso!\n");
}

/* ===========================
   MAIN
   =========================== */

int main ()
{
    try {
        code = readFile("programa.txt");

        if(code == NULL)
        {
            printf("Não foi possível abrir o arquivo 'programa.txt'.\n");
            return 1;
        }

        // chama o parser LL(1) que internamente usa proximo_token()
        parse_ll1();

    }
    catch (const std::exception& e) {
        std::cerr << "\n\n!!! EXCEÇÃO DETECTADA !!!\n";
        std::cerr << "Erro: " << e.what() << "\n";
        return 1;
    }
    catch (...) {
        std::cerr << "\n\n!!! ERRO DESCONHECIDO !!!\n";
        std::cerr << "Ocorreu uma falha não identificada.\n";
        return 1;
    }

    return 0;
}
