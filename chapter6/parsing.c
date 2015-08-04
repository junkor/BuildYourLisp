#include <stdlib.h>
#include <stdio.h>

#include <editline/readline.h>
//#include <editline/history.h>

#include "mpc.h"

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
	puts("Lispy Version 0.0.0.0.2");
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
		
			/* On success print and delete the AST */
			mpc_ast_print(r.output);
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

	return 0;

}
