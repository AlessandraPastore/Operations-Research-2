#include "utils.h"

#define POPULATION 100


// member of the population
typedef struct {
    int* solution;  // tsp feasible tour
    double fitness; // score of the member
} member;

void initMember(instance *inst, member *population, int i){

        population[i].solution = (int*)calloc(inst->nnodes,sizeof(int));

        //inst->seed = rand();
        grasp(inst,0,0);

        //initialize each member with a random sol. from grasp
        population[i].solution = inst->best_sol;
        population[i].fitness = -inst->zbest;
        
        inst->zbest = -1;
}

int genetic(instance *inst){

    member *population = (member*)calloc(POPULATION, sizeof(member));

    // initialize population
    for(int i=0; i<POPULATION; i++){
        initMember(inst,population,i);
    }

    //to do


    
    free(population);
    return 0;
}