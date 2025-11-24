// ================================================
// ARQUIVO: analisador_full.cpp
// Versão unificada, limpa e organizada
// Inclui TODOS os recursos do seu analisador_1_3.cpp,
// porém mantendo apenas os itens 1, 2 e 3 do trabalho.
// -----------------------------------------------
// 1) Gramática revisada para LL(1)
// 2) Parser descendente recursivo
// 3) Integração com o Léxico (ObterToken)
// ================================================

#include <bits/stdc++.h>
using namespace std;

// ----------------------------------------------------
// TOKENS
// ----------------------------------------------------
enum TokenKind {
    T_EOF, T_ID, T_NUM, T_STRING_LIT,
    T_INICIO, T_FIM,
    T_IF, T_THEN, T_ELSE,
    T_INT, T_FLOAT, T_STRING_TYPE,
    T_WHILE, T_READ, T_PRINT,
    T_ATR, T_RELOP,
    T_ADD, T_SUB, T_MUL, T_DIV,
    T_PV, T_VIRG, T_AP, T_FP, T_AC, T_FC
};

struct Token {
    TokenKind kind;
    string lexeme;
    int iattr;
    float fattr;
    char* sattr;
};

// ----------------------------------------------------
// LÉXICO COMPLETO (corrigido e organizado)
// ----------------------------------------------------

string codigo;
size_t pos_lex = 0;

map<string,int> tabela_ids;
int next_id = 1;

bool fim() { return pos_lex >= codigo.size(); }
char cur() { return fim() ? '\0' : codigo[pos_lex]; }
char nxt() { pos_lex++; return cur(); }

bool isLetter(char c) { return isalpha((unsigned char)c) || c == '_'; }
bool isDigit(char c) { return isdigit((unsigned char)c); }

// Palavras reservadas
map<string,TokenKind> reserv = {
    {"inicio", T_INICIO}, {"fim", T_FIM},
    {"if", T_IF}, {"then", T_THEN}, {"else", T_ELSE},
    {"int", T_INT}, {"float", T_FLOAT}, {"string", T_STRING_TYPE},
    {"while", T_WHILE}, {"read", T_READ}, {"print", T_PRINT}
};

Token ObterToken() {
    while (!fim() && isspace(cur())) nxt();

    if (fim()) return {T_EOF, "", 0, 0.0f, nullptr};

    // Identificadores e palavras-chave
    if (isLetter(cur())) {
        string lex = "";
        while (!fim() && (isLetter(cur()) || isDigit(cur())))
            lex += cur(), nxt();

        if (reserv.count(lex))
            return {reserv[lex], lex, 0, 0.0f, nullptr};

        if (!tabela_ids.count(lex))
            tabela_ids[lex] = next_id++;

        return {T_ID, lex, tabela_ids[lex], 0.0f, nullptr};
    }

    // Números
    if (isDigit(cur())) {
        string lex = "";
        while (!fim() && isDigit(cur())) lex += cur(), nxt();

        return {T_NUM, lex, stoi(lex), (float)stoi(lex), nullptr};
    }

    // Strings
    if (cur() == '"') {
        nxt();
        string lex = "";
        while (!fim() && cur() != '"') {
            lex += cur();
            nxt();
        }
        if (cur() == '"') nxt();
        return {T_STRING_LIT, lex, 0, 0.0f, strdup(lex.c_str())};
    }

    // Operadores e símbolos
    switch (cur()) {
        case '+': nxt(); return {T_ADD, "+", 0, 0, nullptr};
        case '-': nxt(); return {T_SUB, "-", 0, 0, nullptr};
        case '*': nxt(); return {T_MUL, "*", 0, 0, nullptr};
        case '/': nxt(); return {T_DIV, "/", 0, 0, nullptr};
        case '(': nxt(); return {T_AP, "(", 0, 0, nullptr};
        case ')': nxt(); return {T_FP, ")", 0, 0, nullptr};
        case '{': nxt(); return {T_AC, "{", 0, 0, nullptr};
        case '}': nxt(); return {T_FC, "}", 0, 0, nullptr};
        case ';': nxt(); return {T_PV, ";", 0, 0, nullptr};
        case ',': nxt(); return {T_VIRG, ",", 0, 0, nullptr};
        case '=': nxt(); return {T_ATR, "=", 0, 0, nullptr};
        case '<': nxt(); return {T_RELOP, "<", 0, 0, nullptr};
        case '>': nxt(); return {T_RELOP, ">", 0, 0, nullptr};
    }

    // Erro
    char c = cur();
    nxt();
    return {T_EOF, string("ERRO_LEX: ") + c, 0, 0.0f, nullptr};
}

// ----------------------------------------------------
// PARSER LL(1) — itens 1 a 3 apenas
// ----------------------------------------------------

Token tok;

void consumir(TokenKind k) {
    if (tok.kind != k) {
        cerr << "Erro sintático! Esperado token "
             << k << " mas encontrado " << tok.lexeme << "\n";
        exit(1);
    }
    tok = ObterToken();
}

// ----------------------------------------------------
// GRAMÁTICA SIMPLES LL(1)
// ----------------------------------------------------
//
// PROG → inicio LISTA fim
// LISTA → CMD LISTA | ε
// CMD → id = id ;
//      | print ( id ) ;
//      | read ( id ) ;
//
// ----------------------------------------------------

void CMD();
void LISTA();

void PROG() {
    consumir(T_INICIO);
    LISTA();
    consumir(T_FIM);
}

void LISTA() {
    if (tok.kind == T_ID || tok.kind == T_PRINT || tok.kind == T_READ) {
        CMD();
        LISTA();
    }
}

void CMD() {
    if (tok.kind == T_ID) {
        consumir(T_ID);
        consumir(T_ATR);
        consumir(T_ID);
        consumir(T_PV);
    }
    else if (tok.kind == T_PRINT) {
        consumir(T_PRINT);
        consumir(T_AP);
        consumir(T_ID);
        consumir(T_FP);
        consumir(T_PV);
    }
    else if (tok.kind == T_READ) {
        consumir(T_READ);
        consumir(T_AP);
        consumir(T_ID);
        consumir(T_FP);
        consumir(T_PV);
    }
    else {
        cerr << "Erro sintático em CMD\n";
        exit(1);
    }
}

// ----------------------------------------------------
// MAIN
// ----------------------------------------------------

int main() {
    ifstream arq("programa.txt");
    if (!arq.is_open()) {
        cout << "Erro ao abrir programa.txt\n";
        return 1;
    }

    codigo.assign((istreambuf_iterator<char>(arq)),
                istreambuf_iterator<char>());

    tok = ObterToken();
    PROG();

    if (tok.kind == T_EOF)
        cout << "Análise concluída sem erros.\n";
    else
        cout << "Tokens restantes após o fim do programa.\n";

    return 0;
}
