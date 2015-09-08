#include <stdlib.h>
#include <stdio.h>

#include <editline/readline.h>
//#include <editline/history.h>

#include "mpc.h"

/* Create Enumeration of Possible Error Types  */
enum { LERR_DIV_ZERO, LERR_BAD_OP, LERR_BAD_NUM };

/* Create Enumeration of Possible lval types */
enum { LVAL_NUM, LVAL_ERR };

/* Declare New lval Struct */
typedef struct {
    long num;
    int  type;
    int  err;
}lval;

/* Create a new number type lval */
lval lval_num(long x){
    lval v;
    v.type = LVAL_NUM;
    v.num = x;
    return v;
}

/* Create a new error type lval */
lval lval_err(int x){
    lval v;
    v.type = LVAL_ERR;
    v.err = x;
    return v;
}

/* Print an "lval" */
void lval_print(lval v){
    switch(v.type){
        /* In the case the type is a number print it  */
        /* Then 'break' out of the switch */
        case LVAL_NUM:
            printf("%li",v.num);
        break;

        /* In the case the type is an error */
        case LVAL_ERR:
            /* Check what type of error it is and print it */
            if(v.err == LERR_DIV_ZERO){
                printf("Error: Division By ZERO!");
            }
            if(v.err == LERR_BAD_OP){
                printf("Error: Invalid Operator!");
            }
            if(v.err == LERR_BAD_NUM){
                printf("Error: Invalid Number!");
            }
        break;
    }
}

/* Print an "lval" followed by a newling */
void lval_println(lval v){
    lval_print(v);
    putchar('\n');
}

/* Use operator string to see which operation to perform */
lval eval_op(lval x, char* op, lval y){
	
    /* If either values is an error return it */
    if(x.type == LVAL_ERR){ return x; }
    if(y.type == LVAL_ERR){ return y; }

    if(strcmp(op,"+")==0){	return lval_num(x.num + y.num);	}
	if(strcmp(op,"-")==0){  return lval_num(x.num - y.num); }
	if(strcmp(op,"*")==0){  return lval_num(x.num * y.num); }
	if(strcmp(op,"/")==0){
    
        /* If second operand is zero return error */
        return y.num == 0 ? lval_err(LERR_DIV_ZERO):lval_num(x.num/y.num);
     }
	return lval_err(LERR_BAD_OP);
}


lval eval(mpc_ast_t* t){

	if(strstr(t->tag,"number")){
	    /* check if there is some error in conversion */
        errno = 0;
        long x = strtol(t->contents,NULL,10);
    	return errno != ERANGE ? lval_num(x):lval_err(LERR_BAD_NUM);
	}

	/* the operator is always second child */
	char* op = t->children[1]->contents;
	lval x = eval(t->children[2]);

	/* Iterate the remaining children and combining */
	int i = 3;
	while(strstr(t->children[i]->tag,"expr")){
		x = eval_op(x,op,eval(t->children[i]));
		i++;
	}
	return x;
}


int main(int argc, char** argv){

	/* Create Some Parsers */
	mpc_parser_t* number 	= mpc_new("number");
	mpc_parser_t* operator 	= mpc_new("operator");
	mpc_parser_t* expr		= mpc_new("expr");
	mpc_parser_t* lispy		= mpc_new("lispy");

	/* Define them with the following Language */
	mpca_lang(MPCA_LANG_DEFAULT,
			"													\
				number	:	/-?[0-9]+/;							\
				operator:	'+'|'-'|'*'|'/';					\
				expr	:	<number> |'('<operator><expr>+')';	\
				lispy	:	/^/ <operator> <expr>+ /$/;			\
			",
			number,operator,expr,lispy
			);

	/* Print Version and Exit Information */
	puts("Lispy Version 0.0.0.0.3");
	puts("Press Ctrl_c to Exit\n");
	
	/* In a never ending loop */
	while(1){

		/* Output our prompt and get input*/
		char * input = readline("lispy>");

		/* add input to history*/
		add_history(input);

		/* Attempt to parse the user input */
		mpc_result_t r;
		if (mpc_parse("<stdin>",input,lispy,&r)){
		
			lval result = eval(r.output);
			lval_println(result);
            mpc_ast_delete(r.output);
		}else{
		
			/* otherwise print and delete the error */
			mpc_err_print(r.error);
			mpc_err_delete(r.error);
		}

		/* Echo input bace to user */
		// printf("No you're a %s",input);

		free(input);
	}

	mpc_cleanup(4,number,operator,expr,lispy);

	return 0;
}
