#include "../utils/utils.h"

#define MAX 2147483647

int TABU(instance* inst, double tl)
{
	if (VERBOSE >= 10) printf("--- Starting TABU ---\n");
	if (!inst->flagCost)
		computeCost(inst);

	double delta;
	double newCost = inst->zbest;

	//tune
	int tmax;
	int tenure_ratio = 2; //tune: 10,8,6,4
	int tenure = 100; //tune: 100,150,200

	if (inst->nnodes / tenure_ratio < tenure)
		tmax = inst->nnodes / tenure_ratio;
	else
		tmax = tenure;


	//vector with the new edge combination
	int* solution = (int*)calloc(inst->nnodes, sizeof(int));
	int* tabu = (int*)calloc(inst->nnodes, sizeof(int));

	for (int i = 0; i < inst->nnodes; i++)
		tabu[i] = MAX;

	double oldCost = inst->zbest;
	printf("----- OPT2: %f -----\n", inst->zbest);

	//solution copies the actual local minimum
	memcpy(solution, inst->best_sol, sizeof(int) * inst->nnodes);

	int iteration = 0; // to keep track of the number of the current iteration

	do {
		delta = INFBOUND;
		int a = -1, b = -1;
		for (int i = 0; i < inst->nnodes - 1; i++) {
			//if a node i or it neighbour solution[i] form a tabu edge I skip the trial to see if exist a delta
			if (abs(tabu[i] - iteration) > tmax && abs(tabu[solution[i]] - iteration) > tmax) {

				for (int j = i + 1; j < inst->nnodes; j++)
				{
					//if a node j or it neighbour solution[j] form a tabu edge I skip the trial to see if exist a delta
					if (abs(tabu[j] - iteration) > tmax && abs(tabu[solution[j]] - iteration) > tmax)
					{

						double deltaTemp = get_cost(i, j, inst) + get_cost(solution[j], solution[i], inst) - (get_cost(i, solution[i], inst) + get_cost(j, solution[j], inst));

						if (deltaTemp!=0 && deltaTemp < delta)
						{

							delta = deltaTemp;
							a = i;
							b = j;
						}
					}
				}
			}
		}


		if (a >= 0 && b >= 0)
		{
			reverse(inst, solution, a, b);

			int choose = rand() % 4;
			if(delta > 0)
			{
				if (choose == 0)
					tabu[a] = iteration;
				if (choose == 1)
					tabu[b] = iteration;
				if (choose == 2)
					tabu[solution[a]] = iteration;
				if (choose == 3)
					tabu[solution[b]] = iteration;
			}
			
			newCost += delta;  //update cost
			//printf("- %d\n",delta);

		}

		

		if (VERBOSE >= 10) {
			if (checkSol(inst, solution)) return 1;
			if (checkCost(inst, solution, newCost)) return 1;
		}
		
		//update solution
		if (inst->zbest == -1 || inst->zbest > newCost) {
			updateSol(inst, newCost, solution);
		}

		//add to plot newCost
		if (iteration < 300)
		addToPlot(newCost, iteration);
		printf("----- new: %f -----\n", newCost);
		

		iteration++;

	} while (!timeOut(inst, tl));

	printf("it: %d\n", iteration);

	plotPerf("TabuPerf");


	//if (VERBOSE >= 10) printf("zbest: %f, cost: %f\n", inst->zbest, newCost);

	plot(inst, solution, "Tabu");

	if (VERBOSE >= 1 && inst->zbest < oldCost)
		printf("Tabu IMROVEMENT: old cost %f --> new cost %f\n", oldCost, inst->zbest);
	else
		printf("no improvment\n");
	return 0;
}
