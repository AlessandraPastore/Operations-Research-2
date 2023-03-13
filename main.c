#include "tsp.h" 

double second();
void print_error(const char* err);
void read_input(instance* inst);
void parse_command_line(int argc, char** argv, instance* inst);
void free_instance(instance* inst);
int extra_mileage(instance *inst);
int greedy(instance *inst,int startNode);

int main(int argc, char** argv)
{
	if (argc < 2) { printf("Usage: %s -help for help\n", argv[0]); exit(1); }
	if (VERBOSE >= 2) { for (int a = 0; a < argc; a++) printf("%s ", argv[a]); printf("\n"); }

	double t1 = second();
	instance inst;

	parse_command_line(argc, argv, &inst);
	read_input(&inst);
	
	if ( extra_mileage(&inst) ) print_error(" error within VRPopt()");
	//if ( greedy(&inst,0) ) print_error(" error within VRPopt()");

	double t2 = second();

	if (VERBOSE >= 1)
	{
		printf("... TSP problem solved in %lf sec.s\n", t2 - t1);
	}

	free_instance(&inst);

	return 0;
}







