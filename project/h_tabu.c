#include "utils.h"

int TABU(instance *inst)
{
     if(VERBOSE >= 10) printf("--- Starting TABU ---\n");

    double lostTime = second() - inst->timeEnd;
    
    
    //vector with the new edge combination
    int* solution = (int*)calloc(inst->nnodes, sizeof(int));
    double cost = inst->zbest;


    memcpy(solution,inst->best_sol,sizeof(int)*inst->nnodes); 
        
    opt_2(inst, inst->timelimit, solution, &cost); //to change tl

         //new crossed edges
    plot(inst, solution,"TABU");

    inst->flagtabu=0;
   
    return 0;
}