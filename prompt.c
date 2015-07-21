#include <stdlib.h>
#include <stdio.h>

#include <editline/readline.h>
#include <editline/history.h>

int main(int argc, char** argv){

	/* Print Version and Exit Information */
	puts("Lispy Version 0.0.0.0.1");
	puts("Press Ctrl_c to Exit\n");
	
	/* In a never ending loop */
	while(1){

		/* Output our prompt and get input*/
		char * input = readline("lispy>");

		/* add input to history*/
		add_history(input);

		/* Echo input bace to user */
		printf("No you're a %s",input);

		free(input);
	}

	return 0;

}
