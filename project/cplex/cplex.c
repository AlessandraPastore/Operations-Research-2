#include "../utils/utils.h"
#include <cplex.h>

//TO DO
void set_params(instance* inst, CPXENVptr env) {

	CPXsetintparam(env, CPX_PARAM_SCRIND, CPX_OFF);
	if (VERBOSE >= 10) CPXsetintparam(env, CPX_PARAM_SCRIND, CPX_ON); // Cplex output on screen

	//random seed
	if (inst->seed != -1) CPXsetintparam(env, CPX_PARAM_RANDOMSEED, fabs(inst->seed));

	//time limit
	if (inst->timelimit == 0) inst->timelimit = 3600;
	//CPXsetdblparam(env, CPX_PARAM_TILIM,inst->timelimit); 

	// TO DO, NUM THREADS
	// TO DO, set tree memory limit
}

//TO DO
void build_model(instance* inst, CPXENVptr env, CPXLPptr lp)
{


	//double zero = 0.0;  
	char binary = 'B';

	//colname parameter for CPXnewcols
	char** cname = (char**)calloc(1, sizeof(char*));		// (char **) required by cplex... array of 1 pointer
	cname[0] = (char*)calloc(100, sizeof(char));

	// add binary var.s x(i,j) for i < j  
	/*
		min sum x(i,j)*dist(i,j) <-- objective function coeff
		0 <= x(i,j) <= 1
	*/

	for (int i = 0; i < inst->nnodes; i++)
	{
		for (int j = i + 1; j < inst->nnodes; j++)
		{
			sprintf(cname[0], "x(%d,%d)", i + 1, j + 1);  		// ... x(1,2), x(1,3) ....
			//printf("x(%d,%d)", i + 1, j + 1);
			double obj = dist(inst, i, j); // cost == distance   
			double lb = 0.0;
			double ub = 1.0;

			/*

			int CPXnewcols: adds empty columns
					env
					lp
					ccnt --> An integer that specifies the number of new variables being added to the problem object. (eg columns)
					obj --> array lenght ccnt, containing the objective function coefficients of the new variables
					lb --> 0	(lower bound)
					ub --> 1	(upper bound)
					xctype --> 'B' binary
					colname --> array length ccnt, contains pointers to character strings that specify the names of the new variables added to the problem object
			*/
			if (CPXnewcols(env, lp, 1, &obj, &lb, &ub, &binary, cname)) print_error(" wrong CPXnewcols on x var.s");
			if (CPXgetnumcols(env, lp) - 1 != xpos(i, j, inst)) print_error(" wrong position for x var.s");
		}
		printf("\n");
	}

	// add the degree constraints 
	// max 2 degree per node 



	int* index = (int*)calloc(inst->nnodes, sizeof(int));
	double* value = (double*)calloc(inst->nnodes, sizeof(double));

	for (int h = 0; h < inst->nnodes; h++)  		// add the degree constraint on node h
	{
		double rhs = 2.0;
		char sense = 'E';                            // 'E' for equality constraint 
		sprintf(cname[0], "degree(%d)", h + 1);
		int nnz = 0;
		for (int i = 0; i < inst->nnodes; i++)
		{
			if (i == h) continue;
			index[nnz] = xpos(i, h, inst);
			value[nnz] = 1.0;
			nnz++;
		}
		int izero = 0;
		/*

		int CPXaddrows: adds constraints
				env
				lp
				ccnt --> number of new columns in the constraints being added
				rcnt --> number of new rows to be added
				nzcnt --> number of nonzero constraint coefficients to be added
				rhs --> array length rcnt, contains the righthand side term for each constraint to be added (e.g. degree 2)
				sense --> 'E' equality costraint
				rmatbeg
				rmatind
				rmatval
				colname
				rowname
		*/
		if (CPXaddrows(env, lp, 0, 1, nnz, &rhs, &sense, &izero, index, value, NULL, &cname[0])) print_error("CPXaddrows(): error 1");
	}

	free(value);
	free(index);

	free(cname[0]);
	free(cname);

	if (VERBOSE >= 1) CPXwriteprob(env, lp, "model.lp", NULL);

}

int xpos(int i, int j, instance* inst)      // to be verified                                           
{
	if (i == j) print_error(" i == j in xpos");
	if (i > j) return xpos(j, i, inst);
	int pos = i * inst->nnodes + j - ((i + 1) * (i + 2)) / 2;
	return pos;
}

#define DEBUG    // comment out to avoid debugging 
#define EPS 1e-5

void build_sol(const double* xstar, instance* inst, int* succ, int* comp, int* ncomp) // build succ() and comp() wrt xstar()...
{

#ifdef DEBUG
	int* degree = (int*)calloc(inst->nnodes, sizeof(int));
	for (int i = 0; i < inst->nnodes; i++)
	{
		for (int j = i + 1; j < inst->nnodes; j++)
		{
			int k = xpos(i, j, inst);
			if (fabs(xstar[k]) > EPS && fabs(xstar[k] - 1.0) > EPS) print_error(" wrong xstar in build_sol()");
			if (xstar[k] > 0.5)
			{
				++degree[i];
				++degree[j];
			}
		}
	}
	for (int i = 0; i < inst->nnodes; i++)
	{
		if (degree[i] != 2) print_error("wrong degree in build_sol()");
	}
	free(degree);
#endif

	* ncomp = 0;
	for (int i = 0; i < inst->nnodes; i++)
	{
		succ[i] = -1;
		comp[i] = -1;
	}

	for (int start = 0; start < inst->nnodes; start++)
	{
		if (comp[start] >= 0) continue;  // node "start" was already visited, just skip it

		// a new component is found
		(*ncomp)++;
		int i = start;
		int done = 0;
		while (!done)  // go and visit the current component
		{
			comp[i] = *ncomp;
			done = 1;
			for (int j = 0; j < inst->nnodes; j++)
			{
				if (i != j && xstar[xpos(i, j, inst)] > 0.5 && comp[j] == -1) // the edge [i,j] is selected in xstar and j was not visited before 
				{
					succ[i] = j;
					i = j;
					done = 0;
					break;
				}
			}
		}
		succ[i] = start;  // last arc to close the cycle

		// go to the next component...
	}
}

int TSPopt(instance* inst)
{

	// ----- open CPLEX model -----
	int error;
	CPXENVptr env = CPXopenCPLEX(&error);
	if (error) print_error("CPXopenCPLEX() error");
	CPXLPptr lp = CPXcreateprob(env, &error, "TSP model version 1");
	if (error) print_error("CPXcreateprob() error");

	// ----- create TSP model -----
	build_model(inst, env, lp);

	// ----- Cplex's parameter setting -----
	set_params(inst, env);



	//choose formulation
	if (strcmp(inst->heuristic, "BENDERS") == 0) return benders();



	// free and close cplex model   
	CPXfreeprob(env, &lp);
	CPXcloseCPLEX(&env);

	return 0;

}