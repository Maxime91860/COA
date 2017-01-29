
//--------------------------------- // 
//--------- PROJET COA -------------//
// Amira Akloul - Maxime Kermarquer //
//		M2 CHPS - Janvier 2017		//
//--------------------------------- //

/* 
	Le but de ce plugin GCC est d'effectuer une analyse statique du code afin 
	d'avertir un éventuel deadlock du code MPI compilé.
	Pour cela on parcourt le CFG pour vérifier que chaque chemin allant du debut du programme aux
	sorties de celui-ci, a bien la même séquence d'appel aux collectives MPI que tous les autres.
*/ 

#include <gcc-plugin.h>                                                          
#include <plugin-version.h>                                                      
#include <tree.h>                                                                
#include <basic-block.h>                                                         
#include <gimple.h>                                                              
#include <tree-pass.h>                                                           
#include <context.h>                                                             
#include <function.h>                                                            
#include <gimple-iterator.h>      
#include <c-family/c-common.h>                                               


/* Variable globale necéssaire au plugin */
int plugin_is_GPL_compatible;

int nb_bb_avant_ajout;

/* Objet representant notre passe */
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

//Vérifie si le statement est un appel à une collective MPI et renvoie le code de cet appel 
enum mpi_collective_code is_mpi_call( gimple * stmt, int bb_index)
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
			if ( strncmp( callee_name, mpi_collective_name[i], strlen(mpi_collective_name[i] ) ) == 0 )
			{
				found = true ;
				returned_code = (enum mpi_collective_code) i ;
			}
			i++ ;
		} 
	}

	return returned_code;
}

//Met à jour le champs aux des basic blocks avec le code de l'appel à la collective MPI
void read_and_mark_mpi(function *fun)
{
	basic_block bb;
	gimple_stmt_iterator gsi;
	gimple *stmt;

	FOR_EACH_BB_FN(bb,fun)
	{

		/* Iterate on gimple statements in the current basic block */
		for (gsi = gsi_start_bb (bb); !gsi_end_p (gsi); gsi_next (&gsi))
		{
			
			stmt = gsi_stmt(gsi);

			enum mpi_collective_code c;
			c = is_mpi_call(stmt, bb->index);
			if(bb->aux == (void *) LAST_AND_UNUSED_MPI_COLLECTIVE_CODE)
			{
				bb->aux = (void *) c ;
			}

		}	
	}
}

//Change la valeur "aux" de tous les basic blocks
void clean_aux_field( function * fun, long val )
{
	basic_block bb; 

	/* Traverse all BBs to clean 'aux' field */
	FOR_ALL_BB_FN (bb, fun)
	{
		bb->aux = (void *)val ;
	}
}


//------------------------------------------------------------------------//
//------------------- FONCTIONS DECOUPAGE BB -----------------------------//
//------------------------------------------------------------------------//

//Renvoie le nombre d'appels MPI dans un basic block
int get_nb_mpi_calls_in_bb( basic_block bb )
{
		gimple_stmt_iterator gsi;
		int nb_mpi_call = 0 ;

		for (gsi = gsi_start_bb (bb); !gsi_end_p (gsi); gsi_next (&gsi))
		{
			gimple *stmt = gsi_stmt (gsi); 

			enum mpi_collective_code c ;
			c = is_mpi_call( stmt, bb->index ) ;

			if ( c != LAST_AND_UNUSED_MPI_COLLECTIVE_CODE )
			{
				nb_mpi_call++ ;
			}
		}

		return nb_mpi_call ;
}

//Vérifie si il y a dans le CFG des basic blocks avec plusieurs appels MPI
bool check_multiple_mpi_calls_per_bb(function *fun)
{
	basic_block bb;
	bool is_multiple_mpi_coll = false;
	int nb_mpi_coll_in_bb = 0;
	FOR_EACH_BB_FN (bb, fun)
	{
		nb_mpi_coll_in_bb = get_nb_mpi_calls_in_bb(bb);	
		if(nb_mpi_coll_in_bb > 1){
			is_multiple_mpi_coll = true;
		}
	}

	return is_multiple_mpi_coll;
}

