#include "../utils/utils.h"

static int CPXPUBLIC my_callback(CPXCALLBACKCONTEXTptr context, CPXLONG contextid, void* userhandle)
/********************************************************************************************************/
{
	instance* inst = (instance*)userhandle;
	double* xstar = (double*)malloc(inst->ncols * sizeof(double));
	double objval = CPX_INFBOUND;
	if (CPXcallbackgetcandidatepoint(context, xstar, 0, inst->ncols - 1, &objval)) print_error("CPXcallbackgetcandidatepoint error");

	int mythread = -1; CPXcallbackgetinfoint(context, CPXCALLBACKINFO_THREADID, &mythread);
	int mynode = -1; CPXcallbackgetinfoint(context, CPXCALLBACKINFO_NODECOUNT, &mynode);
	double incumbent = CPX_INFBOUND; CPXcallbackgetinfodbl(context, CPXCALLBACKINFO_BEST_SOL, &incumbent);
	if ( VERBOSE >= 10 ) printf(" ... callback at node %5d thread %2d incumbent %10.2lf\n", mynode,mythread,incumbent);

	//vector with the current solutions
	int* succ = (int*)malloc(inst->nnodes * sizeof(int));

	//components vector
	int* comp = (int*)malloc(inst->nnodes * sizeof(int));
	int ncomp = 0;

	build_sol(xstar, inst, succ, comp, &ncomp);
	if (ncomp > 1)
	{
		int* index = (int*)malloc(inst->ncols * sizeof(int));
		double* value = (double*)malloc(inst->ncols * sizeof(double));


		//colname parameter for CPXnewcols
		char** cname = (char**)calloc(1, sizeof(char*));		// (char **) required by cplex... array of 1 pointer
		cname[0] = (char*)calloc(100, sizeof(char));

		//for each component
		for (int k = 1; k <= ncomp; k++) {
			char sense = 'L'; // <= constraint
			double sk = 0.0;
			int nnz = 0;
			int izero = 0;


			//sprintf(cname[0], "sec(%d,%d)", it, k);

			for (int i = 0; i < inst->nnodes; i++) {
				if (comp[i] != k) continue;
				sk++;

				for (int j = i + 1; j < inst->nnodes; j++) {
					if (comp[j] != k) continue;

					index[nnz] = xpos(i, j, inst);
					value[nnz] = 1.0;
					nnz++;
				}
			}


			double rhs = sk - 1;// |S|-1

			if (CPXcallbackrejectcandidate(context, 1, nnz, &rhs, &sense, &izero, index, value)) print_error("CPXcallbackrejectcandidate() error");

		}
	}
	
		free(succ);
		free(comp);
		free(xstar);
		return 0;
	
}

int callback_sec(instance * inst, CPXENVptr env, CPXLPptr lp) {

		printf("--- starting B&C via callback ---\n");

		int ncols = CPXgetnumcols(env, lp);
		inst->ncols = ncols;
		//to store the solution
		double* xstar = (double*)calloc(ncols, sizeof(double));

		CPXsetdblparam(env, CPX_PARAM_TILIM,  inst->timelimit);
		CPXLONG contextid = CPX_CALLBACKCONTEXT_CANDIDATE; // ... means lazyconstraints
		if (CPXcallbacksetfunc(env, lp, contextid, my_callback, inst)) print_error("CPXcallbacksetfunc() error");

		int error = CPXmipopt(env, lp);
		if (error)
		{
			printf("CPX error code %d\n", error);
			print_error("CPXmipopt() error");
		}
		//vector with the current solutions
		int* succ = (int*)malloc(inst->nnodes * sizeof(int));

		//components vector
		int* comp = (int*)malloc(inst->nnodes * sizeof(int));
		int ncomp = 0;
		if (CPXgetx(env, lp, xstar, 0, ncols - 1)) {

			if (inst->zbest != -1) {
				printf("----- Terminated before convergence -----\n");
				printf("BEST SOLUTION FOUND\nCOST: %f\n", inst->zbest);
				plot(inst, succ, "benders");
				return 0;
			}
			else print_error("CPXgetx() error");
		}
		build_sol(xstar, inst, succ, comp, &ncomp);
		
		for (int i = 0; i < inst->nnodes; i++)
		{
			for (int j = i + 1; j < inst->nnodes; j++)
			{
				if (xstar[xpos(i, j, inst)] > 0.5) printf("  ... x(%3d,%3d) = 1\n", i + 1, j + 1);
			}
		}
		double objval = -1;
		if (CPXgetobjval(env, lp, &objval)) {
			if (inst->zbest != -1) {
				printf("----- Terminated before convergence -----\n");
				printf("BEST SOLUTION FOUND\nCOST: %f\n", inst->zbest);
				plot(inst, succ, "benders");
				return 0;
			}
			else print_error("CPXgetx() error");
			}

		if (VERBOSE >= 10) {
			if (checkSol(inst, succ)) return 1;
			if (checkCost(inst, succ, objval)) return 1;
		}

			//if current cost is better, update best solution
		if (inst->zbest == -1 || inst->zbest > objval) {
			updateSol(inst, objval, succ);
		}

		if (VERBOSE >= 1) printf("BEST SOLUTION FOUND\nCOST: %f\n", inst->zbest);
			plot(inst, succ, "callback_sec");

		
		free(succ);
		free(comp);
		free(xstar);
		return 0;
	}