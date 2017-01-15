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


/******************************/
/****   TD6 - QUESTION 3   ****/
/******************************/

	static void
handle_pragma_function(cpp_reader *ARG_UNUSED(dummy))
{
	enum cpp_ttype token;
	tree x;
	bool close_paren_needed_p = false;

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
		printf ("#pragma instrument fonction argument is not valid");
		return;
	}
	else
	{
		int cpt = 0;

		while (token == CPP_NAME)
		{
			cpt++;
			const char *op = IDENTIFIER_POINTER (x);
			printf(" -------- argument %d of pragma is : %s\n", cpt, op);  

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
			printf ("#pragma GCC target string... is badly formed\n");
			return;
		}

	}
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


