#include "../utils/utils.h"


//int greedy flag: 
//                  0 -> we are calling grasp
//                  1 -> we are calling greedy
int grasp(instance* inst, int greedy, double tl)
{

	if (greedy) printf("--- Starting GREEDY ---\n");
	else printf("--- Starting GRASP ---\n");

	if (!inst->flagCost)
		computeCost(inst);

	if (inst->seed != -1)
		srand(inst->seed);

	//vector with the solutions
	int* solution = (int*)calloc(inst->nnodes, sizeof(int));

	//0-1 vector to memorize nodes already part of the solution
	int* visited = (int*)malloc(inst->nnodes * sizeof(int));

	double graspRand = 0.9; //0.5,0.6,0.7,0.8,0.9
	if (greedy) {
		graspRand = 1;
	}

	int iter = 0;


	do {

		//puts visited all at zeros
		memset(visited, 0, inst->nnodes * sizeof(int));

		//init
		int minIndex = -1;
		int minIndex2 = -1;

		double minDist = INFBOUND;
		double minDist2 = INFBOUND;

		int current;

		//start node at random
		if (greedy) current = iter;
		else current = rand() % inst->nnodes;
		
		int start = current;

		if (VERBOSE >= 100)   printf("start %d - ", current);

		visited[current] = 1;

		double cost = 0;

		for (int j = 0; j < inst->nnodes - 1; j++)
		{
			for (int i = 0; i < inst->nnodes; i++)
			{
				if (visited[i] == 1) continue;    //I skip nodes already part of the solution


				double actualDist = get_cost(current, i, inst);

				//if actual dist is better, update
				if (actualDist < minDist)
				{
					//the current min becomes the second min
					minDist2 = minDist;
					minIndex2 = minIndex;

					//new min
					minDist = actualDist;
					minIndex = i;
				}
				else if (actualDist < minDist2)
				{
					//new second min
					minDist2 = actualDist;
					minIndex2 = i;
				}
			}

			//last iter with only one node to pair (no second min)
			if (j == inst->nnodes - 2) {
				minDist2 = minDist;
				minIndex2 = minIndex;
			}

			if ((rand() % 10 + 1) <= (graspRand * 10))
			{
				cost += minDist;

				//now I add the new edge
				solution[current] = minIndex;
				visited[minIndex] = 1;

				current = minIndex;
			}
			else
			{
				cost += minDist2;

				//now I add the new edge
				solution[current] = minIndex2;
				visited[minIndex2] = 1;

				current = minIndex2;
			}

			minDist = INFBOUND;
			minDist2 = INFBOUND;
		}

		//close the circuit last-first
		solution[current] = start;
		cost += get_cost(current, start, inst);

		if (VERBOSE >= 100) printf("current cost: %f\n", cost);

		if (VERBOSE >= 10) {
			if (checkSol(inst, solution)) return 1;
			if (checkCost(inst, solution, cost)) return 1;
		}

		//if current cost is better, update best solution
		if (inst->zbest == -1 || inst->zbest > cost) {
			updateSol(inst, cost, solution);
			inst->indexStart = start;
		}

		iter++;

	} while (!timeOut(inst, tl) && iter*greedy < inst->nnodes);

	if (VERBOSE >= 50) printf("iter: %d\n", iter);

	//if (VERBOSE >= 10) printf("timeOut greedy: %f    that should be tl: %f\n", second() - inst->timeStart, tl);
	if (VERBOSE >= 1) printf("BEST SOLUTION FOUND\nSTART: %d     COST: %f\n", inst->indexStart, inst->zbest);

	plot(inst, inst->best_sol, "greedy");

	free(visited);
	free(solution);

	return 0;
}