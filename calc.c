#include <stdarg.h>
#include "token.h"
#include "dict.h"

int error_sig = 0;
char error_text[280] = "";

void usage(void);
void die(char *,...);
void clean_up();
void calc(void);
double compound_stmt(void);
double block(void);
double declaration(void);
double stmt(void);
double builtin(int);
double expression(void);
double term(void);
double primary(void);
char *to_string(double);

int main(int argc,char *argv[])
{

	install("pi","3.141592654");
	install("e","2.718281828");
	calc();	
	return 0;	
}

void usage(void)
{
	printf("C-style arbitrary precision calculator (version 0.0.1)\n" \
               "Calc is open software. For license details type:  help copyright\n"\
               "[Type '%c' to exit, or '%c' for print.]\n",QUIT,PRINT);
	
}

void die(char *s,...)
{
	va_list params;
	va_start(params,s);
	vfprintf(stderr,s,params);
	va_end(params);
	exit(1);	
}


char *to_string(double d)
{
	char *w = (char *) malloc(255);
	sprintf(w,"%f",d);
	return w;
}

/*calculator main loop*/
void calc(void)
{	
	double ret;
	struct token tok;
	for(;;){
		printf("> ");
		tok = get_token();
		switch(tok.key){
			case QUIT:
				exit(0);
			case HELP:
				usage();
				clean_up();
				break;
			case PRINT:
				break;
			default:
				pushback(tok);	
				ret = compound_stmt();
				if(!error_sig)
			        	printf("%f\n",ret);
				clean_up();
		}	                
	}
}


void clean_up()
{
	struct token tok;
	printf("%s",error_text);		
	tok = get_token();
	while(tok.key != PRINT)
		tok = get_token();
	error_sig = 0;
	error_text[0] = '\0';
}


double compound_stmt(void)
{
	double ret,exp;
	struct token tok;
	tok = get_token();
	switch(tok.key){
		case IF:
			exp = expression();
			if(error_sig)
				return 0;
			if(exp)
				return block();
		
			return 0;
		default:
			pushback(tok);
			return stmt();
	}
}

double stmt(void)
{
	double exp;
	struct token tok;
	struct nlist *s;
	tok = get_token();
	switch(tok.key){
		case LET: 
			exp =  declaration();	
			break;
		default:
			pushback(tok); /*fall back to expression*/
			exp = expression();
			break;
	}	
	if(error_sig)
				return 0;
	tok = get_token();
	if(tok.key != PRINT){
		sprintf(error_text,"new line is expected\n");
		error_sig = 1;
		return 0;
	}
	return exp;
}

double block(void)
{
	struct token tok;
	double ret;
	tok = get_token();
	switch(tok.key){
		case '{':
			for(;;){
				ret = compound_stmt();
				if(error_sig)
					return 0;
				tok = get_token();
				if(tok.key == '}')
					break;
				pushback(tok);
			}
			return ret;
		default:
			return compound_stmt();
	}
}

double declaration()
{
	struct nlist *s;
	struct token tok,tok2;
	double exp;
	tok = get_token();
	if(tok.key != SYM){
		sprintf(error_text,"symbol is expected\n");
		error_sig = 1;
		return 0;
	}
	if((s = lookup(tok.val)) != NULL){
		sprintf(error_text,"already declared\n");
		error_sig = 1;
		return 0;
	}
	tok2 = get_token();
	exp = expression();
	switch(tok2.key){
		case ASSIGN:
			install(tok.val,to_string(exp));
			return exp;
		case PLUSEQUAL:
		case MINUSEQUAL:
			if((s = lookup(tok.val)) == NULL){
				error_sig = 1;
				sprintf(error_text,"id: '%s' is undefined\n",tok.val);
				return 0;
			}
			exp = tok2.key == PLUSEQUAL ? exp + atof(s->defn) : atof(s->defn) - exp;
			install(tok.val,to_string(exp));
			return exp;
		default:
			/**/
			sprintf(error_text,"'%s' is not an assignment operator",tok2.val);
			error_sig = 1;
			return 0;
	
	}
	return 0;
}



double expression(void)
{
	double t;
	struct token op;
	struct nlist *s;
	t = term();	
	for(;;){
		if(error_sig)
			return 0;
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
			case LESS:
				t = (int)t < (int)term();
				break;
			case LESSEQUAL:
				t = (int)t <= (int)term();
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
		if(error_sig)
			return 0;
		op = get_token();
		switch(op.key){
			case '*':	
				p1*=primary();
				break;
			case '/':
				p2 = primary();
				if(p2 == 0){
					sprintf(error_text,"zero division error\n");
					error_sig = 1;
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
	struct nlist *s;
	double d;
	switch(tok.key){
		case '(':
			d = expression();
			tok = get_token();
			if(tok.key != ')'){	
				sprintf(error_text,"unbalanced paranthese %c\n",tok.key);
				error_sig = 1;
				return 0;
			}
			else return d;
		case NUMBER:
			return atof(tok.val);
		case SYM:
			if((s = lookup(tok.val)) == NULL){
				sprintf(error_text,"'%s' is not defined\n",tok.val);
				error_sig = 1;
				return 0;
			}
			return atof(s->defn);
		case '-':
			return -primary();
		case '{':
		case PRINT:
			pushback(tok);
			return 0;
		default:
			if(IS_BUILT_IN(tok.key))
				return builtin(tok.key);
			sprintf(error_text,"invalid syntax");
			error_sig = 1;
			return 0;
			
	}
	return 0;
}

double builtin(int key)
{
	struct token tok;
	double d;
	switch(key){
		case COS:	
		case SIN:
		case SQRT:
		case SQR:
			tok = get_token();
			if(tok.key != '('){
				error_sig = 1;
				sprintf(error_text,"expected ( after builtin fn");
				return 0;
			}
			d = expression();
			if(error_sig)
				return 0;
			tok = get_token();
			if(tok.key != ')'){
				sprintf(error_text,"unbalanced parens");
				error_sig = 1;
				return 0;
			}
			switch(key){
				case COS:
					return cos(d);
				case SIN:
					return sin(d);
				case SQRT:
					return sqrt(d);
				default:
					return d*d;
				
			}
		default:
			error_sig = 1;
			return 0;
	}					
}
