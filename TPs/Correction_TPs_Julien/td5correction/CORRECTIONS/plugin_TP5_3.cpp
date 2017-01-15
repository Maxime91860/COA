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
/*************************                       TD3                        *************************/ 
/****************************************************************************************************/
/****************************************************************************************************/




/******************************/
/**** TD3 - QUESTION 1 à 3 ****/
/******************************/

enum mpi_collective_code td3_q1_q2_q3_is_mpi_call( gimple * stmt, int bb_index)
{
	enum mpi_collective_code returned_code ;

	returned_code = LAST_AND_UNUSED_MPI_COLLECTIVE_CODE ;

	if (is_gimple_call (stmt))
	{
		tree t ;
		const char * callee_name ;
		int i ;
		bool found = false ;

		t = gimple_call_fndecl( stmt ) ;
		callee_name = IDENTIFIER_POINTER(DECL_NAME(t)) ;

		i = 0 ;
		while ( !found && i < LAST_AND_UNUSED_MPI_COLLECTIVE_CODE )
		{
			if ( strncmp( callee_name, mpi_collective_name[i], strlen(
							mpi_collective_name[i] ) ) == 0 )
			{
				found = true ;
				returned_code = (enum mpi_collective_code) i ;
			}
			i++ ;
		} 

	}


	if ( returned_code != LAST_AND_UNUSED_MPI_COLLECTIVE_CODE )
	{
		printf( "    -->   |||++||| BB %d [MPI CALL] Found call to %s\n", bb_index, mpi_collective_name[returned_code] ) ;
	}

	return returned_code ;


}

/******************************/
/** TD3 - FIN QUESTION 1 à 3 **/
/******************************/





/******************************/
/****   TD3 - QUESTION 5   ****/
/******************************/

void clean_aux_field( function * fun, long val )
{
	basic_block bb; 

	/* Traverse all BBs to clean 'aux' field */
	FOR_ALL_BB_FN (bb, fun)
	{
		bb->aux = (void *)val ;
	}

}
/******************************/
/**   TD3 - FIN QUESTION 5   **/
/******************************/



void td3_q1_q2_q3_q4_read_and_mark_mpi(function *fun)
{
	basic_block bb;
	gimple_stmt_iterator gsi;
	gimple *stmt;

	FOR_EACH_BB_FN(bb,fun)
	{

		/* Iterate on gimple statements in the current basic block */
		for (gsi = gsi_start_bb (bb); !gsi_end_p (gsi); gsi_next (&gsi))
		{
			
			/******************************/
			/**   TD3 - QUESTION 1 à 4   **/
			/******************************/
			stmt = gsi_stmt(gsi);

			enum mpi_collective_code c;
			c = td3_q1_q2_q3_is_mpi_call(stmt, bb->index);
			if(bb->aux == (void *) LAST_AND_UNUSED_MPI_COLLECTIVE_CODE)
			{
				bb->aux = (void *) c ;
			}

			/******************************/
			/** TD3 - FIN QUESTION 1 à 4 **/
			/******************************/

		}	
	}

}


/******************************/
/****   TD3 - QUESTION 7   ****/
/******************************/

int td3_q7_get_nb_mpi_calls_in_bb( basic_block bb )
{
		gimple_stmt_iterator gsi;
		int nb_mpi_coll = 0 ;

		for (gsi = gsi_start_bb (bb); !gsi_end_p (gsi); gsi_next (&gsi))
		{
			gimple *stmt = gsi_stmt (gsi); 

			enum mpi_collective_code c ;
			c = td3_q1_q2_q3_is_mpi_call( stmt, bb->index ) ;

			if ( c != LAST_AND_UNUSED_MPI_COLLECTIVE_CODE )
			{
				nb_mpi_coll++ ;
			}
		}

		printf( " == BB %d == contains %d MPI collective(s)\n", 
				bb->index, nb_mpi_coll ) ;

		return nb_mpi_coll ;
}

