#include "utils.h"
#define tmax 20



int opt_2(instance *inst, double tl, int *solution, double *cost){

    if(VERBOSE >= 10) printf("--- Starting OPT2 ---\n");
    
    if(!inst->flagCost)
        computeCost(inst);
    
    double delta;
    double oldCost = *cost; 


    do{
        delta = 0;
        int a = -1, b = -1;
        for(int i=0; i<inst->nnodes-1; i++)
                for(int j=i+1; j<inst->nnodes; j++)
                {
                   double deltaTemp = get_cost(i,j,inst)  +  get_cost(solution[j],solution[i],inst) -  (get_cost(i,solution[i],inst)  +  get_cost(j,solution[j],inst));
                    
                    if(deltaTemp < delta)
                    {   
                        delta = deltaTemp;
                        a = i;
                        b = j;
                     }
                    
                }
           

        if(delta < 0)  
        {
            reverse(inst,solution,a,b);
            *cost += delta;  //update cost
        }
            
     }while(!timeOut(inst, tl)  && delta < 0);

    
    if(VERBOSE >= 10) {
        if(checkSol(inst,solution)) return 1;
        if(checkCost(inst,solution,*cost)) return 1;
    }


    if(VERBOSE >= 10) printf("zbest: %f, cost: %f\n", inst->zbest,*cost);
    
    //update solution
    if(inst->zbest == -1 || inst->zbest > *cost){
        updateSol(inst,*cost,solution);
    }
    
    plot(inst,solution,"2OPT");

    if(VERBOSE >= 1) printf("OPT2 IMROVEMENT: old cost %f --> new cost %f\n",oldCost, *cost);
    
    return 0;
}