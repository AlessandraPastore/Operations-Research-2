#include "utils.h"

double EUC_2D(instance* inst, int index1, int index2) {
	double x = inst->xcoord[index1] - inst->xcoord[index2];

	double y = inst->ycoord[index1] - inst->ycoord[index2];

	return sqrt((x * x + y * y));
}

double ATT(instance* inst, int index1, int index2) {
	double x = inst->xcoord[index1] - inst->xcoord[index2];

	double y = inst->ycoord[index1] - inst->ycoord[index2];

	return sqrt((x * x + y * y) / 10);
}

double MAN_2D(instance* inst, int index1, int index2) {
	double x = abs(inst->xcoord[index1] - inst->xcoord[index2]);

	double y = abs(inst->ycoord[index1] - inst->ycoord[index2]);

	return x + y;
}

double MAX_2D(instance* inst, int index1, int index2) {
	double x = abs(inst->xcoord[index1] - inst->xcoord[index2]);

	double y = abs(inst->ycoord[index1] - inst->ycoord[index2]);

	return (x > y) ? x : y;
}

double GEO(instance* inst, int index1, int index2) {

	double pi = 3.141592;

	//converts x1 and y1 in latitude and longitude
	int deg = (int)(inst->xcoord[index1] + 0.5);

	int min = inst->xcoord[index1] - deg;

	double lat1 = pi * (deg + 5.0 * min / 3.0) / 180.0;

	deg = (int)(inst->ycoord[index1] + 0.5);

	min = inst->ycoord[index1] - deg;

	double long1 = pi * (deg + 5.0 * min / 3.0) / 180.0;

	//converts x2 and y2 in latitude and longitude
	deg = (int)(inst->xcoord[index2] + 0.5);

	min = inst->xcoord[index2] - deg;

	double lat2 = pi * (deg + 5.0 * min / 3.0) / 180.0;

	deg = (int)(inst->ycoord[index2] + 0.5);

	min = inst->ycoord[index2] - deg;

	double long2 = pi * (deg + 5.0 * min / 3.0) / 180.0;

	//compute distance
	double rrr = 6378.388;
	double q1 = cos(long1 - long2);
	double q2 = cos(lat1 - lat2);
	double q3 = cos(lat1 + lat2);


	return rrr * acos(0.5 * ((1 + q1) * q2 - (1 - q1) * q3)) + 1;
}

double CEIL_2D(instance* inst, int index1, int index2) {
	double d = EUC_2D(inst, index1, index2);
	return (int)(d + 0.5);
}



double dist(instance* inst, int index1, int index2) {

	if (strncmp(inst->weight_type, "EUC_2D", 6) == 0) return EUC_2D(inst, index1, index2);
	if (strncmp(inst->weight_type, "MAX_2D", 6) == 0) return MAX_2D(inst, index1, index2);
	if (strncmp(inst->weight_type, "MAN_2D", 6) == 0) return MAN_2D(inst, index1, index2);
	if (strncmp(inst->weight_type, "CEIL_2D", 7) == 0) return CEIL_2D(inst, index1, index2);
	if (strncmp(inst->weight_type, "GEO", 3) == 0) return GEO(inst, index1, index2);
	if (strncmp(inst->weight_type, "ATT", 3) == 0) return ATT(inst, index1, index2);
	if (strncmp(inst->weight_type, "EMPTY", 5) == 0) print_error(" format error:  EDGE_WEIGHT_TYPE not implemented");

	return 0;
}

void computeCost(instance* inst)
{

	for (int i = 0; i < inst->nnodes; i++)
		for (int j = 0; j < i; j++)
			inst->cost[i * inst->nnodes + j] = inst->cost[j * inst->nnodes + i] = dist(inst, i, j);

	inst->flagCost = 1;
	inst->zbest = -1;
	inst->best_sol = (int*)calloc(inst->nnodes, sizeof(int));
}

double get_cost(int i, int j, instance* inst) {
	if (i < 0 || j < 0 || i >= inst->nnodes || j >= inst->nnodes) {
		printf("get_cost(): unexpected params i: %d, j= %dd", i, j);
	}
	return inst->cost[i * inst->nnodes + j];
}

