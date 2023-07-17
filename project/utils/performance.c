#include "utils.h"
#define MAX_COORD 10000

void randomInstance(instance* inst) {

	printf("...RANDOM\n");

	if (inst->seed != -1)
		srand(inst->seed);

	for (int i = 0; i < inst->nnodes; i++) {
		inst->xcoord[i] = rand() % MAX_COORD;
		inst->ycoord[i] = rand() % MAX_COORD;
	}

	computeCost(inst);

}

void initInstance(instance* inst) {

	printf("...INIT\n");

	inst->nnodes = 5000;

	inst->xcoord = (double*)calloc(inst->nnodes, sizeof(double));
	inst->ycoord = (double*)calloc(inst->nnodes, sizeof(double));

	inst->cost = (double*)calloc(inst->nnodes * inst->nnodes, sizeof(double));
	inst->flagCost = 0;

}

int callH(instance* inst, char name[]) {

	inst->timeStart = second();
	strcpy(inst->heuristic, name);
	if (heuristic(inst)) {
		print_error("ERROR IN HEUR");
		return 1;
	}

	return 0;
}

int callC(instance* inst, char name[]) {

	inst->timeStart = second();
	strcpy(inst->cplex, name);
	if (TSPopt(inst)) {
		print_error("ERROR IN CPLEX");
		return 1;
	}

	return 0;
}

//greedy grasp 2opt extramileage
int gge2(instance* inst, struct dirent* dir, FILE* out) {
	if (callH(inst, "GREEDY")) return 1;

	printf("%s,%f\n", dir->d_name, inst->zbest);
	fprintf(out, "%s,%f", dir->d_name, inst->zbest);

	memset(inst->best_sol, 0, inst->nnodes * sizeof(int));
	inst->zbest = -1;

	if (callH(inst, "GREEDYGRASP")) return 1;

	printf(",%f\n", inst->zbest);
	fprintf(out, ",%f", inst->zbest);


	memset(inst->best_sol, 0, inst->nnodes * sizeof(int));
	inst->zbest = -1;

	if (callH(inst, "2_OPT")) return 1;

	printf(",%f\n", inst->zbest);
	fprintf(out, ",%f", inst->zbest);


	memset(inst->best_sol, 0, inst->nnodes * sizeof(int));
	inst->zbest = -1;

	if (callH(inst, "EXTRAMILEAGE")) return 1;

	printf(",%f\n", inst->zbest);
	fprintf(out, ",%f\n", inst->zbest);
}

//tabu, vns, genetic, annealing
int meta(instance* inst, struct dirent* dir, FILE* out) {
	if (callH(inst, "TABU")) return 1;

	printf("%s,%f\n", dir->d_name, inst->zbest);
	fprintf(out, "%s,%f", dir->d_name, inst->zbest);

	memset(inst->best_sol, 0, inst->nnodes * sizeof(int));
	inst->zbest = -1;

	if (callH(inst, "VNS")) return 1;

	printf(",%f\n", inst->zbest);
	fprintf(out, ",%f", inst->zbest);


	memset(inst->best_sol, 0, inst->nnodes * sizeof(int));
	inst->zbest = -1;

	if (callH(inst, "GENETIC")) return 1;

	printf(",%f\n", inst->zbest);
	fprintf(out, ",%f", inst->zbest);


	memset(inst->best_sol, 0, inst->nnodes * sizeof(int));
	inst->zbest = -1;

	if (callH(inst, "ANNEALING")) return 1;

	printf(",%f\n", inst->zbest);
	fprintf(out, ",%f\n", inst->zbest);
}

int exact(instance* inst, struct dirent* dir, FILE* out) {

	if (callC(inst, "BENDERS")) return 1;

	int end = second() - inst->timeStart;;

	//printf("%s,%f\n", dir->d_name, end);
	fprintf(out, "%s,%d", dir->d_name, end);

	memset(inst->best_sol, 0, inst->nnodes * sizeof(int));
	inst->zbest = -1;

	if (callC(inst, "LAZY")) return 1;

	end = second() - inst->timeStart;

	//printf(",%f\n", end);
	fprintf(out, ",%d\n", end);

}

int tuneH(instance* inst, struct dirent* dir, FILE* out, char name[]) {
	if (callH(inst, name)) return 1;

	fprintf(out, "%f\n", inst->zbest);
}

int tuneC(instance* inst, struct dirent* dir, FILE* out, char name[]) {
	if (callC(inst, name)) return 1;

	int end = second() - inst->timeStart;

	fprintf(out, "%d\n", end);
}


int performance(instance* inst) {

	FILE* out = fopen(".\\output\\soft5.txt", "a"); //append
	if (out == NULL) {
		printf("output directory not found!");
		return 1;
	}

	//printf("time limit: %f", inst->timelimit);

	//initInstance(inst);
	//fprintf(out, "2,benders,callback\n");

	//FILE* in = fopen(inst->input_file, "r"); //append
	//if (in == NULL) printf("input directory not found!");

	read_input(inst);

	if (callC(inst, "HARD")) return 1;

	//int end = second() - inst->timeStart;

	fprintf(out, "%f\n", inst->zbest);



	
	/*

	DIR* d;
	struct dirent* dir;
	d = opendir(".\\input");
	if (d) {
		
		
		while ((dir = readdir(d)) != NULL) {
			printf("%s\n", dir->d_name);
			if (strncmp(dir->d_name, ".", 1) == 0) continue;
			if (strncmp(dir->d_name, "..", 1) == 0) continue;


			//for(int i=0; i<20; i++){

				//randomInstance(inst);
			char file[] = ".\\input\\";
			strcat(file, dir->d_name);
			printf("FILE:%s\n", file);
			strcpy(inst->input_file, file);
			//read_input(inst);

			

			//}

			//meta(inst, dir, out);

			//tuneH(inst, dir, out, "ANNEALING");

			//exact(inst, dir, out);

			//tuneC(inst, dir, out, "CONCORDE");

			System(or2.exe -)

			
		} 
		closedir(d);
	}
	free(dir);
	*/





	
	free_instance(inst);
	fclose(out);
	return 0;
}