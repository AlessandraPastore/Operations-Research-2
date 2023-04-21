#include "../utils/utils.h"

int refinement(instance *inst, int *succ, int *comp, int ncomp, int iter) {

	//repairs the solution succ one component at a time
	for (int n = 1; n < ncomp; n++) {

		double delta = INFBOUND;
		int a = -1;
		int b = 1;
		
		//for all nodes i in comp 1
		for (int i = 0; i < inst->nnodes; i++) {
			if (comp[i] != 1) continue;

			//for all nodes j in next comp
			for (int j = i + 1; j < inst->nnodes; j++) {
				if (comp[j] != n + 1) continue;

				double deltaTemp = get_cost(i, succ[j], inst) + get_cost(j, succ[i], inst) 
								   - (get_cost(i, succ[i], inst) + get_cost(j, succ[j], inst));


				//if the new edges are better become candidates
				if (deltaTemp < delta)
				{
					delta = deltaTemp;
					a = i;
					b = j;
				}

			}


		}

		//add the new edges
		if (a != -1 && b != -1) {

			int tmp = succ[a];
			succ[a] = succ[b];
			succ[b] = tmp;
		}

		//merge the two components
		for (int i = 0; i < inst->nnodes; i++) {
			if (comp[i] == n + 1) comp[i] = 1;
		}

	}

	double cost = 0;

	for (int i = 0; i < inst->nnodes; i++) {
		cost += get_cost(i, succ[i], inst);
	}

	if (VERBOSE >= 10) {
		if (checkSol(inst, succ)) return 1;
		if (checkCost(inst, succ, cost)) return 1;
	}

	if (inst->zbest == -1 || inst->zbest > cost) {
		updateSol(inst, cost, succ);
	}

	return 0;

}