bool td3_q7_check_multiple_mpi_calls_per_bb(function *fun)
{
	basic_block bb;
	bool is_multiple_mpi_coll = false;
	int nb_mpi_coll_in_bb = 0;
	FOR_EACH_BB_FN (bb, fun)
	{
		nb_mpi_coll_in_bb = td3_q7_get_nb_mpi_calls_in_bb(bb);	
		if(nb_mpi_coll_in_bb > 1){
			is_multiple_mpi_coll = true;
		}
	}

	return is_multiple_mpi_coll;
}

/******************************/
/**   TD3 - FIN QUESTION 7   **/
/******************************/


/******************************/
/****   TD3 - QUESTION 8   ****/
/******************************/

void td3_q8_split_multiple_mpi_calls( function * fun )
{
	basic_block bb; 

	FOR_EACH_BB_FN (bb, fun)
	{
		int n = td3_q7_get_nb_mpi_calls_in_bb( bb ) ;

		if ( n > 1 ) 
		{
			gimple_stmt_iterator gsi;

			printf( "[SPLIT] Need to split BB %d (%d collectives)\n",
					bb->index, n ) ;
			for (gsi = gsi_start_bb (bb); !gsi_end_p (gsi); gsi_next (&gsi))
			{
				gimple *stmt = gsi_stmt (gsi); 
				enum mpi_collective_code c ;

				c = td3_q1_q2_q3_is_mpi_call( stmt, bb->index ) ;

				if ( c != LAST_AND_UNUSED_MPI_COLLECTIVE_CODE )
				{
					split_block( bb, stmt ) ;
				}

			}
		}
	}
}

/******************************/
/**   TD3 - FIN QUESTION 8   **/
/******************************/



/****************************************************************************************************/
/****************************************************************************************************/
/*************************                     FIN TD3                      *************************/ 
/****************************************************************************************************/
/****************************************************************************************************/



/****************************************************************************************************/
/****************************************************************************************************/
/*************************                       TD4                        *************************/ 
/****************************************************************************************************/
/****************************************************************************************************/




/******************************/
/****   TD4 - QUESTION 5   ****/
/******************************/


void td4_q5_q6_print_dominators(function * fun)
{
	printf("DOMINATORS:\n");

	basic_block bb;
	FOR_ALL_BB_FN(bb, fun)
	{
		vec<basic_block> dominators;
//		dominators = get_dominated_by(CDI_DOMINATORS, bb);
		dominators = get_all_dominated_blocks(CDI_DOMINATORS, bb);	

		printf("Basic block BB %d dominates :", bb->index);

		
		int i;
		int size = dominators.length();
	
		for (i = 0; i<size; i++)
		{
		
/******************************/
/****   TD4 - QUESTION 6   ****/
/******************************/
			if(dominators[i]->index != bb->index)
			{

				printf(" { BB %d }", dominators[i]->index);
			}
/******************************/
/**   TD4 - FIN QUESTION 6   **/
/******************************/


		}
		printf("\n");
	}
		

}

/******************************/
/**   TD4 - FIN QUESTION 5   **/
/******************************/


/******************************/
/****   TD4 - QUESTION 7   ****/
/******************************/


void td4_q7_print_postdominators(function * fun)
{
	printf("POST-DOMINATORS:\n");

	basic_block bb;
	FOR_ALL_BB_FN(bb, fun)
	{
		vec<basic_block> postdoms;
		postdoms = get_all_dominated_blocks(CDI_POST_DOMINATORS, bb);	

		printf("Basic block BB %d dominates :", bb->index);

		
		int i;
		int size = postdoms.length();
	
		for (i = 0; i<size; i++)
		{
		
			if(postdoms[i]->index != bb->index)
			{

				printf(" { BB %d }", postdoms[i]->index);
			}

		}
		printf("\n");
	}
		

}

