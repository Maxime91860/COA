#include <gcc-plugin.h>                                                          
#include <plugin-version.h>                                                      
#include <tree.h>                                                                
#include <basic-block.h>                                                         
#include <gimple.h>                                                              
#include <tree-pass.h>                                                           
#include <context.h>                                                             
#include <function.h>                                                            
#include <gimple-iterator.h>                                                     


/* Global variable required for plugin to execute */
int plugin_is_GPL_compatible;


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




/* Enum to represent the collective operations */
enum mpi_collective_code {
#define DEFMPICOLLECTIVES( CODE, NAME ) CODE,
#include "MPI_collectives.def"
        LAST_AND_UNUSED_MPI_COLLECTIVE_CODE
#undef DEFMPICOLLECTIVES
} ;

/* Name of each MPI collective operations */
#define DEFMPICOLLECTIVES( CODE, NAME ) NAME,
const char *const mpi_collective_name[] = {
#include "MPI_collectives.def"
} ;
#undef DEFMPICOLLECTIVES

void td_isol_print(int td)
{
	printf("\n\n\n");
	printf("/****************************************************************************************************/\n");
	printf("/****************************************************************************************************/\n");
	printf("/*************************                       TD%d                        *************************/\n", td);
	printf("/****************************************************************************************************/\n");
	printf("/****************************************************************************************************/\n");
	printf("\n\n\n");

 
}


void function_isol_print(function *fun)
{
	printf("\n\n\n");
	printf("/************************************************************************************************************************/\n");
	printf("/************************************************************************************************************************/\n");
	printf("/***********************************            %s               ***********************************/\n", function_name(fun) );
	printf("/************************************************************************************************************************/\n");
	printf("/************************************************************************************************************************/\n");
	printf("\n\n\n");

 
}

/****************************************************************************************************/
/****************************************************************************************************/
/*************************                       TD3                        *************************/ 
/****************************************************************************************************/
/****************************************************************************************************/


void td3_q1_detect_MPI_CALL (gimple *stmt){
	tree t ;
	const char * callee_name ;

	t = gimple_call_fndecl( stmt ) ;
	callee_name = IDENTIFIER_POINTER(DECL_NAME(t));
	
	if(strncmp(callee_name,"MPI_",4) == 0){
		printf("          |||++|||      - ce gimple statement est un appel mpi\n");
	}
}

void td3_q2_detect_MPI_CALL (gimple *stmt){

	tree t ;
	const char * callee_name ;

	t = gimple_call_fndecl( stmt ) ;
	callee_name = IDENTIFIER_POINTER(DECL_NAME(t));

	int i;
	for(i=0; i<LAST_AND_UNUSED_MPI_COLLECTIVE_CODE; i++){
		if(strcmp(callee_name,mpi_collective_name[i]) == 0){
			printf("          |||++|||      - ce gimple statement est un appel mpi visé : %s\n",mpi_collective_name[i]);
		}
	}
}



/****************************************************************************************************/
/****************************************************************************************************/
/*************************                     FIN TD3                      *************************/ 
/****************************************************************************************************/
/****************************************************************************************************/


/****************************************************************************************************/
/****************************************************************************************************/
/*************************                       TD2                        *************************/ 
/****************************************************************************************************/
/****************************************************************************************************/




/******************************/
/**** TD2 - QUESTION 3 & 4 ****/
/******************************/

const char * td2_q3_q4_print_func_name(function * fun)
{
	//            const char * fname = fndecl_name(fun->decl);
	const char * fname = function_name(fun);
	//            const char * fname = current_function_name();
	printf("\t ... in function %s\n", fname);

	return fname;
}

/******************************/
/** TD2 - FIN QUESTION 3 & 4 **/
/******************************/


/******************************/
/****   TD2 - QUESTION 8   ****/
/******************************/

void td2_q8_print_called_functions( basic_block bb )
{
	gimple_stmt_iterator gsi;

	/* Iterate on gimple statements in the current basic block */
	for (gsi = gsi_start_bb (bb); !gsi_end_p (gsi); gsi_next (&gsi))
	{
		/* Get the current statement */
		gimple *stmt = gsi_stmt (gsi);

		if (is_gimple_call (stmt))
		{
			tree t ;
			const char * callee_name ;

			t = gimple_call_fndecl( stmt ) ;
			callee_name = IDENTIFIER_POINTER(DECL_NAME(t)) ;


			printf("          |||++|||      - gimple statement is a function call: function called is \" %s \" \n", callee_name);
			td3_q1_detect_MPI_CALL(stmt);
			td3_q2_detect_MPI_CALL(stmt);
		}

		
	}
}
/******************************/
/**   TD2 - FIN QUESTION 8   **/
/******************************/




/******************************/
/**** TD2 - QUESTION 5 & 6 ****/
/******************************/

