#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/*spec token type*/

#define GREATER		1
#define GEQUAL		2
#define EQUAL		3
#define ASSIGN 		4
#define SINUS		5
#define NUMBER 		6
#define SYM		7
#define HELP		8
#define COS		9
#define PLUSEQUAL	10
#define MINUSEQUAL	11
#define KEYWORD		12
#define QUIT 		'q'
#define PRINT		';'

struct token{
	int key;
	char val[256];
};

struct token tokens[1000];
int ntok = 0;


int getnum(char *s)
{
	int ret;
	double d;
	ret = scanf("%lf",&d);
	sprintf(s,"%f",d);
	return ret;
}

int getop(char *s)
{
	int c,i;
	while((c = getchar()) == ' ' || c == '\t')
		;
	if(!isdigit(c) && !isalpha(c))
		return c;
	ungetc(c,stdin);
	if(isdigit(c)){
		if(getnum(s))
			return NUMBER;
		else
			exit(1);
	}
	i = 0;
	for(;;){
		if(!isalnum(s[i++] = getchar())){
			ungetc(s[i-1],stdin);
			s[i-1] = '\0';
			return KEYWORD;
		}
	}
}

void pushback(struct token tok)
{
	tokens[ntok++] = tok;
}


int getkeyword(char *s)
{
	if(strcmp("exit",s) == 0)
		return QUIT;
	if(strcmp("HELP",s) == 0)
		return HELP;
	if(strcmp("cos",s) == 0)
		return COS;
	if(strcmp("sin",s) == 0)
		return SINUS;
	return SYM;
}

int get_special(int c)
{
	int c2;
	if(c == EOF || c == QUIT)
		return QUIT;
	if(c == '\n')
		return PRINT;
	switch(c){
		case '(':
		case ')': 
		case PRINT:
		case '*':  
		case '/':
		case '^':
			return c;
		case '>':
			switch(c2 = getchar()){
				case '=':
					return GEQUAL;
				default:
					ungetc(c2,stdin);
					return GREATER;
			}
			break;
		case '=':
			switch(c2 = getchar()){
				case '=':
					return EQUAL;
					break;
				default:
					ungetc(c2,stdin);
					return ASSIGN;
			}
			break;
		case '+':
			switch(c2 = getchar()){
				case '=':
					return PLUSEQUAL;
					break;
				default:
					ungetc(c2,stdin);
					return '+';
			}
			break;
		case '-':
			switch(c2 = getchar()){
				case '=':
					return MINUSEQUAL;
				default:
					ungetc(c2,stdin);
					return '-';
			}
			break;
		default:
			exit(1);
	}
}


struct token get_token()
{
	if(ntok > 0)
		return tokens[--ntok];
	struct token ret;
	ret.key = getop(ret.val);
	if(ret.key == NUMBER)
		return ret;
	//get specefic
	if(ret.key == KEYWORD)
		ret.key = getkeyword(ret.val);
	else
		ret.key = get_special(ret.key);
	return ret;
}



