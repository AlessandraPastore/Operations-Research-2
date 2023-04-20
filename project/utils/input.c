#include "utils.h"

void print_error(const char* err) { printf("\n\n ERROR: %s \n\n", err); fflush(NULL); exit(1); }


void free_instance(instance* inst)
{
	free(inst->xcoord);
	free(inst->ycoord);
	free(inst->cost);
	free(inst->best_sol);
	
}

//simple parser for TSPLIB 
void read_input(instance* inst)
{
	if (VERBOSE >= 100) printf("input file: %s\n", inst->input_file);
	FILE* fin = fopen(inst->input_file, "r");
	if (fin == NULL) print_error(" input file not found!");

	//default
	inst->nnodes = -1;
	strcpy(inst->weight_type, "EMPTY");

	char line[180];
	char* par_name;
	char* token1;
	char* token2;

	int active_section = 0; // =1 NODE_COORD_SECTION



	while (fgets(line, sizeof(line), fin) != NULL)
	{

		if (VERBOSE >= 2000) { printf("%s", line); fflush(NULL); }
		if (strlen(line) <= 1) continue; // skip empty lines

		par_name = strtok(line, " :");

		if (VERBOSE >= 3000) { printf("parameter \"%s\" ", par_name); fflush(NULL); }

		if (strncmp(par_name, "NAME", 4) == 0)
		{
			active_section = 0;
			continue;
		}

		if (strncmp(par_name, "COMMENT", 7) == 0)
		{
			active_section = 0;
			token1 = strtok(NULL, "");
			if (VERBOSE >= 10) printf(" ... solving instance %s \n\n", token1);
			continue;
		}

		if (strncmp(par_name, "TYPE", 4) == 0)
		{
			token1 = strtok(NULL, " :");
			if (strncmp(token1, "TSP", 3) != 0) print_error(" format error:  only TYPE == TSP implemented so far!!!!!!");
			active_section = 0;
			continue;
		}


		if (strncmp(par_name, "DIMENSION", 9) == 0)
		{
			if (inst->nnodes >= 0) print_error(" repeated DIMENSION section in input file");
			token1 = strtok(NULL, " :");
			inst->nnodes = atoi(token1);
			inst->xcoord = (double*)calloc(inst->nnodes, sizeof(double));
			inst->ycoord = (double*)calloc(inst->nnodes, sizeof(double));
			//inst->index=(int*)calloc(inst->nnodes, sizeof(int));
			inst->cost = (double*)calloc(inst->nnodes * inst->nnodes, sizeof(double));
			inst->flagCost = 0;
			active_section = 0;
			continue;
		}


		if (strncmp(par_name, "EDGE_WEIGHT_TYPE", 16) == 0)
		{
			token1 = strtok(NULL, " :");
			//if (strncmp(token1, "ATT", 3) != 0) print_error(" format error:  only EDGE_WEIGHT_TYPE == ATT implemented so far!!!!!!");

			if (strncmp(token1, "EUC_2D", 6) == 0) strcpy(inst->weight_type, "EUC_2D");
			if (strncmp(token1, "MAX_2D", 6) == 0) strcpy(inst->weight_type, "MAX_2D");
			if (strncmp(token1, "MAN_2D", 6) == 0) strcpy(inst->weight_type, "MAN_2D");
			if (strncmp(token1, "CEIL_2D", 7) == 0) strcpy(inst->weight_type, "CEIL_2D");
			if (strncmp(token1, "GEO", 3) == 0) strcpy(inst->weight_type, "GEO");
			if (strncmp(token1, "ATT", 3) == 0) strcpy(inst->weight_type, "ATT");
			if (strncmp(token1, "EMPTY", 5) == 0) print_error(" format error:  EDGE_WEIGHT_TYPE not implemented");

			active_section = 0;
			continue;
		}

		if (strncmp(par_name, "NODE_COORD_SECTION", 18) == 0)
		{
			if (inst->nnodes <= 0) print_error(" ... DIMENSION section should appear before NODE_COORD_SECTION section");
			active_section = 1;
			continue;
		}

		if (strncmp(par_name, "EOF", 3) == 0)
		{
			active_section = 0;
			break;
		}

		if (strncmp(par_name, "DISPLAY_DATA_TYPE", 18) == 0)
		{
			active_section = 0;
			break;
		}


		if (active_section == 1) // within NODE_COORD_SECTION
		{
			//index
			int i = atoi(par_name) - 1;

			if (VERBOSE >= 100) { printf("index: %d \n", i); fflush(NULL); }
			if (i < 0 || i >= inst->nnodes) print_error(" ... unknown node in NODE_COORD_SECTION section");

			token1 = strtok(NULL, " :,");
			token2 = strtok(NULL, " :,");
			//inst->index[i]=i;
			inst->xcoord[i] = atof(token1);
			inst->ycoord[i] = atof(token2);

			if (VERBOSE >= 100) { printf("coord: %f,%f \n", inst->xcoord[i], inst->ycoord[i]); fflush(NULL); }

			continue;
		}

		printf(" final active section %d\n", active_section);
		print_error(" ... wrong format for the current simplified parser!!!!!!!!!");

	}

	fclose(fin);

}

void parse_command_line(int argc, char** argv, instance* inst)
{

	if (VERBOSE >= 100) printf(" running %s with %d parameters \n", argv[0], argc - 1);

	//DEFAULT VALUES
	strcpy(inst->input_file, "NULL");
	inst->timelimit = 0;
	inst->seed = -1;
	strcpy(inst->heuristic, "NULL");
	strcpy(inst->cplex, "NULL");


	int help = 0; if (argc < 1) help = 1;
	for (int i = 1; i < argc; i++)
	{
		if (strcmp(argv[i], "-f") == 0) { strcpy(inst->input_file, argv[++i]); continue; } 			// input file
		if (strcmp(argv[i], "-tl") == 0) { inst->timelimit = atof(argv[++i]); continue; }		// time limit
		if (strcmp(argv[i], "-h") == 0) { strcpy(inst->heuristic, argv[++i]); continue; } 		// heuristic to run
		if (strcmp(argv[i], "-c") == 0) { strcpy(inst->cplex, argv[++i]); continue; } 		// exact algorithm to run
		if (strcmp(argv[i], "-seed") == 0) { inst->seed = atof(argv[++i]); continue; } 		//  random seed
		if (strcmp(argv[i], "-help") == 0) { help = 1; continue; } 									// help
		if (strcmp(argv[i], "--help") == 0) { help = 1; continue; } 									// help
		//if (strcmp(argv[i], "-performance") == 0) { help = 1; break; } 
		help = 1;
	}

	if (help)		// print current parameters
	{
		printf("\n\navailable parameters (vers. may-2023) --------------------------------------------------\n");
		printf("-f file name\n");
		printf("-tl time limit\n");
		printf("-seed random seed\n");
		printf("-h heuristic method\n");
		printf("-c exact method using cplex\n");

		printf("\nenter -help or --help for help\n");
		printf("----------------------------------------------------------------------------------------------\n\n");
	}

	if (help) exit(1);

}

