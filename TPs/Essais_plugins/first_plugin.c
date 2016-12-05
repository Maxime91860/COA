
#include <gcc-plugin.h>

int plugin_is_GPL_compatible;

int plugin_init (struct plugin_name_args *plugin_info, struct plugin_gcc_version *version)
{
	printf("Initialisation de mon plugin.\n");
	return 0;
}