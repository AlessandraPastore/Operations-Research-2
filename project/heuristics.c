#include "utils.h"


int heuristic(instance *inst){
    if(strcmp(inst->heuristic, "GREEDY") == 0)           return grasp(inst,1);
    if(strcmp(inst->heuristic, "GREEDYGRASP") == 0)      return grasp(inst,0);
    if(strcmp(inst->heuristic, "EXTRAMILEAGE") == 0)    return extra_mileage(inst);

    return 1;
}