//Divise les basic blocks ayant plusieurs appels au collectives MPI
void split_multiple_mpi_calls( function * fun )
{
	basic_block bb; 

	FOR_EACH_BB_FN (bb, fun)
	{
		int n = get_nb_mpi_calls_in_bb( bb ) ;

		if ( n > 1 ) 
		{
			gimple_stmt_iterator gsi;
			for (gsi = gsi_start_bb (bb); !gsi_end_p (gsi); gsi_next (&gsi))
			{
				gimple *stmt = gsi_stmt (gsi); 
				enum mpi_collective_code c ;

				c = is_mpi_call( stmt, bb->index ) ;

				if ( c != LAST_AND_UNUSED_MPI_COLLECTIVE_CODE )
				{
					split_block( bb, stmt ) ;
				}

			}
		}
	}
}

//Vérifie et découpe les basic blocks qui ont plusieurs appels MPI.
//Met à jour le champs "aux"
void mpi_in_blocks(function * fun)
{
	bool do_splitting = false;
	
	read_and_mark_mpi(fun);
	do_splitting = check_multiple_mpi_calls_per_bb(fun);

	if(do_splitting)
	{
		split_multiple_mpi_calls(fun);
	}

	clean_aux_field(fun, LAST_AND_UNUSED_MPI_COLLECTIVE_CODE);
	read_and_mark_mpi(fun);
}

//------------------------------------------------------------------------//
//------------------------------------------------------------------------//
//------------------------------------------------------------------------//

//------------------------------------------------------------------------//
//---------------------- FONCTIONS WARNING -------------------------------//
//------------------------------------------------------------------------//

void issue_warnings (bitmap_head ipdf_set[], function * fun)
{
	basic_block bb;
	gimple_stmt_iterator gsi;
	gimple *stmt;

	int i;

	for(i=0; i<LAST_AND_UNUSED_MPI_COLLECTIVE_CODE; i++)
	{	
		if ( bitmap_count_bits( &ipdf_set[i] ) == 0 )
		{
			continue;
		} 	
		printf("\n\n----------- Problems for %s ---------\n\n", mpi_collective_name[i]);

		FOR_EACH_BB_FN( bb, fun )
		{
			if(bitmap_bit_p(&ipdf_set[i], bb->index))
			{
				gsi = gsi_start_bb(bb);
				stmt = gsi_stmt(gsi);

				printf("/!\\ /!\\ /!\\ Basic Block  %d  (line %d) might cause an issue\n", bb->index, gimple_lineno(stmt)); 
			}
		}
	}
}

void bitmap_post_dominance_frontiers (bitmap_head *frontiers, function * fun)
{
	edge p;
	edge_iterator ei;
	basic_block b;

	FOR_EACH_BB_FN (b, fun)
	{
		if (EDGE_COUNT (b->succs) >= 2)
		{
			FOR_EACH_EDGE (p, ei, b->succs)
			{
				basic_block runner = p->dest;
				basic_block pdomsb; 

				if (runner == EXIT_BLOCK_PTR_FOR_FN (fun))
					continue;

				pdomsb = get_immediate_dominator (CDI_POST_DOMINATORS, b);
				while (runner != pdomsb)
				{
					if (!bitmap_set_bit (&frontiers[runner->index], b->index))
						break;
					runner = get_immediate_dominator (CDI_POST_DOMINATORS, runner);
				}
			}
		}
	}
}

void bitmap_set_post_dominance_frontiers (bitmap_head node_set, bitmap_head *pdf, bitmap_head * pdf_set, function * fun)
{
	basic_block bb;

	/* Create the union of each PDF */
	FOR_ALL_BB_FN( bb, cfun )
	{
		if ( bitmap_bit_p( &node_set, bb->index ) )
		{
			bitmap_ior_into( pdf_set, &pdf[bb->index] ) ;
		}
	}

	/* Remove the nodes that have no other PDF remaining */
	FOR_ALL_BB_FN( bb, cfun )
	{
		if ( bitmap_bit_p( pdf_set, bb->index ) )
		{
			bool found = false ;
			basic_block bb2 ;

			FOR_ALL_BB_FN( bb2, cfun )
			{
				// bb2->index != bb->index &&
				if (!bitmap_bit_p( &node_set, bb2->index ) && bitmap_bit_p( &pdf[bb2->index], bb->index ) )
				{
					found = true ;
				}
			}

			if ( found == false )
			{
					bitmap_clear_bit( pdf_set, bb->index ) ;
			}
		}
	}
}

