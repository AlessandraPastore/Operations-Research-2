#include "utils.h" 

int main(int argc, char** argv)
{
	if (argc < 2) { printf("Usage: %s -help for help\n", argv[0]); exit(1); }
	if (VERBOSE >= 2) { for (int a = 0; a < argc; a++) printf("%s ", argv[a]); printf("\n"); }

	
	instance inst;

	parse_command_line(argc, argv, &inst);
	read_input(&inst);
	inst.timeStart = second();

	
	if(strcmp(inst.heuristic, "NULL")){
		printf("- - - Solving a heuristic: %s - - - \n", inst.heuristic);
		if(heuristic(&inst)) print_error("ERROR IN HEURISTIC");
	}


	if (VERBOSE >= 0)
	{
		printf("... TSP problem solved in %lf sec.s\n", inst.timeEnd - inst.timeStart);
	}

	free_instance(&inst);

	fflush(stdout);

	return 0;
}







