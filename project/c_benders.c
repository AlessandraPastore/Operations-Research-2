#include "utils.h"
#include <cplex.h>

int benders(instance* inst, CPXENVptr env, CPXLPptr lp) {

	int ncols = CPXgetnumcols(env, lp);

	//to store the solution
	double* xstar = (double*)calloc(ncols, sizeof(double));

	//vector with the current solutions
	int* succ = (int*)malloc(inst->nnodes * sizeof(int));

	//components vector
	int* comp = (int*)malloc(inst->nnodes * sizeof(int));
	int ncomp = 0;


	//BENDERS
	do {

		double start = second();

		//time limit for current mip opt
		CPXsetdblparam(env, CPX_PARAM_TILIM, inst->timeStart + inst->timelimit - start);

		// run CPLEX to get a solution
		int error = CPXmipopt(env, lp);
		if (error)
		{
			printf("CPX error code %d\n", error);
			print_error("CPXmipopt() error");
		}

		// use the optimal solution found by CPLEX and prints it

		//in teoria non cambia mai, cols = n. var
		//ncols = CPXgetnumcols(env, lp);

		if (CPXgetx(env, lp, xstar, 0, ncols - 1)) print_error("CPXgetx() error");
		for (int i = 0; i < inst->nnodes; i++)
		{
			for (int j = i + 1; j < inst->nnodes; j++)
			{
				if (xstar[xpos(i, j, inst)] > 0.5) printf("  ... x(%3d,%3d) = 1\n", i + 1, j + 1);
			}
		}


		// ----- Transforms xstar to succ and comp -----
		build_sol(xstar, inst, succ, comp, &ncomp);

		if (ncomp == 1) break;

		//TO DO: add SEC



	} while (!timeOut(inst, inst->timelimit));


	free(succ);
	free(comp);



	free(xstar);
	return 0;
}