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
#define NUMBER 		6
#define SYM		7
#define HELP		8
#define PLUSEQUAL	10
#define MINUSEQUAL	11
#define KEYWORD		12
#define LESS		13
#define LESSEQUAL	14
#define LET		15
#define IF		16
#define QUIT 		'q'
#define PRINT		';'

/*260 < builtin token < 300*/

#define COS 	260
#define SIN	261
#define SQR	262
#define SQRT	263


#define IS_BUILT_IN(x) ((x) >= 260 && (x) <= 300)


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
		getnum(s);
		return NUMBER;
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
	if(strcmp("help",s) == 0)
		return HELP;
	if(strcmp("cos",s) == 0)
		return COS;
	if(strcmp("sqrt",s) == 0)
		return SQRT;
	if(strcmp("sqr",s) == 0)
		return SQR;
	if(strcmp("sin",s) == 0)
		return SIN;
	if(strcmp("q",s) == 0)
		return QUIT;
	if(strcmp("let",s) == 0)
		return LET;
	if(strcmp("LET",s) == 0)
		return LET;
	if(strcmp("IF",s) == 0)
		return IF;
	if(strcmp("if",s) == 0)
		return IF;
	
	return SYM;
}

int get_special(int c)
{
	int c2;
	if(c == EOF)
		return QUIT;
	if(c == '\n')
		return PRINT;
	switch(c){
		case '(':
		case ')': 
		case '*':  
		case '/':
		case '^':
		case '{':
		case '}':
			return c;
		case '\n':
		case ';':
			return PRINT;
		case '>':
			switch(c2 = getchar()){
				case '=':
					return GEQUAL;
				default:
					ungetc(c2,stdin);
					return GREATER;
			}
			break;
		case '<':
			switch(c2 = getchar()){
				case '=':
					return LESSEQUAL;
				default:
					ungetc(c2,stdin);
					return LESS;
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
		
	struct token ret;
	if(ntok > 0)
		return tokens[--ntok];	
	
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



