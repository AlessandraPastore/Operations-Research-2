#include "utils.h"

//reverse path b -> a1
void reverse(instance *inst, int *solution, int a,int b)
{
    printf("SWAP 2OPT: %d with %d\n",a,b);
    int a1 = solution[a];
    int b1 = solution[b];

    int* old = (int*)malloc(inst->nnodes * sizeof(int));
    memcpy(old,solution,sizeof(int)*inst->nnodes);

    
    solution[a]=b;
    solution[a1]=b1;
    
    int i=a1;
    
    while (i!=b)
        {
            solution[old[i]] = i;
            i = old[i];
        }

    free(old);

}

int opt_2(instance *inst, double tl, int *solution, double *cost){

    if(VERBOSE >= 10) printf("--- Starting OPT2 ---\n");

    double lostTime = second() - inst->timeEnd;
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

            *cost += delta;  //update cost
            
            if(delta < 0)
                reverse(inst,solution,a,b);
                

    } while(!timeOut(inst, tl + lostTime)  && delta < 0);

    inst->timeEnd = second() - lostTime; //removes the time of plot greedy
    
    
    if(VERBOSE >= 10) {
        if(checkSol(inst,solution)) return 1;
        if(checkCost(inst,solution,*cost)) return 1;
    }


    if(VERBOSE >= 10) printf("zbest: %f, cost: %f\n", inst->zbest,*cost);
    
    //update solution
    if(inst->zbest == -1 || inst->zbest > *cost){
        updateSol(inst,*cost,solution);
    }
    
    

    if(VERBOSE >= 1) printf("OPT2 IMROVEMENT: old cost %f --> new cost %f\n",oldCost, *cost);

    plot(inst, solution,"2_opt");
  
    return 0;
}