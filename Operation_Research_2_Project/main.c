#include "tsp.h" 
//#include <gnuplot_c.h>


double second();
void print_error(const char* err);
void read_input(instance* inst);
void parse_command_line(int argc, char** argv, instance* inst);
void free_instance(instance* inst);

int main(int argc, char** argv)
{
	if (argc < 2) { printf("Usage: %s -help for help\n", argv[0]); exit(1); }
	if (VERBOSE >= 2) { for (int a = 0; a < argc; a++) printf("%s ", argv[a]); printf("\n"); }

	double t1 = second();
	instance inst;

	parse_command_line(argc, argv, &inst);

	//printf(" file %s has %d non-empty lines\n", inst.input_file, number_of_nonempty_lines(inst.input_file)); exit(1);

	read_input(&inst);
	//if ( VRPopt(&inst) ) print_error(" error within VRPopt()");
	double t2 = second();

	if (VERBOSE >= 1)
	{
		printf("... TSP problem solved in %lf sec.s\n", t2 - t1);
	}

	free_instance(&inst);

	/*
	h_GPC_Plot* plotter;
	plotter = gpc_init_xy("Best solution", "X coord", "Y coord",
		GPC_AUTO_SCALE, GPC_KEY_DISABLE);

	

	gpc_close(plotter);
	*/

	return 0;
}







