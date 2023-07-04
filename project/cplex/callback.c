#include "../utils/utils.h"
#include "../utils/tsp.h"
#include "../utils/mincut.h"

#define EPS 1e-5

typedef struct {
	CPXCALLBACKCONTEXTptr context;
	instance* inst;
} relaxation_callback_params;



static int violated_cuts_callback(double cutval, int num_nodes, int* members, void* param) {
	
	relaxation_callback_params* params = (relaxation_callback_params*)param;
	instance* inst = params->inst;
	CPXCALLBACKCONTEXTptr context = params->context;
	
	

	double rhs = num_nodes - 1;
	char sense = 'L';
	int matbeg = 0;
	int num_edges = num_nodes * (num_nodes - 1) / 2;
	
	double* values = (double*)malloc(num_edges* sizeof(double));
	int* edges = (int*)malloc(num_edges * sizeof(int));

	
	int k = 0;
	for (int i = 0; i < num_nodes; i++) {
		
		for (int j = 0; j < num_nodes; j++) {
			if (members[i] >= members[j]) { continue; } // undirected graph. If the node in index i is greated than the node in index j, we skip since (i,j) = (j,i)
			values[k] = 1.0;
			edges[k] = xpos(members[i], members[j], inst);
			k++;
			
		}
	}

	
	int purgeable = CPX_USECUT_FILTER;
	int local = 0;
	int status = CPXcallbackaddusercuts(context, 1, num_edges, &rhs, &sense, &matbeg, edges, values, &purgeable, &local);
	
	if (status) print_error("error on CPXcallbackaddusercuts");
	free(values);
	free(edges);

	printf("----------------io vado bene!-------------");
	return 0;
}

static int CPXPUBLIC my_callback_candidate(CPXCALLBACKCONTEXTptr context, CPXLONG contextid, void* userhandle)
/********************************************************************************************************/
{
	instance* inst = (instance*)userhandle;
	double* xstar = (double*)malloc(inst->ncols * sizeof(double));
	double objval = CPX_INFBOUND;
	if (CPXcallbackgetcandidatepoint(context, xstar, 0, inst->ncols - 1, &objval)) print_error("CPXcallbackgetcandidatepoint error");

	int mythread = -1; CPXcallbackgetinfoint(context, CPXCALLBACKINFO_THREADID, &mythread);
	int mynode = -1; CPXcallbackgetinfoint(context, CPXCALLBACKINFO_NODECOUNT, &mynode);
	double incumbent = CPX_INFBOUND; CPXcallbackgetinfodbl(context, CPXCALLBACKINFO_BEST_SOL, &incumbent);
	if (VERBOSE >= 10) printf(" ... callback at node %5d thread %2d incumbent %10.2lf\n", mynode, mythread, incumbent);

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
		if (inst->refopt2 == 1) {

			//here solution may contain cross edge so we use 2_opt in order to improve it, and then post it to cplex
			int* succ1 = (int*)malloc(inst->nnodes * sizeof(int));

			//components vector
			int* comp1 = (int*)malloc(inst->nnodes * sizeof(int));
			int ncomp1 = ncomp;

			for (int i = 0; i < inst->nnodes; i++)
			{
				succ1[i] = succ[i];
				comp1[i] = comp[i];

			}



			refinement(inst, succ1, comp1, ncomp1, 0);
			double cost = 0;

			for (int i = 0; i < inst->nnodes; i++) {
				cost += get_cost(i, succ1[i], inst);
			}

			objval = cost;
			opt_2(inst, inst->timelimit / 10, succ1, &objval);



			// Reinit xstar to 0. We want to reuse it in order to avoid another memory allocation
			memset(xstar, 0.0, inst->ncols);

			int* ind = (int*)malloc(inst->ncols * sizeof(int));

			for (int j = 0; j < inst->ncols; j++) ind[j] = j;

			for (int i = 0; i < inst->nnodes; i++) xstar[xpos(i, succ1[i], inst)] = 1.0;
			if (CPXcallbackpostheursoln(context, inst->ncols, ind, xstar, objval, CPXCALLBACKSOLUTION_NOCHECK)) print_error("CPXcallbackpostheursoln() error");
			free(ind);
			free(succ1);
			free(comp1);
		}
		//for each component
		for (int k = 1; k <= ncomp; k++) {
			char sense = 'L'; // <= constraint
			double sk = 0.0;
			int nnz = 0;
			int izero = 0;

			addSEC(inst, comp, &sk, index, value, &nnz, k);
		
			double rhs = sk - 1;// |S|-1

			if (CPXcallbackrejectcandidate(context, 1, nnz, &rhs, &sense, &izero, index, value)) print_error("CPXcallbackrejectcandidate() error");

		}
	
		
	}
	
	

	free(succ);
	free(comp);
	free(xstar);
	return 0;

}

