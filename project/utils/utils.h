#pragma once

#ifndef HEUR

#define HEUR

#include "tsp.h"


//chrono.c
double second();

//tsp.c
void print_error(const char* err);
void read_input(instance* inst);
void parse_command_line(int argc, char** argv, instance* inst);
void free_instance(instance* inst);

//heuristics
int heuristic(instance* inst);

int extra_mileage(instance* inst, int diam);
void computeExtra(instance* inst, int* visited, int* solution, double* cost);

int grasp(instance* inst, int greedy, double tl);
int opt_2(instance* inst, double tl, int* solution, double* cost);

int VNS(instance* inst);
void reverse2(int* solution, int* old, int a1, int b);

int TABU(instance* inst, double tl);
int genetic(instance* inst);
int annealing(instance* inst, double tl);

int refinement(instance* inst, int* succ, int* comp, int ncomp, int iter);


int performance(instance* inst);


//cplex
int TSPopt(instance* inst);
int benders(instance* inst, CPXENVptr env, CPXLPptr lp);

int CPXPUBLIC my_callback(CPXCALLBACKCONTEXTptr context, CPXLONG contextid, void* userhandle);

#endif