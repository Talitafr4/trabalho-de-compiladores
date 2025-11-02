
/*
 * ESTE CÓDIGO IMPLEMENTA UM ANALIZADOR LÉXICO PARA O EXEMPLO DE FRAGMENTO DE LINGUAGEM APRESENTADO EM SALA DE AULA (VEJA OS SLIDES DA AULA 4: ANÁLISE LÉXICA: PARTE 2)
 * E PODERÁ SER UTILIZADO COMO PONTO DE PARTIDA PARA IMPLEMENTAÇÃO DO ANALISADOR LÉXICO PARA LINGUAGEM ADOTADA NO TRABALHO PROPOSTO.
 * */

#include <stdio.h>
#include <ctype.h>
#include <cstring>


//NOME TOKENS
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


//ATRIBUTOS
#define LT 262
#define LE 263
#define EQ 264
#define NE 265
#define GT 266
#define GE 267



struct Token{
 int nome_token;
 int atributo;
};



int estado = 0;
int partida = 0;
int cont_sim_lido = 0;
int valor_lexico;
char *code;


char *readFile(char *fileName)
{
	FILE *file = fopen(fileName, "r");
	char *code;
	int n = 0;
	int c;

	if(file == NULL) return NULL;

	fseek(file, 0, SEEK_END);
	long f_size = ftell(file);
	fseek(file, 0, SEEK_SET);

	code = new char [f_size + 1];

	while ((c = fgetc(file))!= '\0')
	{
		code[n++] = (char) c;
	}
	code[n] = '\0';
	return code;
}




int falhar()
{
	switch(estado)
	{
	case 0: partida = 9; break;

	case 9: partida = 12; break;

	case 12: partida = 20; break;

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
	int count_id = 0;
	Token token;
	char c;
	while(code[cont_sim_lido] != EOF)
	{
		switch(estado)
		{
			case 0: //estado inicial
				c = code[cont_sim_lido];
				if((c == ' ')||(c == '\n'))
				{
					estado = 0;
					cont_sim_lido++;
				}
				else if(c == '<') estado = 1;
				else if(c == '=') estado = 5;
				else if(c == '>') estado = 6;
				else if(c == '"') estado = 10;
				else if(isdigit(c) || (c=='-')) estado = 12;
				else if((isdigit(c) == false) && (((ispunct(c)) && (c=='_') ) || (ispunct(c)==false))) estado = 9;
				else
					{
					 estado = falhar();
					}
				break;

			case 1:
				cont_sim_lido++;
				c = code[cont_sim_lido];

				if(c == '=') estado = 2;
				else if(c == '>') estado = 3;
				else estado = 4;
				break;

			case 2:
				cont_sim_lido++;
				printf("<relop, LE>\n");
				token.nome_token = RELOP;
				token.atributo = LE;
				estado = 0;
				return(token);
				break;

			case 3:
				cont_sim_lido++;
				printf("<relop, NE>\n");
				token.nome_token = RELOP;
				token.atributo = NE;
				estado = 0;
				return(token);
				break;

			case 4:
				//cont_sim_lido++;
				printf("<relop, LT>\n");
				token.nome_token = RELOP;
				token.atributo = LT;
				estado = 0;
				return(token);
				break;

			case 5:
				cont_sim_lido++;
				printf("<relop, EQ>\n");
				token.nome_token = RELOP;
				token.atributo = EQ;
				estado = 0;
				return(token);
				break;

			case 6:
				cont_sim_lido++;
				c = code[cont_sim_lido];
				if(c == '=') estado = 7;
				else estado = 8;
				break;

			case 7:
				cont_sim_lido++;
				printf("<relop, GE>\n");
				token.nome_token = RELOP;
				token.atributo = GE;
				estado = 0;
				return(token);
				break;

			case 8:
				//cont_sim_lido++;
				printf("<relop, GT>\n");
				token.nome_token = RELOP;
				token.atributo = GT;
				//ATENÇÃO - CORREÇÃO: foi acrescentado o comando "estado = 0;"
				estado = 0;
				return(token);
				break;

			
			case 9: // identificador
				c = code[cont_sim_lido];
				char palavra[50];
				int i = 0;
				
				while (isalpha(code[cont_sim_lido]) || isdigit(code[cont_sim_lido]) ) {
					if(isspace(code[cont_sim_lido])){
						cont_sim_lido++;
						break;
					}
					palavra[i++] = code[cont_sim_lido];
					cont_sim_lido++;
				}
				palavra[i] = '\0';

				
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

                else if (strcmp(palavra, "string")){ 
					token.nome_token = STRING_TYPE;
					printf("<STRING>\n");
				}
                else if (strcmp(palavra, "while")) {
					token.nome_token = WHILE;
					printf("<WHILE>\n");
				}
                else if (strcmp(palavra, "read")) {
					token.nome_token = READ;
					printf("<READ>\n");
				}
                else if (strcmp(palavra, "print")) {
					token.nome_token = PRINT;
					printf("<PRINT>\n");
				}
				else {
					count_id++;
					printf("<id, %s>\n", count_id);
					token.nome_token = ID;
				}

				token.atributo = 0;
				estado = 0;
				return token;
					
					/*implementar ações referentes aos estado */
					
				

				break;

			case 10: // strings
				char * string_texto;
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
				
				printf("<STRING_TYPE, {%s}>\n", string_texto);
				estado = 0;
				return token;
				break;


				/*implementar ações para os estados 10, 11, 12*/

			case 12: // números inteiros e float
				

				// a ser implementado
				

				/*implementar ações para os estados 13-19*/

			case 20:
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
				break;

				/*implementar ações para os estados 21-24*/

			case 25:
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
					token.nome_token = -1;
					token.atributo = -1;
					return(token);
				}
				break;
		}

	}
	token.nome_token = EOF;
	token.atributo = -1;
	return(token);
}




int main ()
{
	Token token;
    code = readFile("programa.txt");
    token = proximo_token();
    token = proximo_token();


}