static int CPXPUBLIC my_callback_relaxation(CPXCALLBACKCONTEXTptr context, CPXLONG contextid, void* userhandle)
{
	int depth = 0;
	int node = -1;
	int threadid = -1;
	double objval = CPX_INFBOUND;

	instance* inst = (instance*)userhandle;
	double* xstar = (double*)malloc(inst->ncols * sizeof(double));

	CPXcallbackgetinfoint(context, CPXCALLBACKINFO_NODEDEPTH, &depth);
	CPXcallbackgetinfoint(context, CPXCALLBACKINFO_NODECOUNT, &node);
	CPXcallbackgetinfoint(context, CPXCALLBACKINFO_THREADID, &threadid);


	if (node % 10 != 0) return 0;

	long ncols = inst->ncols;
	
	int status = CPXcallbackgetrelaxationpoint(context, xstar, 0, ncols - 1, &objval);
	if (status)
	{
		print_error("error on CPXcallbackgetrelaxationpoint");
	}

	int numcomps = 0;
	int* elist = (int*)malloc(2*inst->ncols * sizeof(int)); // elist contains each pair of vertex such as (1,2), (1,3), (1,4), (2, 3), (2,4), (3,4) so in list becomes: 1,2,1,3,1,4,2,3,2,4,3,4
	
	
	
	int* compscount = NULL;
	int* comps = NULL;
	int loader = 0;

	int num_edges = 0;
	for (int i = 0; i < inst->nnodes; i++) {
		for (int j = i + 1; j < inst->nnodes; j++) {
			//if (fabs(xstar[x_udir_pos(i, j, inst->num_nodes)]) <= EPS) continue;
			elist[loader++] = i;
			elist[loader++] = j;
		}
	}

	// Checking whether or not the graph is connected with the fractional solution.
	status = CCcut_connect_components(inst->nnodes, inst->ncols, elist, xstar, &numcomps, &compscount, &comps);
	if (status)
	{
		print_error("error on CCcut_connect_components");
	}


	if (numcomps == 1) {
		
		relaxation_callback_params params = { .context = context, .inst = inst };
		// At this point we have a connected graph. This graph could not be a "tsp". We interpret the fractional
		// solution as capacity of a cut. A cut of a graph G is composed by S and T = V - S where V is the nodes set.
		// The capacity of the cut is the sum of all ingoing and outgoing edges of the cut. Since we have a TSP,
		// we want that for each cut, we have a capacity of 2 (i.e. one ingoing edge and one outgoing edge).
		// So we want to seek the cuts which don't have a capacity of 2. The cuts with capacity < 2 violates the 
		// constraints and we are going to add SEC to them.
		// NB: We use cutoff as 2.0 - EPS for numerical stability due the fractional values we obtain in the solution. 
		status = CCcut_violated_cuts(inst->nnodes, ncols, elist, xstar, 2.0 - EPS, violated_cuts_callback, &params);
		if (status) {
			print_error("error on  CCcut_violated_cuts ");
		}
		
	}
	else if (numcomps > 1) {
		
		int startindex = 0;

		int* components = (int*)malloc(inst->nnodes * sizeof(int));

		// Transforming the concorde's component format into our component format in order to use our addSEC function
		for (int subtour = 0; subtour < numcomps; subtour++) {

			for (int i = startindex; i < startindex + compscount[subtour]; i++) {
				int index = comps[i];
				components[index] = subtour + 1;
			}

			startindex += compscount[subtour];

		}

		int* indexes = (int*)malloc(inst->ncols * sizeof(int));
		double* values = (double*)malloc(inst->ncols * sizeof(double));
		int purgeable = CPX_USECUT_FILTER;
		int local = 0;

		

		for (int k = 1; k <= numcomps; k++) {
			char sense = 'L'; // <= constraint
			double sk = 0.0;
			int nnz = 0;
			int izero = 0;



			addSEC(inst, components, &sk, indexes, values, &nnz, k);

			double rhs = sk - 1;// |S|-1

			if (CPXcallbackaddusercuts(context, 1, nnz, &rhs, &sense, &izero, indexes, values, &purgeable, &local))
				print_error("CPXcallbackaddusercuts() error"); // add one violated usercut 

		}
		
		
		free(indexes);
		free(values);
		free(components);

	}


	free(xstar);
	free(elist);
	free(compscount);
	free(comps);
	
	return 0;
}

