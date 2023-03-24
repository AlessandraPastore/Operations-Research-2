#include "utils.h"


int heuristic(instance *inst){
    if(strcmp(inst->heuristic, "GREEDY") == 0)           return grasp(inst, 1, inst->timelimit);
    if(strcmp(inst->heuristic, "GREEDYGRASP") == 0)      return grasp(inst, 0, inst->timelimit);
    if(strcmp(inst->heuristic, "EXTRAMILEAGE") == 0)    return extra_mileage(inst);
    if(strcmp(inst->heuristic, "2_OPT") == 0)   {
        grasp(inst, 0, inst->timelimit / 5); //to modify, how much time do we actually want to use?
        return opt_2(inst, inst->timelimit, inst->best_sol, &(inst->zbest)); //TIME DA MODIFICARE
    }
    if(strcmp(inst->heuristic, "VNS") == 0)   {
        
        grasp(inst, 0, inst->timelimit / 5); //to modify, how much time do we actually want to use?
        opt_2(inst, inst->timelimit, inst->best_sol, &(inst->zbest)); //TIME DA MODIFICARE
        return VNS(inst);

    }

    print_error("heuristic name not appropriate");
    return 1;
}