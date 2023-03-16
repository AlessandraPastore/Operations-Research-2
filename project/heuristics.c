#include "tsp.h"
int greedy(instance *inst,int startNode);
int grasp(instance *inst,int startNode);
int extra_mileage(instance *inst);
int opt_2(instance *inst);

int heuristic(instance *inst){
    if(strcmp(inst->heuristic, "GREEDY") == 0)           return greedy(inst,0);
    if(strcmp(inst->heuristic, "GREEDYGRASP") == 0)      return grasp(inst,0);
    if(strcmp(inst->heuristic, "EXTRA_MILEAGE") == 0)    return extra_mileage(inst);
    if(strcmp(inst->heuristic, "2_OPT") == 0)   {
        grasp(inst,0);
        return opt_2(inst);
     }
    return 1;
}