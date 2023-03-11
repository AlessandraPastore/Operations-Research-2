#pragma once

#ifndef TSP_H_

#define TSP_H_

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>


#define VERBOSE				    50		// printing level  (=10 only incumbent, =20 little output, =50-60 good, =70 verbose, >=100 cplex log)

//hard-wired parameters
#define XSMALL		  		  1e-5 		// 1e-4*	// tolerance used to decide ingerality of 0-1 var.s
#define EPSILON		  		  1e-9		// 1e-9		// very small numerical tolerance
#define TICKS_PER_SECOND 	  1000.0  	// cplex's ticks on Intel Core i7 quadcore @2.3GHZ
#define INFBOUND                99999999999

#define GRASP_RAND 0.8

//data structures

typedef struct {

    //input data
    int nnodes;
    double* xcoord;     
    double* ycoord;
    int* index;

    // parameters
    double timelimit;						// overall time limit, in sec.s
    char input_file[1000];		  			// input file
   

    //costs 
    double* cost;

    //flag to see if the costs array is set, 0 if not set
    int flagCost;

    //best sol.
    double zbest;							// cost of best sol. available
    double tbest;							// time for the best sol. available
    int* best_sol;						// best sol. available
    double	best_lb;						// best lower bound available

    // model; starting point
    int xstart;
    int ystart;
    int indexStart;
 
} instance;

//inline
inline int imax(int i1, int i2) { return (i1 > i2) ? i1 : i2; }
inline double dmin(double d1, double d2) { return (d1 < d2) ? d1 : d2; }
inline double dmax(double d1, double d2) { return (d1 > d2) ? d1 : d2; }
double dist(instance *inst,int index1,int index2);
void computeCost(instance *inst);
double get_cost(int i, int j, instance *inst);
#endif   /* TSP_H_ */