void td2_q5_q6_print_blocks(function * fun)
{
	basic_block bb;
	gimple_stmt_iterator gsi;
	gimple *stmt;

	FOR_EACH_BB_FN(bb,fun)
	{
		gsi = gsi_start_bb(bb);
		stmt = gsi_stmt(gsi);
		printf("          |||++||| BLOCK INDEX %d : LINE %d\n", bb->index, gimple_lineno(stmt));

		td2_q8_print_called_functions(bb);

	}
}

/******************************/
/** TD2 - FIN QUESTION 5 & 6 **/
/******************************/





/****************************************************************************************************/
/****************************************************************************************************/
/*************************                     FIN TD2                      *************************/ 
/****************************************************************************************************/
/****************************************************************************************************/




/****************************************************************************************************/
/****************************************************************************************************/
/*************************                PLUGIN GRAPHVIZ                   *************************/ 
/****************************************************************************************************/
/****************************************************************************************************/




/* Build a filename (as a string) based on function name */
static char * cfgviz_generate_filename( function * fun, const char * suffix )
{
	char * target_filename ; 

	target_filename = (char *)xmalloc( 1024 * sizeof( char ) ) ;

	snprintf( target_filename, 1024, "%s_%s_%d_%s.dot", 
			current_function_name(),
			LOCATION_FILE( fun->function_start_locus ),
			LOCATION_LINE( fun->function_start_locus ),
			suffix ) ;

	return target_filename ;
}

/* Dump the graphviz representation of function 'fun' in file 'out' */
static void cfgviz_internal_dump( function * fun, FILE * out, int td ) 
{
	basic_block bb; 

	// Print the header line and open the main graph
	fprintf(out, "Digraph G{\n");



	/******************************/
	/****   TD2 - QUESTION 7   ****/
	/******************************/



	FOR_ALL_BB_FN(bb,cfun)
	{

		//
		// Print the basic block BB, with the MPI call if necessary
		//

		fprintf( out,
				"%d [label=\"BB %d\" shape=ellipse]\n",
				bb->index,
				bb->index
		       ) ;

		//
		// Process output edges 
		//
		edge_iterator eit;
		edge e;

		FOR_EACH_EDGE( e, eit, bb->succs )
		{
			const char *label = "";
			if( e->flags == EDGE_TRUE_VALUE )
				label = "true";
			else if( e->flags == EDGE_FALSE_VALUE )
				label = "false";

			fprintf( out, "%d -> %d [color=red label=\"%s\"]\n",
					bb->index, e->dest->index, label ) ;

		}
	}
	/******************************/
	/**   TD2 - FIN QUESTION 7   **/
	/******************************/


	// Close the main graph
	fprintf(out, "}\n");
}

void 
cfgviz_dump( function * fun, const char * suffix, int td )
{
	char * target_filename ; 
	FILE * out ;

	target_filename = cfgviz_generate_filename( fun, suffix ) ;

	printf( "[GRAPHVIZ] Generating CFG of function %s in file <%s>\n",
			current_function_name(), target_filename ) ;

	out = fopen( target_filename, "w" ) ;

	cfgviz_internal_dump( fun, out, td ) ;

	fclose( out ) ;
	free( target_filename ) ;
}


/****************************************************************************************************/
/****************************************************************************************************/
/*************************            FIN  PLUGIN GRAPHVIZ                 *************************/ 
/****************************************************************************************************/
/****************************************************************************************************/




void td2_through_the_cfg(function * fun)
{
	td2_q3_q4_print_func_name(fun);
	td2_q5_q6_print_blocks(fun);
}



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
			function_isol_print(fun);
			printf("plugin: gate... \n");
			td2_q3_q4_print_func_name(fun);
			return true;
		}

		/* Execute function */
		unsigned int execute (function *fun)
		{
			printf("plugin: execute...\n");

			td_isol_print(/*TD*/2);

			/******************************/
			/**** TD2 - QUESTION 3 à 8 ****/
			/******************************/

			td2_through_the_cfg(fun);

			/******************************/
			/** TD2 - FIN QUESTION 3 à 8 **/
			/******************************/


			/******************************/
			/****   TD2 - QUESTION 7   ****/
			/******************************/

			cfgviz_dump( fun, "0_ini", /*TD*/2 ) ;

			/******************************/
			/**   TD2 - FIN QUESTION 7   **/
			/******************************/




			return 0;
		}


}; 



/* Main entry point for plugin */
	int 
plugin_init(struct plugin_name_args * plugin_info, 
		struct plugin_gcc_version * version)
{
	struct register_pass_info my_pass_info;

	printf( "plugin_init: Entering...\n" ) ;

	/* First check that the current version of GCC is the right one */

	if(!plugin_default_version_check(version, &gcc_version)) 
		return 1;

	printf( "plugin_init: Check ok...\n" ) ;

	/* Declare and build my new pass */
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

	return 0;
}