void plot(instance* inst, int* solution, char name[]) {

	FILE* out = fopen(".\\output\\out.txt", "w");
	if (out == NULL) printf("output dir not found!");
	

	int x = 0;
	for (int i = 0; i < inst->nnodes; i++) {

		fprintf(out, "%f %f\n", inst->xcoord[solution[x]], inst->ycoord[solution[x]]);
		x = solution[x];

	}

	fprintf(out, "%f %f\n", inst->xcoord[solution[x]], inst->ycoord[solution[x]]);

	fclose(out);

	//da modificare o aggiungere una funzione per creare il file commands.txt
	//GNUPLOT to cmd
	FILE* commands = fopen(".\\plot\\commands.txt", "w+");
	if (commands == NULL) printf("commands dir not found!");

	fprintf(commands, "set terminal png\n");
	fprintf(commands, "set output \"./plot/%s.png\"\n", name);
	fprintf(commands, "set style line 1 \\\n");
	fprintf(commands, "    linecolor rgb'#FF0000' \\\n");
	fprintf(commands, "    linetype 1 linewidth 2 \\\n");
	fprintf(commands, "    pointtype 7 pointsize 2\n");
	fprintf(commands, "plot \"./output/out.txt\" with linespoint linestyle 1\n");



	fclose(commands);
	system("gnuplot ./plot/commands.txt");
}

void addToPlot(double obj, int iter) {

	char mode[3];
	if (iter == 0) {
		//mode = "w";
		strcpy(mode, "w");
	}
	else strcpy(mode, "a+");

	FILE* out = fopen(".\\output\\outPerf.txt", mode);
	
	if (out == NULL) printf("output dir not found!");
	

	fprintf(out, "%d %f\n", iter, obj);

	fclose(out);
}

void plotPerf(char name[]) {
	//da modificare o aggiungere una funzione per creare il file commands.txt
	//GNUPLOT to cmd
	FILE* commands = fopen(".\\plot\\commands.txt", "w+");
	if (commands == NULL) printf("commands dir not found!");

	fprintf(commands, "set terminal png\n");
	fprintf(commands, "set output \"./plot/%s.png\"\n", name);
	fprintf(commands, "set style line 1 \\\n");
	fprintf(commands, "    linecolor rgb'#FF0000' \\\n");
	fprintf(commands, "    linetype 1 linewidth 2 \\\n");
	fprintf(commands, "    pointtype 7 pointsize 2\n");
	fprintf(commands, "plot \"./output/outPerf.txt\" with linespoint linestyle 1\n");



	fclose(commands);
	system("gnuplot ./plot/commands.txt");
}

int timeOut(instance* inst, double tl) {
	return (second() - inst->timeStart) > tl;
}

int checkSol(instance* inst, int* sol) {
	int* visited = (int*)calloc(inst->nnodes, sizeof(int));
	int error = 0;

	int next = 0;

	for (int i = 0; i < inst->nnodes; i++) {
		visited[sol[next]]++;
		next = sol[next];
	}

	for (int i = 0; i < inst->nnodes; i++) {
		if (visited[i] != 1) {
			printf("ERROR IN THE SOLUTION: %d as %d\n\n", i, visited[i]);
			error = 1;
		}
	}
	free(visited);
	return error;

}

int checkCost(instance* inst, int* sol, double c) {
	double cost = 0;
	for (int i = 0; i < inst->nnodes; i++) {
		cost += get_cost(i, sol[i], inst);
	}

	//checks that the true cost of the solution and the computed cost are equal
	if (fabs(cost - c) > 0.000001) {
		printf("ERROR IN THE SOLUTION COST: %f as %f\n\n", cost, c);
		return 1;
	}

	return 0;

}

//updates best solution and zcost
void updateSol(instance* inst, double cost, int* solution)
{
	if (VERBOSE >= 10) printf("-------- Updating best solution --------\n");
	inst->zbest = cost;
	memcpy(inst->best_sol, solution, inst->nnodes * sizeof(int));
}


//reverse path b -> a1
void reverse(instance* inst, int* solution, int a, int b)
{
	//printf("SWAP 2OPT: %d with %d\n",a,b);
	int a1 = solution[a];
	int b1 = solution[b];

	int* old = (int*)malloc(inst->nnodes * sizeof(int));
	memcpy(old, solution, sizeof(int) * inst->nnodes);


	solution[a] = b;
	solution[a1] = b1;

	int i = a1;

	while (i != b)
	{
		solution[old[i]] = i;
		i = old[i];
	}


	free(old);

}