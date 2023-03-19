#include "utils.h"
void reverse(instance *inst,int a,int b)
{
    int a1=inst->best_sol[a],b1=inst->best_sol[b];
    int* old = (int*)malloc(inst->nnodes * sizeof(int));
    
    memcpy(old,inst->best_sol,sizeof(int)*inst->nnodes);

    
    inst->best_sol[a]=b;
    inst->best_sol[a1]=b1;
    
    int i=a1;
    
    while (i!=b)
        {
            inst->best_sol[old[i]] = i;
            i = old[i];
        }

    free(old);

}

int opt_2(instance *inst){

    if(VERBOSE >= 10) printf("--- Starting OPT2 ---\n");
    double t = second();
    double delta;

    double oldCost = inst->zbest;
    double cost = oldCost;

    do{
        delta = 0;
        int a = -1, b = -1;
        for(int i=0; i<inst->nnodes-1; i++)
            for(int j=i+1; j<inst->nnodes; j++)
            {
                     double deltaTemp = get_cost(i,j,inst)  +  get_cost(inst->best_sol[j],inst->best_sol[i],inst) -  (get_cost(i,inst->best_sol[i],inst)  +  get_cost(j,inst->best_sol[j],inst));
                    
                    if(deltaTemp < delta)
                    {   
                        delta = deltaTemp;
                        a = i;
                        b = j;
                    }
            }

            //inst->zbest += delta;
            cost += delta;  //update cost
            
            if(delta < 0)
                reverse(inst,a,b);

    } while(second()-t < inst->timelimit && delta < 0);
    
    if(VERBOSE >= 10)checkSol(inst,inst->best_sol);
    if(VERBOSE >= 10)checkCost(inst,inst->best_sol,cost);

    inst->zbest = cost;
    if(VERBOSE >= 10) printf("OPT2 IMROVEMENT: old cost %f --> new cost %f\n",oldCost, inst->zbest);

    plot(inst);
  
    return 0;
}