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

int call(instance* inst, char name[]) {

	inst->timeStart = second();
	strcpy(inst->heuristic, name);
	if (heuristic(inst)) {
		print_error("ERROR IN HEUR");
		return 1;
	}

	return 0;
}

//greedy grasp 2opt extramileage
int gge2(instance* inst, struct dirent* dir, FILE* out) {
	if (call(inst, "GREEDY")) return 1;

	printf("%s,%f\n", dir->d_name, inst->zbest);
	fprintf(out, "%s,%f", dir->d_name, inst->zbest);

	memset(inst->best_sol, 0, inst->nnodes * sizeof(int));
	inst->zbest = -1;

	if (call(inst, "GREEDYGRASP")) return 1;

	printf(",%f\n", inst->zbest);
	fprintf(out, ",%f", inst->zbest);


	memset(inst->best_sol, 0, inst->nnodes * sizeof(int));
	inst->zbest = -1;

	if (call(inst, "2_OPT")) return 1;

	printf(",%f\n", inst->zbest);
	fprintf(out, ",%f", inst->zbest);


	memset(inst->best_sol, 0, inst->nnodes * sizeof(int));
	inst->zbest = -1;

	if (call(inst, "EXTRAMILEAGE")) return 1;

	printf(",%f\n", inst->zbest);
	fprintf(out, ",%f\n", inst->zbest);
}

//tabu, vns, genetic, annealing
int meta(instance* inst, struct dirent* dir, FILE* out) {
	if (call(inst, "TABU")) return 1;

	printf("%s,%f\n", dir->d_name, inst->zbest);
	fprintf(out, "%s,%f", dir->d_name, inst->zbest);

	memset(inst->best_sol, 0, inst->nnodes * sizeof(int));
	inst->zbest = -1;

	if (call(inst, "VNS")) return 1;

	printf(",%f\n", inst->zbest);
	fprintf(out, ",%f", inst->zbest);


	memset(inst->best_sol, 0, inst->nnodes * sizeof(int));
	inst->zbest = -1;

	if (call(inst, "GENETIC")) return 1;

	printf(",%f\n", inst->zbest);
	fprintf(out, ",%f", inst->zbest);


	memset(inst->best_sol, 0, inst->nnodes * sizeof(int));
	inst->zbest = -1;

	if (call(inst, "ANNEALING")) return 1;

	printf(",%f\n", inst->zbest);
	fprintf(out, ",%f\n", inst->zbest);
}

int tune(instance* inst, struct dirent* dir, FILE* out, char name[]) {
	if (call(inst, name)) return 1;

	fprintf(out, "%f\n", inst->zbest);
}


int performance(instance* inst) {

	FILE* out = fopen(".\\output\\sa95.txt", "w");
	if (out == NULL) printf("output directory not found!");

	printf("time limit: %f", inst->timelimit);

	initInstance(inst);
	fprintf(out, "4,tabu,vns,genetic,annealing\n");

	

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
			read_input(inst);

			

			//}

			meta(inst, dir, out);

			//tune(inst, dir, out, "ANNEALING");

			
		}
		closedir(d);
	}





	free(dir);
	free_instance(inst);
	fclose(out);
	return 0;
}