/******************************/
/**   TD4 - FIN QUESTION 7   **/
/******************************/





/******************************/
/****   TD4 - QUESTION 8   ****/
/******************************/


void td4_q8_print_dfs(function * fun)
{

	basic_block bb;
	FOR_EACH_BB_FN(bb, fun)
	{
		printf("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n");
		printf("$   Dominance Frontier values for BB %d   $\n", bb->index);
		printf("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n");



		printf("entry bb_dom_dfs_in (CDI_DOMINATORS, BB %d) = %d\n", bb->index, bb_dom_dfs_in (CDI_DOMINATORS, bb));
		printf("exit bb_dom_dfs_out (CDI_DOMINATORS, BB %d) = %d\n", bb->index, bb_dom_dfs_out (CDI_DOMINATORS, bb));

//		printf("entry bb_dom_dfs_in (CDI_POST_DOMINATORS, BB %d) = %d\n", bb->index, bb_dom_dfs_in (CDI_POST_DOMINATORS, bb));
//		printf("exit bb_dom_dfs_out (CDI_POST_DOMINATORS, BB %d) = %d\n", bb->index, bb_dom_dfs_in (CDI_POST_DOMINATORS, bb));

	}
	

}



void td4_q8_compute_post_dominance_frontiers (function * fun)
{

	printf("\n\n$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n");
	printf("$$       Dominance Frontier values      $$\n");
	printf("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n\n\n");

	edge p;
	edge_iterator ei;
	basic_block b;


	int cpt = 0;

	int number_bbs = last_basic_block_for_fn (fun);
/*
	FOR_EACH_BB_FN (b, fun)
	{
		if(b->index > number_bbs)
			number_bbs = b->index;
	}

	number_bbs ++;
*/
	/* One line per bb to register each bb in postdom frontier for the current bb. */
	/* first cell of each line to store number of bb in postdom frontier of current bb */ 
	int isPDF[number_bbs][number_bbs+1];
	memset (&isPDF, 0, number_bbs * (number_bbs+1) * sizeof(int)); 

	FOR_EACH_BB_FN(b, fun)
	{ 

		if (EDGE_COUNT (b->succs) >= 2)
//		if (EDGE_COUNT (b->preds) >= 2)
		{
			FOR_EACH_EDGE (p, ei, b->succs)
//			FOR_EACH_EDGE (p, ei, b->preds)
			{
				basic_block runner = p->dest;
//				basic_block runner = p->src;
				basic_block pdomsb; 

				if (runner == EXIT_BLOCK_PTR_FOR_FN (fun))
//				if (runner == ENTRY_BLOCK_PTR_FOR_FN (fun))
					continue;


				pdomsb = get_immediate_dominator (CDI_POST_DOMINATORS, b);
//				pdomsb = get_immediate_dominator (CDI_DOMINATORS, b);
				while (runner != pdomsb)
				{
					if (0 == isPDF[runner->index][b->index+1])
					{
						isPDF[runner->index][b->index+1] = 1;
						isPDF[runner->index][0]++;
						cpt++;
						printf("%d\n", cpt);
					}
					else
					{
						break;
					}
					runner = get_immediate_dominator (CDI_POST_DOMINATORS, runner);
//					runner = get_immediate_dominator (CDI_DOMINATORS, runner);
				}
			}
		}
	}

	int i;
	for(i=0; i<number_bbs; i++)
	{
		if(0 == isPDF[i][0])
		{
			printf("Basic block %d has empty PDF\n", i); fflush(stdout);
		}
		else
		{
			printf("Basic block %d has %d basic blocks in its PDF:\n", i, isPDF[i][0]); fflush(stdout);
			int j;
			for(j=0; j<number_bbs; j++)
			{
				if(0 != isPDF[i][j+1])
				{
					printf("------- Basic Block %d\n", j); fflush(stdout);
				}
			}
		}
	}


	
}


