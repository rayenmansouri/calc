/*read input stream into token*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>

#define MAXTOK 		100
#define MAXLEN		10  //max symbol length
#define MAXDIGIT	100 //max number size

/*token type */

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
#define QUIT 		'q'
#define PRINT 		';'


struct token{
	int key;
	char* val;
};

struct token tokbuf[MAXTOK];

struct token keywords[] = {
	{QUIT,"exit"},
	{HELP,"help"},
	{SINUS,"sin"},
	{COS,"cos"}
};

int nkeys 	= sizeof(keywords) / sizeof(struct token);

int ntok 	= 0; //token buff

struct token *
getkeyword(char *val)
{
	int i;
	for(i = 0; i < nkeys;i++){
		if(strcmp(keywords[i].val,val) == 0)
			return &keywords[i];
	}
	return NULL;
}


void pushback(struct token tok)
{
	tokbuf[ntok++] = tok;
}

struct token
get_token(void)
{
	if(ntok > 0)
		return tokbuf[--ntok];
	char w[MAXTOK];
	struct token tok;
	while((*w = getchar()) == ' ' || *w == '\t')
		;
	if(*w == EOF || *w  == QUIT){  
	 	tok.key = QUIT;
		return tok;
	}
	if(*w == '\n') {
		tok.key = PRINT;
		return tok;
	}
	if(isdigit(*w)){ 
		int len;	
		double d;
		ungetc(*w,stdin);
		scanf("%lf",&d);
		sprintf(w,"%lf",d);
		len = strlen(w);
		if(len > MAXDIGIT){
			fprintf(stderr,"very big number\n");
			exit(1);
		}
		tok.val = (char *) malloc(len);	
		strcpy(tok.val,w);
		tok.key = NUMBER;
		return tok;
	}
	switch(*w){
		case '(':
		case ')': 
		case PRINT:
		case '*':  
		case '/':
		case '^':
			tok.key = *w;
			break;
		case '>':
			switch(w[1] = getchar()){
				case '=':
					tok.key = GEQUAL;
				default:	
					ungetc(w[1],stdin);
					tok.key = GREATER;
			}
			break;
		case '=':
			switch(w[1] = getchar()){
				case '=':
					tok.key = EQUAL;
					break;
				default:
					ungetc(w[1],stdin);
					tok.key = ASSIGN;
			}
			break;
		case '+':
			switch(w[1] = getchar()){
				case '=':
					tok.key = PLUSEQUAL;
					break;
				default:
					ungetc(w[1],stdin);
					tok.key = '+';
			}
			break;
		case '-':
			switch(w[1] = getchar()){
				case '=':
					tok.key = MINUSEQUAL;
					break;
				default:
					ungetc(w[1],stdin);
					tok.key = '-';
			}
			break;

		default:	
			if(isalpha(*w)){
				tok.key = SYM;
				int i;
				for(i = 1;i < MAXLEN;i++)
					if(!isalnum(w[i] = getchar())){
						ungetc(w[i],stdin);
						w[i] = '\0';
						break;
					}
				if(i >= MAXLEN){
					fprintf(stderr,"token too long\n");
					exit(1);
				}
				tok.val = (char *) malloc(strlen(w));
				strcpy(tok.val,w);
				
				struct token *t2;
				if((t2 = getkeyword(tok.val)) != NULL){
					tok.key = t2->key;
				}	
			}else{
				fprintf(stderr,"unkown token %c\n",*w);
				exit(1);
			}
		
	}
	return tok;	
}