void iterated_post_dominance(bitmap_head pdf_node_set, bitmap_head *pdf, bitmap_head * ipdf_set, function * fun)
{
	basic_block bb, b;
	bitmap_head bitmap_tmp, bitmap_test;
	bitmap_initialize (&bitmap_tmp, &bitmap_default_obstack);
	bitmap_initialize (&bitmap_test, &bitmap_default_obstack);

	FOR_ALL_BB_FN( bb, fun )
	{
		if ( bitmap_bit_p( &pdf_node_set, bb->index ) )
		{
			bitmap_copy(&bitmap_test, &pdf_node_set);
			while(!bitmap_equal_p(&bitmap_tmp, &bitmap_test) /*|| begin*/)
			{
				bitmap_copy(&bitmap_tmp, &bitmap_test);
				FOR_ALL_BB_FN( b, fun )
				{
					if(bitmap_bit_p(&pdf[bb->index], b->index))
					{
						bitmap_set_bit(&bitmap_test, b->index);
					}
				}
			}	
			bitmap_copy(&bitmap_test, ipdf_set);
			bitmap_ior(ipdf_set, &bitmap_test, &bitmap_tmp);	
		}
	}
}

void bitmap_and_pdf_it(function * fun, bitmap_head ipdf_set[])
{
	calculate_dominance_info (CDI_POST_DOMINATORS);	
	bitmap_head *pfrontiers;
	basic_block bb;
	pfrontiers = XNEWVEC (bitmap_head, last_basic_block_for_fn (fun));

	FOR_ALL_BB_FN (bb, cfun)
	{
		bitmap_initialize (&pfrontiers[bb->index], &bitmap_default_obstack);
	}

	//Calcul de la frontière de post-dominance de tous les noeuds
	bitmap_post_dominance_frontiers (pfrontiers, fun);	

	int i;
	for(i=0; i<LAST_AND_UNUSED_MPI_COLLECTIVE_CODE; i++)
	{ 

		bitmap_initialize (&ipdf_set[i], &bitmap_default_obstack);

		/* Compute the set regrouping nodes with MPI calls */
		bitmap_head mpi_set ;
		bitmap_initialize( &mpi_set,  &bitmap_default_obstack);

		//Si un basic block contient la collective MPI i, on change la valeur du bitmap
		//On regroupe les basic blocks de la collectives i
		FOR_ALL_BB_FN (bb, cfun)
		{
			if ( bb->aux == (void *) i) 
			{
				bitmap_set_bit( &mpi_set, bb->index ) ;
			}
		}

		//On cacule la pdf de cet ensemble de basic blocks
		bitmap_head pdf_set;
		bitmap_initialize (&pdf_set, &bitmap_default_obstack);

		bitmap_set_post_dominance_frontiers(mpi_set, pfrontiers, &pdf_set, fun);


		//Calcul de la frontiere de post-dominance itérée de cet ensemble de noeud
		iterated_post_dominance(pdf_set, pfrontiers, &ipdf_set[i], fun);

		//Affichage de la pdf_it pour la collectives i
		// printf( "\n\nIterated PDF for call -- %s -- MPI node set: ", mpi_collective_name[i]) ;
		// bitmap_print( stdout, &ipdf_set[i], "", "\n" ) ;

		//Les noeuds appartement à cet ensemble sont les basic blocks pouvant causés des dead-locks.
	}


	issue_warnings (ipdf_set, fun);
}

//------------------------------------------------------------------------//
//------------------------------------------------------------------------//
//------------------------------------------------------------------------//


//------------------------------------------------------------------------//
//------------ FONCTIONS INCLUSION CHECK_COLLECTIVE-----------------------//
//------------------------------------------------------------------------//

