#include "../utils/utils.h"
#include "../utils/tsp.h"

int softfix(instance* inst, CPXENVptr env, CPXLPptr lp) {

	printf("--- SoftFixing Matheuristic ---\n");

	int ncols = CPXgetnumcols(env, lp);
	inst->ncols = ncols;
	//to store the solution
	double* xstar = (double*)calloc(ncols, sizeof(double));


	double tl = inst->timelimit / 2;
	grasp(inst, 1, tl);
	double initialSol = inst->zbest;

	CPXsetdblparam(env, CPX_PARAM_TILIM, inst->timelimit / 2);

	for (int i = 0; i < inst->nnodes; i++) xstar[xpos(i, inst->best_sol[i], inst)] = 1.0;

	CPXLONG contextid = CPX_CALLBACKCONTEXT_CANDIDATE; // ... means lazyconstraints
	if (CPXcallbacksetfunc(env, lp, contextid, my_callback, inst)) print_error("CPXcallbacksetfunc() error");

	int* ind = (int*)malloc(inst->ncols * sizeof(int));
	for (int j = 0; j < inst->ncols; j++) ind[j] = j;
	int effortlevel = CPX_MIPSTART_NOCHECK;
	int beg = 0;
	if (CPXaddmipstarts(env, lp, 1, inst->ncols, &beg, ind, xstar, &effortlevel, NULL)) print_error("CPXaddmipstarts() error");
	free(ind);
	
	int* indexes = (int*)malloc(ncols * sizeof(int));
	double* values = (double*)malloc(ncols * sizeof(double));
	
	int rad [] = {(int)inst->nnodes/20,(int)inst->nnodes/25,(int)inst->nnodes/30};
	int radindex = 0;
	int numiter = 0;


	char sense = 'G';   // >=
	int matbeg = 0;
	char* names = (char*)malloc(100*sizeof(char));

	int numberOfPlateau = 0;

	do {


		int k = 0;
		for (int i = 0; i < ncols; i++)
		{
			if (xstar[i] > 0.5) {
				indexes[k] = i;
				values[k] = 1.0;
				k++;
			}
		}

		double rhs = inst->nnodes - rad[radindex];
		sprintf(names, "new_constraint(%d)", numiter++);


		if (CPXaddrows(env, lp, 0, 1, k, &rhs, &sense, &matbeg, indexes, values, NULL, &names))
		{
			print_error("error on addrows");
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
		if (ncomp == 1) {
			if (inst->zbest == -1 || inst->zbest > objval) {
				if (VERBOSE >= 10) {
					if (checkCost(inst, succ, objval)) return 1;
					if (checkSol(inst, succ)) return 1;

				}
				if (numberOfPlateau > 0)
					numberOfPlateau--;
				updateSol(inst, objval, succ);
			}
			else {
				numberOfPlateau++;
			}
			if (numberOfPlateau > 3)
				radindex++;
			if (radindex > 2)
				radindex = 0;
		}
		free(succ);
		free(comp);


		int numrows = CPXgetnumrows(env, lp);
		if (CPXdelrows(env, lp, numrows - 1, numrows - 1))
		{
			print_error("error on deleterow");
		}
		


	} while (!timeOut(inst, tl));

	free(indexes);
	free(values);
	free(names);


	if (VERBOSE >= 1) printf("BEST SOLUTION FOUND\nCOST: %f  with greedy was: %f\n", inst->zbest, initialSol);
	plot(inst, inst->best_sol, "hardfix");


	free(xstar);
	return 0;
}