int CPXPUBLIC my_callback(CPXCALLBACKCONTEXTptr context, CPXLONG contextid, void* userhandle)
{
	instance* inst = (instance*)userhandle;
	if (contextid == CPX_CALLBACKCONTEXT_CANDIDATE) {
		return my_callback_candidate(context, contextid, inst);
	}
	if (contextid == CPX_CALLBACKCONTEXT_RELAXATION) {
		return my_callback_relaxation(context, contextid, inst);
	}
	return 1;

}




int callback_sec(instance* inst, CPXENVptr env, CPXLPptr lp) {

	printf("--- starting B&C via callback ---\n");

	int ncols = CPXgetnumcols(env, lp);
	inst->ncols = ncols;
	//to store the solution
	double* xstar = (double*)calloc(ncols, sizeof(double));

	CPXsetdblparam(env, CPX_PARAM_TILIM, inst->timelimit);
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
			plot(inst, succ, "lazycut");
			return 0;
		}
		else print_error("CPXgetx() error");
	}
	build_sol(xstar, inst, succ, comp, &ncomp);
	
	
	for (int i = 0; i < inst->nnodes; i++)
	{
		for (int j = i + 1; j < inst->nnodes; j++)
		{
			if (xstar[xpos(i, j, inst)] > 0.5)  printf("  ... x(%3d,%3d) = 1\n", i + 1, j + 1); 
		}
	}
	
	double objval = -1;
	if (CPXgetobjval(env, lp, &objval)) {
		if (inst->zbest != -1) {
			printf("----- Terminated before convergence -----\n");
			printf("BEST SOLUTION FOUND\nCOST: %f\n", inst->zbest);
			plot(inst, succ, "lazycut");
			return 0;
		}
		else print_error("CPXgetx() error");
	}

	if (VERBOSE >= 10) {
		if (checkCost(inst, succ, objval)) return 1;
		if (checkSol(inst, succ)) return 1;
		
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


int callback_relaxation(instance* inst, CPXENVptr env, CPXLPptr lp) {

	printf("--- starting Concorde ---\n");

	int ncols = CPXgetnumcols(env, lp);
	inst->ncols = ncols;
	//to store the solution
	double* xstar = (double*)calloc(ncols, sizeof(double));

	CPXsetdblparam(env, CPX_PARAM_TILIM, inst->timelimit);
	CPXLONG contextid = contextid = CPX_CALLBACKCONTEXT_CANDIDATE | CPX_CALLBACKCONTEXT_RELAXATION; // ... context for concorde

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
			plot(inst, succ, "relaxationCallback");
			return 0;
		}
		else print_error("CPXgetx() error");
	}
	build_sol(xstar, inst, succ, comp, &ncomp);


	for (int i = 0; i < inst->nnodes; i++)
	{
		for (int j = i + 1; j < inst->nnodes; j++)
		{
			if (xstar[xpos(i, j, inst)] > 0.5)  printf("  ... x(%3d,%3d) = 1\n", i + 1, j + 1);
		}
	}

	double objval = -1;
	if (CPXgetobjval(env, lp, &objval)) {
		if (inst->zbest != -1) {
			printf("----- Terminated before convergence -----\n");
			printf("BEST SOLUTION FOUND\nCOST: %f\n", inst->zbest);
			plot(inst, succ, "relaxationCallback");
			return 0;
		}
		else print_error("CPXgetx() error");
	}

	if (VERBOSE >= 10) {
		if (checkCost(inst, succ, objval)) return 1;
		if (checkSol(inst, succ)) return 1;

	}

	//if current cost is better, update best solution
	if (inst->zbest == -1 || inst->zbest > objval) {
		updateSol(inst, objval, succ);
	}

	if (VERBOSE >= 1) printf("BEST SOLUTION FOUND\nCOST: %f\n", inst->zbest);
	plot(inst, succ, "relaxationCallback");


	free(succ);
	free(comp);
	free(xstar);
	return 0;
}