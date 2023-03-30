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
int heuristic(instance *inst);

int extra_mileage(instance *inst);
int grasp(instance *inst, int greedy, double tl);
int opt_2(instance *inst, double tl, int *solution, double *cost);
int VNS(instance *inst);
int TABU(instance *inst);

int performance(instance *inst);

#endif