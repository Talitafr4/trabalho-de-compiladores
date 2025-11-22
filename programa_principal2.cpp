
/*
 * ESTE CÓDIGO IMPLEMENTA UM ANALIZADOR LÉXICO PARA O EXEMPLO DE FRAGMENTO DE LINGUAGEM APRESENTADO EM SALA DE AULA (VEJA OS SLIDES DA AULA 4: ANÁLISE LÉXICA: PARTE 2)
 * E PODERÁ SER UTILIZADO COMO PONTO DE PARTIDA PARA IMPLEMENTAÇÃO DO ANALISADOR LÉXICO PARA LINGUAGEM ADOTADA NO TRABALHO PROPOSTO.
 * */

#include <vector>

#include <exception> // Necessário para std::exception
#include <iostream>  // Necessário para std::cerr

#include <stdio.h>
#include <ctype.h>
#include <cstring>
#include <cstdlib>

#include <string>
#include <map>

//NOME TOKENS
#define INICIO 255 // int atributo = -1
#define IF  256 // int atributo = -1
#define THEN  257 // int atributo = -1
#define ELSE  258 // int atributo = -1
#define RELOP  259 // int atributo = LT, LE, EQ, NE, GT, GE
#define ID  260	// int atributo = -1
#define NUM  261 // int ou float
#define FIM 268 // int atributo = -1
#define FLOAT 269 // int pra atributo 
#define STRING_TYPE 270 // int atributo = -1 e string para o q tem valor
#define WHILE 271 // int atributo = -1
#define READ 272 // int atributo = -1
#define PRINT 273 // int atributo = -1
#define INT 274 // int atributo = -1
#define ATR 275  // int atributo = -1
#define SUB 276 // int atributo = -1
#define ADD 277	// int atributo = -1
#define MUL 278 // int atributo = -1
#define DIV 279 // int atributo = -1
#define PONTO_VIRGULA 280 // int atributo = -1
#define VIRGULA 281 // int atributo = -1
#define ABRE_PARENTESES 282 // int atributo = -1
#define FECHA_PARENTESES 283 // int atributo = -1
#define ABRE_CHAVES 284 // int atributo = -1
#define FECHA_CHAVES 285 // int atributo = -1


//ATRIBUTOS
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



int estado = 0;
int partida = -1; //segurança
int cont_sim_lido = 0;
int valor_lexico;
char *code;


char *readFile(const char *fileName)
{
	FILE *file = fopen(fileName, "r");
	//char *code;
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
		//retornar msg de erro
		printf("Erro encontrado no código\n");
		cont_sim_lido++;
		break;

	default: printf("Erro do compilador");
	}
	return(partida);
}

