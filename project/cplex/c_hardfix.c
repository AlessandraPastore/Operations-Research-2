#include "../utils/utils.h"
#include "../utils/tsp.h"

int hardfix(instance* inst, CPXENVptr env, CPXLPptr lp) {

	printf("--- HardFixing Matheuristic ---\n");

	int ncols = CPXgetnumcols(env, lp);
	inst->ncols = ncols;
	//to store the solution
	double* xstar = (double*)calloc(ncols, sizeof(double));

	//to do: calcolare quanto tempo ci ha messo greedy effettivamente
	double tl = inst->timelimit/2;
	grasp(inst, 1, tl);
	double initialSol = inst->zbest;

	CPXsetdblparam(env, CPX_PARAM_TILIM, inst->timelimit/2);

	for (int i = 0; i < inst->nnodes; i++) xstar[xpos(i, inst->best_sol[i], inst)] = 1.0;

	CPXLONG contextid = CPX_CALLBACKCONTEXT_CANDIDATE; // ... means lazyconstraints
	if (CPXcallbacksetfunc(env, lp, contextid, my_callback, inst)) print_error("CPXcallbacksetfunc() error");

	int* ind = (int*)malloc(inst->ncols * sizeof(int));
	for (int j = 0; j < inst->ncols; j++) ind[j] = j;
	int effortlevel = CPX_MIPSTART_NOCHECK;
	int beg = 0;
	if (CPXaddmipstarts(env, lp, 1, inst->ncols, &beg, ind, xstar, &effortlevel, NULL)) print_error("CPXaddmipstarts() error");
	free(ind);

	
	double prob = 7; //tune: 5,6,7,8
	double one = 1.0;
	double zero = 0.0;
	char lb = 'L';
	int* indexes = (int*)malloc(ncols * sizeof(int));
	do {

		
		int nchange = 0;
		for (int i = 0; i < ncols; i++)
		{
			int randNum = rand() % 10;
			if (xstar[i] > 0.5 && randNum < prob) {
				CPXchgbds(env, lp, 1, &i, &lb, &one);
				indexes[nchange] = i;
				nchange++;
			}
		}

		int error = CPXmipopt(env, lp);
		if (error)
		{
			printf("CPX error code %d\n", error);
			print_error("CPXmipopt() error");
		}
		int* succ = (int*)malloc(inst->nnodes * sizeof(int));

		//components vector
		int* comp = (int*)malloc(inst->nnodes * sizeof(int));
		int ncomp = 0;
		if (CPXgetx(env, lp, xstar, 0, ncols - 1)) print_error("error on CPXgetx");
		build_sol(xstar, inst, succ, comp, &ncomp);


		double objval = -1;
		if (CPXgetobjval(env, lp, &objval)) print_error("error on CPXgetobjval");

		//if current cost is better, update best solution
		if(ncomp==1)
			if (inst->zbest == -1 || inst->zbest > objval) {
				if (VERBOSE >= 10) {
					if (checkCost(inst, succ, objval)) return 1;
					if (checkSol(inst, succ)) return 1;

				}
				updateSol(inst, objval, succ);
		}
		free(succ);
		free(comp);
		
		for (int i = 0; i < nchange; i++)
		{
			CPXchgbds(env, lp, 1, &indexes[i], &lb, &zero);
		}


	} while (!timeOut(inst, tl));
	
	free(indexes);

	if (VERBOSE >= 1) printf("BEST SOLUTION FOUND\nCOST: %f  with greedy was: %f\n", inst->zbest,initialSol);
	plot(inst, inst->best_sol, "hardfix");


	free(xstar);
	return 0;
}