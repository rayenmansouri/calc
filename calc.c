#include <stdarg.h>
#include "token.h"
#include "dict.h"

void usage(void);
void die(char *,...);
char *to_string(double);
double expression(void);
double term(void);
double primary(void);

int main(int argc,char *argv[])
{

	printf("C-style arbitrary precision calculator (version 0.0.1)\n" \
	       "Calc is open software. For license details type:  help copyright\n"\
	       "[Type '%c' to exit, or '%c' for print.]\n",QUIT,PRINT);
	
	double exp;
	struct token tok;
	struct nlist *s;
	install("pi","3.14");
	for(;;){
		tok = get_token();
		while(tok.key == PRINT)
			tok = get_token();
		
		switch(tok.key){
			case QUIT:
				return 0;
			case HELP:
				usage();
				break;
			case SYM:
				struct token t2 = get_token();
				if(t2.key == ASSIGN){	
					install(tok.val,to_string(expression()));
					break;
				}else if(t2.key == PLUSEQUAL){
					s = lookup(tok.val);
					if(s == NULL)
						die("'%s' is not defined\n",tok.val);	
					install(tok.val,to_string(expression() + atof(s->defn)));
					break;
				}else if(t2.key == MINUSEQUAL){
					s = lookup(tok.val);
					if(s == NULL)
						die("'%s' is not defined\n",tok.val);	
					install(tok.val,to_string(expression()  - atof(s->defn)));
					break;
	
				}
				pushback(t2); //fallback to expression;
			default:
				pushback(tok);	
				exp = expression();
				printf("%f\n",exp);

		}		
		
	}	
	return 0;	
}
void usage(void)
{
	printf("usage: type exit to quit or expression to evaluate\n");
}

void die(char *s,...)
{
	int c;
	va_list params;
	va_start(params,s);
	vfprintf(stderr,s,params);
	va_end(params);
	exit(1);	
}

char *to_string(double d)
{
	char *w = (char *) malloc(MAXDIGIT);
	sprintf(w,"%f",d);
	return w;
}

double expression(void)
{
	double t;
	struct token op;
	t = term();	
	for(;;){
		op = get_token();
		switch(op.key){
			case '+':
				t+=term();
				break;
			case '-':
				t-=term();
				break;
			case GREATER:
				t = (int)t > (int)term();
				break;
			case GEQUAL:
				t = (int)t >= (int)term();
				break;
			case EQUAL:
				t = (int)t == (int)term();
				break;
			default:
				pushback(op);
				return t;
		}
		
	}	
}

double term(void)
{
	double p1,p2;
	struct token op;
	p1 = primary();
	for(;;){
		op = get_token();
		switch(op.key){
			case '*':	
				p1*=primary();
				break;
			case '/':
				p2 = primary();
				if(p2 == 0){
					die("zero division error\n");
				}
				p1/=p2;
				break;
			case '^':
				return pow(p1,primary());
			default:
				pushback(op);	
				return p1;
		}
	}	
}

double primary(void)
{
	struct token tok = get_token();
	double d;
	switch(tok.key){
		case '(':
			d = expression();
			tok = get_token();
			if(tok.key != ')'){	
				die("unbalanced paranthese %c\n",tok.key);
			}
			else return d;
		case NUMBER:
			return atof(tok.val);
		case SYM:
			struct nlist *s;
			s = lookup(tok.val);
			if(s == NULL)
				die("'%s' is not defined\n",tok.val);	
			return atof(s->defn);
		case SINUS:
			tok = get_token();
			if(tok.key != '(')
				die("'(' expected afer sinus\n");
			d = expression();
			tok = get_token();
			if(tok.key != ')')
				die("unbalanced parenthese\n");
			return sin(d);	

		case COS:
			tok = get_token();
			if(tok.key != '(')
				die("'(' expected afer sinus\n");
			d = expression();
			tok = get_token();
			if(tok.key != ')')
				die("unbalanced parenthese\n");
			return cos(d);	

		case '-':
			return -primary();
		default:
			die("Syntax Error\n",tok.key);	
	}
}

