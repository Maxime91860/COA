#include <gcc-plugin.h>                                                          
#include <plugin-version.h>                                                      
#include <tree.h>                                                                
#include <basic-block.h>                                                         
#include <gimple.h>                                                              
#include <tree-pass.h>                                                           
#include <context.h>                                                             
#include <function.h>                                                            
#include <gimple-iterator.h>                                                     
#include <input.h>
#include <c-family/c-pragma.h>
#include <vec.h>


#define FUNC_TABLE_SIZE 256
static vec<char *> list_of_functions;
//static vec<char *> list_of_functions;


/* Global variable required for plugin to execute */
int plugin_is_GPL_compatible;



/******************************/
/****   TD6 - QUESTION 1   ****/
/******************************/



static void handle_pragma_print(cpp_reader *ARG_UNUSED(dummy))
{

	/******************************/
	/****   TD6 - QUESTION 2   ****/
	/******************************/


	enum cpp_ttype token;
	tree x;	
	token = pragma_lex (&x);

	if(token == CPP_NAME)
	{

		const char *op = IDENTIFIER_POINTER (x);	
		printf("USE PRAGMA WITH -- %s -- AS ARGUMENT\n", op);
	}

	/******************************/
	/**   TD6 - FIN QUESTION 2   **/
	/******************************/

}



static int print_vname() {
        char *name;
        unsigned ix;

	if (list_of_functions.length()) {
        	FOR_EACH_VEC_ELT_REVERSE (list_of_functions, ix, name)
        	{
			printf(" %s is an argument of #pragma instrumente function\n", name);;
        	}
	}

	return 0;
}



/******************************/
/****   TD6 - QUESTION 3   ****/
/******************************/

	static void
handle_pragma_function(cpp_reader *ARG_UNUSED(dummy))
{
	enum cpp_ttype token;
	tree x;
	bool close_paren_needed_p = false;



	/******************************/
	/****   TD6 - QUESTION 4   ****/
	/******************************/


	/* check if the pragma is not inserted in a function*/
  if (cfun)
    {
      printf ("#pragma instrumente function is not allowed inside functions\n");
      return;
    }

        /******************************/
        /**   TD6 - FIN QUESTION 4   **/
        /******************************/



	/* check if pragma argument begins with a parenthesis*/
	token = pragma_lex (&x);
	if (token == CPP_OPEN_PAREN)
	{
		close_paren_needed_p = true;
		token = pragma_lex (&x);
	}

	/* if first token of argument is not an openning parenthesis nor a name, then its invalid*/
	if (token != CPP_NAME)
	{
		printf ("\n /!\\#pragma instrument fonction argument is not valid!\n Valid formats:\n ---> #pragma instrumente function NAME\n or\n ---> #pragma instrumente function (NAME1,NAME2,...)\n\n");
		return;
	}
	else
	{
		int cpt = 0;

		while (token == CPP_NAME)
		{
			cpt++;
			const char *op = IDENTIFIER_POINTER (x);
		//	printf(" -------- argument %d of pragma is : %s\n", cpt, op);  
			

	/******************************/
	/****   TD6 - QUESTION 5   ****/
	/******************************/

			list_of_functions.safe_push ( (char*)op);

	 /******************************/
        /**   TD6 - FIN QUESTION 5   **/
        /******************************/


			token = pragma_lex (&x);
			/* checking for commas only if there is an opening parenthesis*/
			while (token == CPP_COMMA && close_paren_needed_p)
			{
				token = pragma_lex (&x);
			}
		}

		/* if there is an opening parenthesis, there must be a closing one*/
		if (close_paren_needed_p)
		{
			if (token == CPP_CLOSE_PAREN)
				token = pragma_lex (&x);
			else
				printf ("pragma is badly shaped. Does not have a needed closing parenthesis!\n");
		}
		/* if there is something after the name or the closing parenthesis, the arguemnt is badly shaped*/
		if (token != CPP_EOF)
		{
			printf ("\n /!\\#pragma GCC target string... is badly formed! Valid formats:\n ---> #pragma instrumente function NAME\n or\n ---> #pragma instrumente function (NAME1,NAME2,...)\n\n");
			return;
		}

	}

	print_vname();

}
/******************************/
/**   TD6 - FIN QUESTION 3   **/
/******************************/




static void register_my_pragma (void *event_data, void *data) {
	c_register_pragma ("instrumente", "function", handle_pragma_function);
}



/* Main entry point for plugin */
	int 
plugin_init(struct plugin_name_args * plugin_info, 
		struct plugin_gcc_version * version)
{

	printf( "plugin_init: Entering...\n" ) ;

	/* First check that the current version of GCC is the right one */

	if(!plugin_default_version_check(version, &gcc_version)) 
		return 1;

	printf( "plugin_init: Check ok...\n" ) ;


	register_callback (plugin_info->base_name, PLUGIN_PRAGMAS,
			register_my_pragma, NULL);


	return 0;
}


/******************************/
/**   TD6 - FIN QUESTION 1   **/
/******************************/


