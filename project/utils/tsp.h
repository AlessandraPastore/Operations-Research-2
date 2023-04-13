#pragma once

#ifndef TSP_H_

#define TSP_H_

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <dirent.h>

//to re define
#define VERBOSE				    50	// printing level  (=10 only incumbent, =20 little output, =50-60 good, =70 verbose, >=100 cplex log)

//hard-wired parameters
#define XSMALL		  		  1e-5 		// 1e-4*	// tolerance used to decide ingerality of 0-1 var.s
#define EPSILON		  		  1e-9		// 1e-9		// very small numerical tolerance
#define TICKS_PER_SECOND 	  1000.0  	// cplex's ticks on Intel Core i7 quadcore @2.3GHZ
#define INFBOUND                2147483647

//#define GRASP_RAND 0.5

//Instance
typedef struct {

	//input data
	int nnodes;
	double* xcoord;
	double* ycoord;
	//int* index;

	// parameters
	double timelimit;						// overall time limit, in sec.s
	char input_file[1000];		  			// input file
	char heuristic[25];                     // name of the heuristic to use
	char cplex[25];                     // name of the heuristic to use
	char weight_type[10];                   // weight type for distance function

	int seed;                               //random seed

	double timeStart;                       //start of the computation
	double timeEnd;                         //end of the computation


	//costs 
	double* cost;

	//flag to see if the costs array is set, 0 if not set
	int flagCost;

	//best sol.
	double zbest;							// cost of best sol. available
	double tbest;							// time for the best sol. available
	int* best_sol;						// best sol. available
	//double	best_lb;						// best lower bound available

	// model; starting point
	int xstart;
	int ystart;
	int indexStart;

} instance;


//inline
double dist(instance* inst, int index1, int index2);
void computeCost(instance* inst);
double get_cost(int i, int j, instance* inst);
void plot(instance* inst, int* solution, char name[]);
int timeOut(instance* inst, double tl);
int checkSol(instance* inst, int* sol);
int checkCost(instance* inst, int* sol, double c);
void updateSol(instance* inst, double cost, int* solution);
void reverse(instance* inst, int* solution, int a, int b);

//cplex
int xpos(int i, int j, instance* inst);
void build_sol(const double* xstar, instance* inst, int* succ, int* comp, int* ncomp); // build succ() and comp() wrt xstar()...


#endif   /* TSP_H_ */