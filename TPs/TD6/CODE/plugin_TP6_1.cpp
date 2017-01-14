#include <gcc-plugin.h>
#include <plugin-version.h>
#include <tree-pass.h>
#include <context.h>
#include <input.h>
#include <c-family/c-pragma.h>

int plugin_is_GPL_compatible;




/******************************/
/****   TD6 - QUESTION 1   ****/
/******************************/



static void my_pragma_action(cpp_reader * *ARG_UNUSED(dummy))
{
	printf("J'AI LU MON PRAGMA ET J'AI FAIT L'ACTION ASSOCIE!\n");
}


static void register_my_pragma(void* event_data, void* data)
{
	c_register_pragma("instrumente", "function" , my_pragma_action);
}


int plugin_init(struct plugin_name_args * plugin_info, struct plugin_gcc_version * version)
{
	fprintf(stderr, "Je commence l'init de mon plugin.\n");
	if(!plugin_default_version_check(version, &gcc_version)) return 1;



	register_callback(plugin_info->base_name, PLUGIN_PRAGMAS, register_my_pragma, NULL);

	return 0;
}

/******************************/
/**   TD6 - FIN QUESTION 1   **/
/******************************/


