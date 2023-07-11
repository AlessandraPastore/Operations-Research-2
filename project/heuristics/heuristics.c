#include "../utils/utils.h"


int heuristic(instance* inst) {
	if (strcmp(inst->heuristic, "GREEDY") == 0)           return grasp(inst, 1, inst->timelimit);
	if (strcmp(inst->heuristic, "GREEDYGRASP") == 0)      return grasp(inst, 0, inst->timelimit);
	if (strcmp(inst->heuristic, "EXTRAMILEAGE") == 0)     return extra_mileage(inst,1);
	if (strcmp(inst->heuristic, "EXTRAMILEAGE2") == 0)     return extra_mileage(inst, 0);
	if (strcmp(inst->heuristic, "2_OPT") == 0) {
		grasp(inst, 0, inst->timelimit / 10); 
		return opt_2(inst, inst->timelimit, inst->best_sol, &(inst->zbest));
	}
	if (strcmp(inst->heuristic, "VNS") == 0) {

		grasp(inst, 0, inst->timelimit / 10); 
		opt_2(inst, inst->timelimit / 2, inst->best_sol, &(inst->zbest));
		return VNS(inst);

	}
	if (strcmp(inst->heuristic, "TABU") == 0) {
		grasp(inst, 0, inst->timelimit / 10);
		opt_2(inst, inst->timelimit / 2, inst->best_sol, &(inst->zbest));
		return TABU(inst, inst->timelimit);

	}

	if (strcmp(inst->heuristic, "GENETIC") == 0) return genetic(inst);

	if (strcmp(inst->heuristic, "ANNEALING") == 0) {
		grasp(inst, 0, inst->timelimit / 10); 
		return annealing(inst, inst->timelimit);

	}



	print_error("heuristic name not appropriate");
	return 1;
}

int cplex(instance* inst) {
	if (strcmp(inst->cplex, "CPLEX") == 0) return TSPopt(inst);
}