/******************************/
/**   TD4 - FIN QUESTION 8   **/
/******************************/






/****************************************************************************************************/
/****************************************************************************************************/
/*************************                     FIN TD4                      *************************/ 
/****************************************************************************************************/
/****************************************************************************************************/


/****************************************************************************************************/
/****************************************************************************************************/
/*************************                       TD5                        *************************/ 
/****************************************************************************************************/
/****************************************************************************************************/



/******************************/
/****   TD5 - QUESTION 1   ****/
/******************************/

void                                                                      
td5_q1_bitmap_post_dominance_frontiers (bitmap_head *frontiers, function * fun)
{
	int cpt = 0;
	edge p;
	edge_iterator ei;
	basic_block b;

	FOR_EACH_BB_FN (b, fun)
	{

		if (EDGE_COUNT (b->succs) >= 2)
//		if (EDGE_COUNT (b->preds) >= 2)
		{
			FOR_EACH_EDGE (p, ei, b->succs)
//			FOR_EACH_EDGE (p, ei, b->preds)
			{
				basic_block runner = p->dest;
//				basic_block runner = p->src;
				basic_block pdomsb; 

				if (runner == EXIT_BLOCK_PTR_FOR_FN (fun))
//				if (runner == ENTRY_BLOCK_PTR_FOR_FN (fun))
					continue;

				pdomsb = get_immediate_dominator (CDI_POST_DOMINATORS, b);
//				pdomsb = get_immediate_dominator (CDI_DOMINATORS, b);
				while (runner != pdomsb)
				{
					if (!bitmap_set_bit (&frontiers[runner->index], b->index))
						break;
/*
					if (1 == bitmap_bit_p(&frontiers[runner->index], b->index))
					{
						break;
					}	
					else
					{
						bitmap_set_bit (&frontiers[runner->index], b->index);	
						cpt++;
						printf("%d\n", cpt);
	
					}
*/
					runner = get_immediate_dominator (CDI_POST_DOMINATORS, runner);
//					runner = get_immediate_dominator (CDI_DOMINATORS, runner);
				}
			}
		}
	}
}

/******************************/
/**   TD5 - FIN QUESTION 1   **/
/******************************/


/******************************/
/****   TD5 - QUESTION 2   ****/
/******************************/

void                                                                      
td5_q2_bitmap_set_post_dominance_frontiers (bitmap_head node_set, bitmap_head *pdf, bitmap_head * pdf_set, function * fun)
{
	basic_block bb;

	printf( "+++++ Entering PDFSet\n" ) ;

	/* TODO use macro to iterate through bit with 1 */

	/* Create the union of each PDF */
	FOR_ALL_BB_FN( bb, cfun )
	{
		if ( bitmap_bit_p( &node_set, bb->index ) )
		{
			bitmap_ior_into( pdf_set, &pdf[bb->index] ) ;
		}
	}

	bitmap_print( stdout, pdf_set, "Partial PDF of set: ", "\n" ) ;

	/* Remove the nodes that have no other PDF remaining */
	FOR_ALL_BB_FN( bb, cfun )
	{
		if ( bitmap_bit_p( pdf_set, bb->index ) )
		{
			bool found = false ;
			basic_block bb2 ;

			FOR_ALL_BB_FN( bb2, cfun )
			{
				if ( 
						// bb2->index != bb->index &&
						!bitmap_bit_p( &node_set, bb2->index ) &&
						bitmap_bit_p( &pdf[bb2->index], bb->index ) )
				{
					printf( "\tWill not remove BB%d because of BB%d\n", 
							bb->index, bb2->index ) ;
					found = true ;
				}
			}

			if ( found == false )
			{
					bitmap_clear_bit( pdf_set, bb->index ) ;
			}
		}
	}

	bitmap_print( stdout, pdf_set, "Final PDF of set: ", "\n" ) ;

	printf( "+++++ Exiting PDFSet\n" ) ;

}

