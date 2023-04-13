#include "../utils/utils.h"

void SEC(instance *inst, CPXENVptr env, CPXLPptr lp, int ncomp, int *comp) {

	int* index = (int*)calloc(inst->nnodes, sizeof(int));
	double* value = (double*)calloc(inst->nnodes, sizeof(double));


	//int nnz = 0;
	//int sk = 0;	//size of each component

	//int izero = 0;

	//char sense = 'L'; // <= constraint
	//double rhs = 0;

	//colname parameter for CPXnewcols
	char** cname = (char**)calloc(1, sizeof(char*));		// (char **) required by cplex... array of 1 pointer
	cname[0] = (char*)calloc(100, sizeof(char));

	//for each component
	for (int k = 1; k <= ncomp; k++) {
		char sense = 'L'; // <= constraint
		double sk = 0.0;
		int nnz = 0;
		int izero = 0;

		memset(index, 0, inst->nnodes * sizeof(int));
		memset(value, 0, inst->nnodes * sizeof(double));

		sprintf(cname[0], "sec(%d)",k);

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

		printf("k = %d\n",k);
		printf("nnz = %d\n", nnz);
		printf("sk = %f\n", sk);


		double rhs = sk - 1;	// |S|-1

		printf("rhs = %f\n", rhs);

		if (CPXaddrows(env, lp, 0, 1, nnz, &rhs, &sense, &izero, index, value, NULL, &cname[0])) print_error("CPXaddrows(): error 1");

		
	}

	

	
	if (VERBOSE >= 1) {
		CPXwriteprob(env, lp, "modelSEC.lp", NULL);
	}

	free(value);
	free(index);

	free(cname[0]);
	free(cname);
}


int benders(instance* inst, CPXENVptr env, CPXLPptr lp) {

	printf("--- starting Benders ---\n");

	int ncols = CPXgetnumcols(env, lp);

	//to store the solution
	double* xstar = (double*)calloc(ncols, sizeof(double));

	//vector with the current solutions
	int* succ = (int*)malloc(inst->nnodes * sizeof(int));

	//components vector
	int* comp = (int*)malloc(inst->nnodes * sizeof(int));
	int ncomp = 0;

	do {
		double start = second();
		ncomp = 0;

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

		printf("ncomp = %d\n", ncomp);

		if (ncomp == 1) break;

		SEC(inst, env, lp, ncomp, comp);



	} while (1);
	//while (!timeOut(inst, inst->timelimit));




	free(succ);
	free(comp);



	free(xstar);
	return 0;
}