#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>
#include<stdlib.h>
#include "monitor/elf.h"

enum {
	NOTYPE = 256, TK_NUM,TK_REG,NE,AND,OR,EQ,
	TK_IDENT
	/* TODO: Add more token types */

};

static struct rule {
	char *regex;
	int token_type;
} rules[] = {

	/* TODO: Add more rules.
	 * Pay attention to the precedence level of different rules.
	 */

	{" +",	NOTYPE},
	{"\\$[a-z]+",TK_REG},
	{"0[xX][0-9a-fA-F]+",TK_NUM},
	{"[0-9]+",TK_NUM},
	{"[a-zA-Z_][a-zA-Z0-9_]*",TK_IDENT},
	{"==",EQ},
	{"!=",NE},
	{"&&",AND},	
	{"\\|\\|",OR},
	{"!",'!'},		// spaces
	{"\\+", '+'},
	{"\\-",'-'},
	{"\\*",'*'},
	{"\\/",'/'},
	{"\\(",'('},
	{"\\)",')'},					// plus
	{"==", EQ}						// equal
};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )

static regex_t re[NR_REGEX];

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
	int i;
	char error_msg[128];
	int ret;

	for(i = 0; i < NR_REGEX; i ++) {
		ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
		if(ret != 0) {
			regerror(ret, &re[i], error_msg, 128);
			Assert(ret == 0, "regex compilation failed: %s\n%s", error_msg, rules[i].regex);
		}
	}
}

typedef struct token {
	int type;
	char str[32];
} Token;

Token tokens[32];
int nr_token;

static bool make_token(char *e) {
	int position = 0;
	int i;
	regmatch_t pmatch;
	
	nr_token = 0;

	while(e[position] != '\0') {
		/* Try all rules one by one. */
		for(i = 0; i < NR_REGEX; i ++) {
			if(regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
				char *substr_start = e + position;
				int substr_len = pmatch.rm_eo;

				if(substr_len>32) printf("token exceeds length limit");

				Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s", i, rules[i].regex, position, substr_len, substr_len, substr_start);
				position += substr_len;

				/* TODO: Now a new token is recognized with rules[i]. Add codes
				 * to record the token in the array `tokens'. For certain types
				 * of tokens, some extra actions should be performed.
				 */
				switch(rules[i].token_type) {
					case NOTYPE:break;
					default:
					{
						Token token;
						token.type=rules[i].token_type;
						int j;
						for(j=0;j<substr_len;j++){
							token.str[j]=substr_start[j];
						}
						token.str[substr_len]='\0';
						if(nr_token>=32){
							printf("too many tokens");
							return false;
						}
						tokens[nr_token]=token;
						nr_token++;
						break;
					}
				}
				break;
			}
		}

		if(i == NR_REGEX) {
			printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
			return false;
		}
	}

	return true; 
}

bool check_parentheses(int p,int q){
	int left_parenthesis=0;
	if(tokens[p].type!='('||tokens[q].type!=')'){
		return false;
	}
	else{
		int i;
		for(i=p+1;i<q;i++){
			if(tokens[i].type=='(') left_parenthesis++;
			else if(tokens[i].type==')'){
				if(left_parenthesis==0) return false;
				else left_parenthesis--;
			}
			else continue;
		}
	}
	return left_parenthesis==0;
}

int eval(int p,int q){
	if(p>q){
		printf("Wrong postion in eval\n");
		assert(0);
	}
	else if(p==q){
		if(tokens[p].type==TK_REG){
			char *sub=tokens[p].str+1;
			if(strcmp(sub,"eip")==0) return cpu.eip;
			else{
				int i;
				for(i=R_EAX;i<=R_EDI;i++){
					if(strcmp(sub,regsl[i])==0){
						return reg_l(i);
					}
				}
			}
			printf("Unknown register: %s\n",sub);
			assert(0);
		}
		else if(tokens[p].type==TK_IDENT){
			return lookup_symbol(tokens[p].str);
		}
		return (int)strtol(tokens[p].str,NULL,0);
	}
	else if(check_parentheses(p,q)==true){
		return eval(p+1,q-1);
	}
	else{
		int op=-1,op_type=-1;
		int priority=-1; //'+''-':5,'*''/':4 ,'==''!=':3,'&&'=2,'||'=1
		int paren=0;
		int i;
		for(i=p;i<=q;i++){
			if(tokens[i].type==TK_NUM) continue;
			if(tokens[i].type=='(') paren++;
			if(tokens[i].type==')') paren--;
			if(paren>0) continue;
			else{
				if((tokens[i].type=='+'||tokens[i].type=='-')&&i>p&&
					(tokens[i-1].type==TK_REG||tokens[i-1].type==TK_NUM||tokens[i-1].type==')')){
					op=i;
					op_type=tokens[i].type;
					priority=5;
				}
				else if((tokens[i].type=='*'||tokens[i].type=='/')&&(priority<=4)&&i>p&&
					(tokens[i-1].type==TK_REG||tokens[i-1].type==TK_NUM||tokens[i-1].type==')')){
					op=i;
					op_type=tokens[i].type;
					priority=4;
				}
				else if((tokens[i].type==EQ||tokens[i].type==NE)&&priority<=3){
					op=i;
					op_type=tokens[i].type;
					priority=3;
				}
				else if((tokens[i].type==AND)&&priority<=2){
					op=i;
					op_type=tokens[i].type;
					priority=2;
				}
				else if((tokens[i].type==OR)&&priority<=1){
					op=i;
					op_type=tokens[i].type;
					priority=1;
				}
			}
		}
		if(op==-1) {
			if(p<q){
				switch (tokens[p].type)
				{
				case '-':
					return -eval(p+1,q);
					break;
				case '*':
					return swaddr_read(eval(p+1,q),4);
					break;
				case '!':
					return !eval(p+1,q);
					break;
				default:
					break;
				}
			}
			
		}
		int val1=eval(p,op-1);
		int val2=eval(op+1,q);

		switch (op_type)
		{
		case '+':
			return val1+val2;
			break;
		case '-':
			return val1-val2;
			break;;
		case '*':
			return val1*val2;
			break;
		case '/':
			return val1/val2;
			break;
		case EQ:
			return val1==val2;
			break;
		case NE:
			return val1!=val2;
			break;
		case AND:
			return val1&&val2;
			break;
		case OR:
			return val1||val2;
			break;
		default:
			assert(0);
		}
	}
}

uint32_t expr(char *e, bool *success) {
	if(!make_token(e)) {
		*success = false;
		return 0;
	}
	*success=true;
	return (uint32_t)eval(0,nr_token-1);
	
	/* TODO: Insert codes to evaluate the expression. */
}

