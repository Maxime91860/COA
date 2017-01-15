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
static vec<char *> tmp_list;


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
/****   TD6 - QUESTION 6   ****/
/******************************/
/* CHECK IF FUNCTION NAME WAS ALREADY PUSHED */
static int already_pushed(const char* str) {
        char *name;
        unsigned ix;

	if (list_of_functions.length()) {
        	FOR_EACH_VEC_ELT_REVERSE (list_of_functions, ix, name)
        	{
			if(strcmp (name, str) == 0) return 1;
        	}
	}

	return 0;
}
void insert_list(vec<char *> tmp_list)
{
        char *name;
        unsigned ix;

	if (tmp_list.length()) {
        	FOR_EACH_VEC_ELT_REVERSE (tmp_list, ix, name)
        	{
			if(!already_pushed(name))
			{
				list_of_functions.safe_push(name);
			}
			else
			{
				printf(" /!\\ %s has already been declared in a #pragma instrumente function\n");
			}
        	}
	}

}

/******************************/
/**   TD6 - FIN QUESTION 6   **/
/******************************/



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

	tmp_list.release();

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
	/****   TD6 - QUESTION 6   ****/
	/******************************/
			tmp_list.safe_push ( (char*)op);

	/******************************/
        /**   TD6 - FIN QUESTION 6   **/
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
			{
				token = pragma_lex (&x);
			}
			else
			{
				printf ("pragma is badly shaped. Does not have a needed closing parenthesis!\n\n\n\n");
				return;
			}
		}
		/* if there is something after the name or the closing parenthesis, the arguemnt is badly shaped*/
		if (token != CPP_EOF)
		{
			printf ("\n /!\\#pragma GCC target string... is badly formed! Valid formats:\n ---> #pragma instrumente function NAME\n or\n ---> #pragma instrumente function (NAME1,NAME2,...)\n\n");
			return;
		}

	}

	/******************************/
	/****   TD6 - QUESTION 6   ****/
	/******************************/
	insert_list(tmp_list);

	/******************************/
        /**   TD6 - FIN QUESTION 6   **/
        /******************************/
//	print_vname();

}
/******************************/
/**   TD6 - FIN QUESTION 3   **/
/******************************/




static void register_my_pragma (void *event_data, void *data) {
	c_register_pragma ("instrumente", "function", handle_pragma_function);
}


/******************************/
/****   TD6 - QUESTION 7   ****/
/******************************/

/* Global object (const) to represent my pass */
const pass_data my_pass_data =
{ 
	GIMPLE_PASS, /* type */ 
	"NEW_PASS", /* name */
	OPTGROUP_NONE, /* optinfo_flags */
	TV_OPTIMIZE, /* tv_id */
	0, /* properties_required */
	0, /* properties_provided */
	0, /* properties_destroyed */
	0, /* todo_flags_start */
	0, /* todo_flags_finish */
};                                                                                                                                     

/* My new pass inheriting from regular gimple pass */
class my_pass : public gimple_opt_pass
{
	public:
		my_pass (gcc::context *ctxt)
			: gimple_opt_pass (my_pass_data, ctxt)
		{}

		/* opt_pass methods: */

		my_pass *clone ()
		{
			return new my_pass(g);
		}

		/* Gate function (shall we apply this pass?) */
		bool gate (function *fun)
		{
			const char * fname = function_name(fun);
			return already_pushed(fname);
		}

		/* Execute function */
		unsigned int execute (function *fun)
		{

			const char * fname = function_name(fun);
            		printf("  PASS  in function %s\n", fname);

           			return 0;
		}


}; 

/******************************/
/**   TD6 - FIN QUESTION 7   **/
/******************************/




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


/******************************/
/****   TD6 - QUESTION 7   ****/
/******************************/
	struct register_pass_info my_pass_info;

	my_pass p(g);

	/* Fill info on my pass 
	 (insertion after the pass building the CFG) */
	my_pass_info.pass = &p;
	my_pass_info.reference_pass_name = "cfg";
	my_pass_info.ref_pass_instance_number = 0;
	my_pass_info.pos_op = PASS_POS_INSERT_AFTER;

	/* Add my pass to the pass manager */
	register_callback(plugin_info->base_name, 
			PLUGIN_PASS_MANAGER_SETUP, 
			NULL, 
			&my_pass_info);

	printf( "plugin_init: Pass added...\n" ) ;

/******************************/
/**   TD6 - FIN QUESTION 7   **/
/******************************/



	return 0;
}


/******************************/
/**   TD6 - FIN QUESTION 1   **/
/******************************/


