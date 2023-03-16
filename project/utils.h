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
int extra_mileage(instance *inst);
int grasp(instance *inst,int greedy);
int heuristic(instance *inst);
int opt_2(instance *inst);

#endif