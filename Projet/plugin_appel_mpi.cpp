
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


/* Variable globale necéssaire au plugin */
int plugin_is_GPL_compatible;

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


//------------------------------------------------------------------------//
//--------------------FONCTIONS AFFICHAGE CFG-----------------------------//
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

/* Ecrit la représentation GraphViz de la fonction dans un fichier .out */
static void cfgviz_internal_dump( function * fun, FILE * out ) 
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

/* Regroupe les deux fonctions précédentes */
void cfgviz_dump( function * fun, const char * suffix )
{
	char * target_filename ; 
	FILE * out ;

	target_filename = cfgviz_generate_filename( fun, suffix ) ;

	out = fopen( target_filename, "w" ) ;

	if(out == NULL)
		fprintf(stderr, "Erreur ouverture\n");

	cfgviz_internal_dump( fun, out ) ;

	fclose( out ) ;
	free( target_filename ) ;
}

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
			cfgviz_dump(fun,"");
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