void insert_print_and_CCfunc(function * fun, bitmap_head ipdf_set[])
{

	basic_block bb;
	gimple_stmt_iterator gsi;
	gimple *stmt;

	int i;

	int cpt = 0;

	for(i=0; i<LAST_AND_UNUSED_MPI_COLLECTIVE_CODE; i++)
	{
		cpt = bitmap_count_bits(&ipdf_set[i]);
		if(cpt > 0 || MPI_FINALIZE == i )
		{

			FOR_ALL_BB_FN (bb, fun)
			{
				if ( bb->aux == (void *) i) 
				{
					/* Iterate on gimple statements in the current basic block */
					for (gsi = gsi_start_bb (bb); !gsi_end_p (gsi); gsi_next (&gsi))
					{

						stmt = gsi_stmt(gsi);

						enum mpi_collective_code c;
						c = is_mpi_call(stmt, bb->index);
						if ( c != LAST_AND_UNUSED_MPI_COLLECTIVE_CODE )
						{

							char error_message[cpt*32+2]; 
							for(int l=0; l<cpt*32+2; l++) error_message[l] = '\0';
							error_message[0] = '-';
							error_message[1] = '-';
							char tmp_message[32]; 
							for(int l=0; l<32; l++) tmp_message[l] = ' ';

							basic_block bb2;
						        gimple_stmt_iterator gsi2;
						        gimple *stmt2;

							FOR_EACH_BB_FN( bb2, fun )
							{
								if(bitmap_bit_p(&ipdf_set[i], bb2->index))
								{	
									for(int l=0; l<32; l++) tmp_message[l] = '\0';
									// memset(&tmp_message[0], 0, 32*sizeof(char));
									gsi2 = gsi_start_bb(bb2);
									stmt2 = gsi_stmt(gsi2);

									sprintf(&tmp_message[0], "Basic Block  %d  (line %d) , ", bb2->index, gimple_lineno(stmt2));
									strncat(&error_message[0], &tmp_message[0], 32); 

								}
							}

							tree MPI_coll_name_tree = fix_string_type( build_string (strlen(mpi_collective_name[i]), mpi_collective_name[i]));	
							tree MPI_coll_name_ptr = build_pointer_type(TREE_TYPE (TREE_TYPE (MPI_coll_name_tree)));
							tree MPI_coll_name_arg = build1 (ADDR_EXPR, MPI_coll_name_ptr, MPI_coll_name_tree);



							tree bb_list_tree = fix_string_type( build_string (cpt*32+2, &error_message[0]));	
							tree bb_list_ptr = build_pointer_type(TREE_TYPE (TREE_TYPE (bb_list_tree)));
							tree bb_list_arg = build1 (ADDR_EXPR, bb_list_ptr, bb_list_tree);


							tree MPI_coll_id_arg = build_int_cst(integer_type_node, i);
							tree MPI_coll_line_arg = build_int_cst(integer_type_node, gimple_lineno(stmt));

							tree function_type_list = build_function_type_list(void_type_node, void_type_node, NULL_TREE);
							tree cc_fn_decl = build_fn_decl("checking_collectives", function_type_list);


							gimple * cc_call = gimple_build_call ( cc_fn_decl, 4, MPI_coll_id_arg, MPI_coll_name_arg, MPI_coll_line_arg, bb_list_arg);

							gimple_set_location(cc_call, gimple_location(stmt));
							gsi_insert_before( &gsi , cc_call, GSI_NEW_STMT);

							gsi_next (&gsi);
						}

					}	
				}
			}
		}

	}
}

//------------------------------------------------------------------------//
//------------------------------------------------------------------------//
//------------------------------------------------------------------------//

//------------------------------------------------------------------------//
//------------------- FONCTIONS AFFICHAGE CFG ----------------------------//
//------------------------------------------------------------------------//
/* Construit un nom de fichier à partir du nom de la fonction et d'un suffixe */
static char * cfgviz_generate_filename( function * fun, const char * suffix )
{
	char * target_filename ; 

	target_filename = (char *)xmalloc( 1024 * sizeof( char ) ) ;

	snprintf( target_filename, 1024, "FICHIERS_GRAPHVIZ/%s_%s_%s.dot", 
			LOCATION_FILE( fun->function_start_locus ),
			current_function_name(),
			suffix ) ;

	return target_filename ;
}