/******************************/
/**   TD5 - FIN QUESTION 2   **/
/******************************/



/****************************************************************************************************/
/****************************************************************************************************/
/*************************                     FIN TD5                      *************************/ 
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

		/******************************/
		/****   TD3 - QUESTION 6   ****/
		/******************************/


		if(td == 3 && (long)bb->aux != LAST_AND_UNUSED_MPI_COLLECTIVE_CODE)
		{
			fprintf( out,
					"%d [label=\"BB %d", bb->index,	bb->index);

			gimple_stmt_iterator gsi;
			gimple * stmt;
			gsi = gsi_start_bb(bb);
			stmt = gsi_stmt(gsi);

			/* Iterate on gimple statements in the current basic block */
			for (gsi = gsi_start_bb (bb); !gsi_end_p (gsi); gsi_next (&gsi))
			{

				stmt = gsi_stmt(gsi);

				enum mpi_collective_code returned_code ;

				returned_code = LAST_AND_UNUSED_MPI_COLLECTIVE_CODE ;

				if (is_gimple_call (stmt))
				{
					tree t ;
					const char * callee_name ;
					int i ;
					bool found = false ;

					t = gimple_call_fndecl( stmt ) ;
					callee_name = IDENTIFIER_POINTER(DECL_NAME(t)) ;

					i = 0 ;
					while ( !found && i < LAST_AND_UNUSED_MPI_COLLECTIVE_CODE )
					{
						if ( strncmp( callee_name, mpi_collective_name[i], strlen(
										mpi_collective_name[i] ) ) == 0 )
						{
							found = true ;
							returned_code = (enum mpi_collective_code) i ;
						}
						i++ ;
					} 

				}


				if ( returned_code != LAST_AND_UNUSED_MPI_COLLECTIVE_CODE )
				{
					fprintf( out, " \\n %s", mpi_collective_name[returned_code] ) ;
				}
			}

			fprintf(out, "\" shape=ellipse]\n");

		}
		/******************************/
		/**** TD3 - FIN QUESTION 6 ****/
		/******************************/
		else
		{
			fprintf( out,
					"%d [label=\"BB %d\" shape=ellipse]\n",
					bb->index,
					bb->index
			       ) ;
		}

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


void td3_mpi_in_blocks(function * fun)
{
	bool do_splitting = false;
	
	td3_q1_q2_q3_q4_read_and_mark_mpi(fun);
	cfgviz_dump( fun, "1_mpi", /*TD*/3 ) ;
	do_splitting = td3_q7_check_multiple_mpi_calls_per_bb(fun);

/******************************/
/****   TD3 - QUESTION 8   ****/
/******************************/

	if(do_splitting)
	{
		td3_q8_split_multiple_mpi_calls(fun);
	}

	clean_aux_field(fun, LAST_AND_UNUSED_MPI_COLLECTIVE_CODE);
	td3_q1_q2_q3_q4_read_and_mark_mpi(fun);
	cfgviz_dump( fun, "2_split", /*TD*/3 ) ;	


/******************************/
/**   TD3 - FIN QUESTION 8   **/
/******************************/


	
}


void td4_dom_and_postdom(function * fun)
{
	/******************************/
	/**** TD4 - QUESTION 5 & 6 ****/
	/******************************/
	calculate_dominance_info (CDI_DOMINATORS);	
	td4_q5_q6_print_dominators(fun);
	/******************************/
	/** TD4 - FIN QUESTION 5 & 6 **/
	/******************************/


	/******************************/
	/****   TD4 - QUESTION 7   ****/
	/******************************/
	calculate_dominance_info (CDI_POST_DOMINATORS);	
	td4_q7_print_postdominators(fun);	
	/******************************/
	/**   TD4 - FIN QUESTION 7   **/
	/******************************/

	td4_q8_print_dfs(fun);
	td4_q8_compute_post_dominance_frontiers (fun);

}


