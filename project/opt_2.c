#include "tsp.h"
void reverse(instance *inst,int i,int j)
{
    int* old = (int*)malloc(inst->nnodes * sizeof(int));
    memcpy(old,inst->best_sol,sizeof(int)*inst->nnodes);
    inst->best_sol[i]=j;
    inst->best_sol[inst->best_sol[j]]=inst->best_sol[j];
    int k=inst->best_sol[i];
    while (k!=j)
        {
            inst->best_sol[old[k]]=k;
            k=old[k];
        }

    free(old);

}
int opt_2(instance *inst){
    double t=second();
    
    double delta=INFBOUND;
    int swap1=-1,swap2=-1;
    do{
        for(int i=0;i<inst->nnodes;i++)
            for(int j=0;j<inst->nnodes;j++)
            {
                if(i!=j){
                    
                    double deltaTemp = get_cost(i,j,inst)  +  get_cost(inst->best_sol[j],inst->best_sol[i],inst) -  (get_cost(i,inst->best_sol[i],inst)  +  get_cost(j,inst->best_sol[j],inst));
                    
                    if(deltaTemp<delta)
                    {
                        delta=deltaTemp;
                        swap1=i;
                        swap2=j;
                    }
                }
                
            }
        reverse(inst,swap1,swap2);

    }while(second()-t<inst->timelimit);
    
    
    return 0;
}