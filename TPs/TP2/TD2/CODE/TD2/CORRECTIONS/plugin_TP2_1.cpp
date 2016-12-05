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



/* Main entry point for plugin */
int 
plugin_init(struct plugin_name_args * plugin_info, struct plugin_gcc_version * version){

	printf( "plugin_init: Entering...\n" ) ;

	/* First check that the current version of GCC is the right one */

	if(!plugin_default_version_check(version, &gcc_version)) 
		return 1;

	printf( "plugin_init: Check ok...\n" ) ;


    return 0;
}