Token proximo_token()
{
	static int count_id = 0;
	Token token;
	char c;
	static int estado = 0;
	static int cont_sim_lido = 0;
	//printf("Caracter: %s\n", code);
	//fflush(stdout);
	while(code[cont_sim_lido] != '\0')
	{
		
		
		switch(estado)
		{
			case 0:{ //estado inicial
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
				else if((c == '-') && code[cont_sim_lido + 1] == ' '){ // subtração
					estado = 15;

				}
				else if((c == '+') && code[cont_sim_lido + 1] == ' '){ // adição
					estado = 16;

				}
				else if((c == '*') && code[cont_sim_lido + 1] == ' '){ // multiplicação
					estado = 17;

				}
				else if((c == '/') && code[cont_sim_lido + 1] == ' '){ // divisão
					estado = 18;

				}
				else if( c == '-') {
					int avante = cont_sim_lido + 1;
					if(code[avante] == '-'){
						
						avante++;
						
						if((code[avante] == '[') && code[avante+1] == '['){
							
							//long comment
							estado = 14;

						}
						else{
							
							estado = 13;
							//short comment
						}
					}
				}
				else if(isdigit(c) || (c=='-')) estado = 12; // Float ou int
				else if(isalpha(c) || (c == '_')) estado = 9; // identificação

				else
					{
					 estado = falhar();
					 
					 break;
					}
				}
				break;

			case 1: {// Less than intermediário
				cont_sim_lido++;
				c = code[cont_sim_lido];

				if(c == '=') estado = 2;
				else if(c == '>') estado = 3; //estado de not equal 
				else estado = 4;
				}
				break;

			case 2: {// Less e equal
				cont_sim_lido++;
				printf("<relop, LE>\n");
				token.nome_token = RELOP;
				token.atributo.i_atributo = LE;
				estado = 0;
				return(token);
			}
				break;

			case 3: {// estado do not equal
				cont_sim_lido++;
				printf("<relop, NE>\n");
				token.nome_token = RELOP;
				token.atributo.i_atributo = NE;
				estado = 0;
				return(token);
			}
				break;

			case 4:{ //LESS THAN
				cont_sim_lido++;
				printf("<relop, LT>\n");
				token.nome_token = RELOP;
				token.atributo.i_atributo = LT;
				estado = 0;
				return(token);
			}
				break;

			case 5: {// equals intermediario (atribuição e checagem de '=')
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

			case 6: {// greater than intermediário
				cont_sim_lido++;
				c = code[cont_sim_lido];
				if(c == '=') estado = 7;
				else estado = 8;
				}
				break;

			case 7:{ //Great and equal
				cont_sim_lido++;
				printf("<relop, GE>\n");
				token.nome_token = RELOP;
				token.atributo.i_atributo = GE;
				estado = 0;
				return(token);
				}
				break;

			case 8:{ // Greater than
				cont_sim_lido++;
				printf("<relop, GT>\n");
				token.nome_token = RELOP;
				token.atributo.i_atributo = GT;
				//ATENÇÃO - CORREÇÃO: foi acrescentado o comando "estado = 0;"
				estado = 0;
				return(token);
				}
				break;

			
			case 9: {// identificador
				c = code[cont_sim_lido];
				char palavra[32];
				int i = 0;
				
				while (isalpha(code[cont_sim_lido]) || isdigit(code[cont_sim_lido]) || code[cont_sim_lido]=='_') {
					if( i<31){
						palavra[i] = code[cont_sim_lido];
						
					}
					else{
						// Excedeu tamanho máximo do identificador
						//printf("Erro: identificador muito longo\n");
						
						partida = 9;
						falhar();
						break;
					}
					i++;
					cont_sim_lido++;
				}
				palavra[i] = '\0';
				//printf("Identificador completo: %s \n", palavra);

				
				// Verifica palavras-chave
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
							// simbolo existente já
							int id_atributo = lista_ids[lexema];
							//token.nome_token = ID;
							//token.atributo.i_atributo = id_atributo;
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
							//printf("Novo id adicionado: %s com atributo %d\n", palavra, count_id);
							printf("<id, %d>\n", count_id);
							estado = 0;
						}
						return token;
						
						
						
					}
					else{
						estado = 9;
						//printf("falhei %s\n e caracter %c\n", palavra, code[cont_sim_lido]);
						falhar();
					}
				}

				token.atributo.i_atributo = -1;
				estado = 0;
				return token;
					
					/*implementar ações referentes aos estado */
					
				
				}
				break;

			case 10: {// strings
				char string_texto[400]; // tamanho máximo = 400 caracteres
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


				/*implementar ações para os estados 10, 11, 12*/

			case 11: {// equal equal
				cont_sim_lido++;
				printf("<relop, EQ>\n");
				token.nome_token = RELOP;
				token.atributo.i_atributo = EQ;
				estado = 0;
				return(token);
			}
				break;

			case 12:{ // números inteiros primeiro e float
				char numero[30];
				int j = 0;
				int cont_ponto = 0;
				numero[j++] = code[cont_sim_lido++];
				int cont_num = 1;
				while(true){
					char c = code[cont_sim_lido];
					if(isdigit(c)){
						numero[j++] = c;
						cont_sim_lido++;
					}
					else if(c == '.'){
						cont_ponto++;
						if(cont_ponto > 1){
							//printf(" ERRO: ponto flutuante com dois pontos");
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

			case 13:{ // short comment
				while(code[cont_sim_lido]!= '\n'){
					cont_sim_lido++;
					
				}
				estado = 0;
				
				}
				break;
			case 14:{ // long comment
				cont_sim_lido += 4;
				while(code[cont_sim_lido] != '\0'){
					
					if((code[cont_sim_lido]==']') && (code[cont_sim_lido+1]==']')){
				
						cont_sim_lido+= 2;
						
						estado = 0;
						break;
					}
					cont_sim_lido++;
				}
				/*implementar ações para os estados 13-19*/
				if(estado == 14){
					partida = 14;
					falhar();
				}
				}
				break;

			case 15:{ // subtração
				cont_sim_lido++;
				printf("<-,>\n");
				token.nome_token = SUB;
				token.atributo.i_atributo = -1;
				estado = 0;
				return token;
			}
				break;
			case 16: {// adição
				cont_sim_lido++;
				printf("<+,>\n");
				token.nome_token = ADD;
				token.atributo.i_atributo = -1;
				estado = 0;
				return token;
			}
				break;
			case 17: {// multiplicação
				cont_sim_lido++;
				printf("<*,>\n");
				token.nome_token = MUL;
				token.atributo.i_atributo = -1;
				estado = 0;
				return token;
			}
				break;

			case 18: {// divisão
				cont_sim_lido++;
				printf("</,>\n");
				token.nome_token = DIV;
				token.atributo.i_atributo = -1;
				estado = 0;
				return token;
			}
				break;

			case 20:{ // estado de espaço em branco
				c = code[cont_sim_lido];
				if((c == ' ')||(c == '\n'))
				{
					estado = 0;
					cont_sim_lido++;
				}
				else
				{
					/*implementar ações referentes aos estado */
					estado = falhar();
				}
			}
				break;

			case 21:{ // estado para o símbolo ;
				cont_sim_lido++;
				printf("<;,>\n");
				token.nome_token = PONTO_VIRGULA;
				token.atributo.i_atributo = -1;
				estado = 0;
				return token;
			}
				break;
				/*implementar ações para os estados 21-24*/

			case 22:{ // estado para o símbolo ,
				cont_sim_lido++;
				printf("<',',>\n");
				token.nome_token = VIRGULA;
				token.atributo.i_atributo = -1;
				estado = 0;
				return token;

			}
				break;
			case 23:{ // estado para o símbolo (
				cont_sim_lido++;
				printf("<(,>\n");
				token.nome_token = ABRE_PARENTESES;
				token.atributo.i_atributo = -1;
				estado = 0;
				return token;
			}
				break;

			case 24:{ // estado para o símbolo )
				cont_sim_lido++;
				printf("<),>\n");
				token.nome_token = FECHA_PARENTESES;
				token.atributo.i_atributo = -1;
				estado = 0;
				return token;
			}
				break;

			case 25:{ // espaço e quebra de linha
				c = code[cont_sim_lido];
				if((c == ' '))
				{
					//printf(" ");
					estado = 0;
					cont_sim_lido++;
				}
				else if(c == '\n'){
					//printf("\n");
					estado = 0;
					cont_sim_lido++;
				}
				else
				{
					/*implementar ações referentes aos estado */
					//estado = falhar();
					estado = 0;
					//token.nome_token = -1;
					//token.atributo.i_atributo = -1;
					//return(token);
				}
			}
				break;
			
			case 26:{ // estado para o símbolo {
				cont_sim_lido++;
				printf("<{,>\n");
				token.nome_token = ABRE_CHAVES;
				token.atributo.i_atributo = -1;
				estado = 0;
				return(token);
			}
				break;

			case 27:{ // estado para o símbolo }
				cont_sim_lido++;
				printf("<},>\n");
				token.nome_token = FECHA_CHAVES;
				token.atributo.i_atributo = -1;
				estado = 0;
				return(token);
			}
				break;

			default:
				//printf("Erro do compilador");
				falhar();
		}


	}
	token.nome_token = EOF;
	token.atributo.i_atributo = -1;
	return(token);
}



/*
int main ()
{
	Token token;
	printf("Tentando ler programa.txt...\n");
    code = readFile("programa.txt");
	Exception e;
	printf("aaaaaaa\n");
	if(code == NULL)
	{
		printf("Não foi possível abrir o arquivo.\n");
		return 1;
	}
	printf("Arquivo lido com sucesso!\n");
	printf("Iniciando análise léxica...\n");
    //token = proximo_token();
    //token = proximo_token();
	do{
		token = proximo_token();
	}while((token.nome_token != EOF));
	return 0;


}
	*/
int main ()
{
    try {
        Token token;
        
        
        // 1. Remova a linha "Exception e;" - Ela causa erro de compilação.
        
        code = readFile("programa.txt");


        fflush(stdout); 

        if(code == NULL)
        {
            //printf("Não foi possível abrir o arquivo.\n");
            return 1;
        }

        fflush(stdout);

	do{
		fflush(stdout);
		token = proximo_token();
		//printf("\n");
		fflush(stdout);
		
	} while((token.nome_token != EOF));
	
	//printf("Análise léxica concluída com sucesso!\n");

    } 
    // Captura exceções padrão do C++ (ex: falta de memória no 'new')
    catch (const std::exception& e) {
        std::cerr << "\n\n!!! EXCEÇÃO DETECTADA !!!\n";
        std::cerr << "Erro: " << e.what() << "\n";
        return 1;
    }
    // Captura qualquer outra coisa que foi lançada (throw)
    catch (...) {
        std::cerr << "\n\n!!! ERRO DESCONHECIDO !!!\n";
        std::cerr << "Ocorreu uma falha não identificada.\n";
		
        return 1;
    }

    return 0;
}