/* Ecrit la représentation GraphViz du CFG de la fonction dans un fichier .out */
static void cfgviz_internal_dump( function * fun, FILE * out ) 
{
	basic_block bb; 

	// Print the header line and open the main graph
	fprintf(out, "Digraph G{\n");


	FOR_ALL_BB_FN(bb,cfun)
	{

		//
		// Print the basic block BB, with the MPI call if necessary
		//

		// td == 3 && 
		if((long)bb->aux != LAST_AND_UNUSED_MPI_COLLECTIVE_CODE)
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



	// Close the main graph
	fprintf(out, "}\n");
}

// bool 

static void cfgviz_internal_dump2( function * fun, FILE * out , bitmap_head ipdf_set[]) 
{

	//Idées :
	// -Si un noeud pose problème le colorier d'une façon différente
	// -Crée un arc d'une couleur différente vers le(s) noeud(s) pouvant provoquer le deadlock

	basic_block bb; 

	// Print the header line and open the main graph
	fprintf(out, "Digraph G{\n");


	FOR_ALL_BB_FN(bb,cfun)
	{

		//
		// Print the basic block BB, with the MPI call if necessary
		//
		bool basic_block_dangeureux = false;
		int i;
		for(i=0; i<LAST_AND_UNUSED_MPI_COLLECTIVE_CODE; i++){
			if(bitmap_bit_p(&ipdf_set[i], bb->index)){
				basic_block_dangeureux = true;
			}
		}

		//Noeud avec dead-lock potentiel
		if(basic_block_dangeureux)
		{
			fprintf( out,
				"%d [color=red, style=filled, label=\"BB %d", bb->index,	bb->index);
		}
		else
		{			
			fprintf( out,
				"%d [label=\"BB %d", bb->index,	bb->index);
		}


		// td == 3 && 
		if((long)bb->aux != LAST_AND_UNUSED_MPI_COLLECTIVE_CODE)
		{

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

		else
		{
			fprintf( out, "\" shape=ellipse]\n") ;
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



	// Close the main graph
	fprintf(out, "}\n");
}

/* Regroupe les deux fonctions précédentes */
void cfgviz_dump( function * fun, const char * suffix , bitmap_head ipdf_set[])
{
	char * target_filename ; 
	FILE * out ;

	target_filename = cfgviz_generate_filename( fun, suffix ) ;

	out = fopen( target_filename, "w" ) ;

	if(out == NULL)
		fprintf(stderr, "Erreur ouverture\n");

	bool deadlock_potentiel = false;
	int i;
	for(i=0; i<LAST_AND_UNUSED_MPI_COLLECTIVE_CODE; i++){
		if(bitmap_count_bits( &ipdf_set[i] ) != 0){
			deadlock_potentiel = true;
			break;
		}
	}

	if ( !deadlock_potentiel )
	{
		cfgviz_internal_dump( fun, out ) ;
	}
	else
	{
		cfgviz_internal_dump2( fun, out, ipdf_set);
	}

	fclose( out ) ;
	free( target_filename ) ;
}

//------------------------------------------------------------------------//
//------------------------------------------------------------------------//
//------------------------------------------------------------------------//


/* Notre passe héritant d'une gimple passe */
class my_pass : public gimple_opt_pass
{
	public:
		my_pass (gcc::context *ctxt): gimple_opt_pass (my_pass_data, ctxt)
		{}

		my_pass *clone ()
		{
			return new my_pass(g);
		}

		/* Fonction gate, qui conditionne l'exécution de la passe */
		bool gate (function *fun)
		{	
			//Conditions sur le #pragma ici

			return true;
		}

		/* Coeur de la passe */
		unsigned int execute (function *fun)
		{
			const char * fname = function_name(fun);
			printf("\n--- EXECUTION DE LA PASSE ---\n");
			printf("   Fonction analysée : %s\n",fname);

			//Le champs "aux" nous est utile pour stocker quelle collectives MPI est dans le basic block.
			//S'il y'en a pas le champs a la valeur LAST_AND_UNUSED_MPI_COLLECTIVE_CODE.
			clean_aux_field(fun, LAST_AND_UNUSED_MPI_COLLECTIVE_CODE);


			//Découpe les basic blocks qui ont plusieurs collectives MPI.
			mpi_in_blocks(fun);


			//Utilisation de la frontière de post-dominance pour detecter les dead-locks.
			bitmap_head ipdf_set [LAST_AND_UNUSED_MPI_COLLECTIVE_CODE];
			bitmap_and_pdf_it(fun, ipdf_set);


			//Pour l'affichage du CFG
			cfgviz_dump(fun,"", ipdf_set);

			insert_print_and_CCfunc(fun, ipdf_set);

			//On remet à 0 le champs "aux" pour les autres passes de la compilation.
			clean_aux_field(fun, 0);

			free_dominance_info( CDI_POST_DOMINATORS ) ;
			return 0;
		}
}; 


/* Point d'entrée du plugin */
int plugin_init(struct plugin_name_args * plugin_info, struct plugin_gcc_version * version){

	/* Vérification de la version courante de GCC */
	if(!plugin_default_version_check(version, &gcc_version)) {
		fprintf(stderr, "Erreur version :\n\tPlugin incompatible avec cette version de GCC.\n");
		return 1;
	}

	//Définition de la passe 
	struct register_pass_info my_pass_info;

	my_pass p(g);

	my_pass_info.pass = &p;
	my_pass_info.reference_pass_name = "cfg";
	my_pass_info.ref_pass_instance_number = 0;
	my_pass_info.pos_op = PASS_POS_INSERT_AFTER;

	//Enregistrement dans le gestionnaire de passe
	register_callback(plugin_info->base_name, PLUGIN_PASS_MANAGER_SETUP, NULL, &my_pass_info);


    return 0;
}


