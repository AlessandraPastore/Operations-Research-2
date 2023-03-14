#include "tsp.h"
int greedy(instance *inst,int startNode);
int extra_mileage(instance *inst);

int heuristic(instance *inst){
    if(strcmp(inst->heuristic, "GREEDY"))           return greedy(inst,0);
    if(strcmp(inst->heuristic, "GREEDYGRASP"))      return greedy(inst,0);
    if(strcmp(inst->heuristic, "EXTRA_MILEAGE"))    return extra_mileage(inst);

    return 1;
}