void td5_bitmap_and_pdf_it(function * fun)
{
	/******************************/
	/****   TD5 - QUESTION 1   ****/
	/******************************/
	bitmap_head *pfrontiers;
	basic_block bb;
	pfrontiers = XNEWVEC (bitmap_head, last_basic_block_for_fn (fun));

	FOR_ALL_BB_FN (bb, cfun)
	{
		bitmap_initialize (&pfrontiers[bb->index], &bitmap_default_obstack);
	}

	td5_q1_bitmap_post_dominance_frontiers (pfrontiers, fun);	

	FOR_ALL_BB_FN (bb, cfun)
	{
		printf( "PDF Node %d: ", bb->index ) ;
		bitmap_print( stdout, &pfrontiers[bb->index], "", "\n" ) ;
	}
	/******************************/
	/**   TD5 - FIN QUESTION 1   **/
	/******************************/



	/******************************/
	/****   TD5 - QUESTION 2   ****/
	/******************************/

	/* Compute the set regrouping nodes with MPI calls */
	bitmap_head mpi_set ;
	bitmap_initialize( &mpi_set,  &bitmap_default_obstack);

	FOR_ALL_BB_FN (bb, cfun)
	{
		if ( bb->aux == (void *)1) 
		{
			bitmap_set_bit( &mpi_set, bb->index ) ;
		}
	}

	/* Print the bitmap containing nodes with MPI calls */
	printf( "MPI node set: ") ;
	bitmap_print( stdout, &mpi_set, "", "\n" ) ;

	bitmap_head pdf_set;
	bitmap_initialize (&pdf_set, &bitmap_default_obstack);

	td5_q2_bitmap_set_post_dominance_frontiers(mpi_set, pfrontiers, &pdf_set, fun);

	printf( "Pdf for MPI node set: ") ;
	bitmap_print( stdout, &pdf_set, "", "\n" ) ;


	/******************************/
	/**   TD5 - FIN QUESTION 2   **/
	/******************************/


	/******************************/
	/****   TD5 - QUESTION 3   ****/
	/******************************/

	/* if the PDF of the set it not empty => issues! */
	if ( bitmap_count_bits( &pdf_set ) > 0 )
	{
		printf( "ISSUES WITH MPI CALLS\n" ) ;
	} 
	else
	{
		printf( "NO ISSUES WITH MPI CALLS\n" ) ;
	}

	/******************************/
	/**   TD5 - FIN QUESTION 3   **/
	/******************************/



	printf("\n\n================== FIN ==================\n\n");
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

			/******************************/
			/**********   TD3   ***********/
			/******************************/

			td_isol_print(/*TD*/3);

			/******************************/
			/****   TD3 - QUESTION 5   ****/
			/******************************/
			clean_aux_field(fun, LAST_AND_UNUSED_MPI_COLLECTIVE_CODE);
			/******************************/
			/**   TD3 - FIN QUESTION 5   **/
			/******************************/

			td3_mpi_in_blocks(fun);

			/******************************/
			/********** FIN TD3 ***********/
			/******************************/

			/******************************/
			/**********   TD4   ***********/
			/******************************/
	
			td_isol_print(/*TD*/4);
			td4_dom_and_postdom(fun);

			/******************************/
			/********** FIN TD4 ***********/
			/******************************/

			/******************************/
			/**********   TD5   ***********/
			/******************************/
	
			td_isol_print(/*TD*/5);
			td5_bitmap_and_pdf_it(fun);

			/******************************/
			/********** FIN TD5 ***********/
			/******************************/

			/******************************/
			/****   TD3 - QUESTION 5   ****/
			/******************************/
			clean_aux_field(fun, 0);
			/******************************/
			/**   TD3 - FIN QUESTION 5   **/
			/******************************/


			free_dominance_info( CDI_DOMINATORS ) ;
			free_dominance_info( CDI_POST_DOMINATORS ) ;

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


