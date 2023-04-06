#include "utils.h"
#include "math.h"


#define alpha 0.99
#define e 2.71828



int annealing(instance *inst,double tl)
{
    if(VERBOSE >= 10) printf("--- Starting SIMULATED ANNEALING ---\n");
    if(!inst->flagCost)
        computeCost(inst);
    double Tmax= (inst->zbest/inst->nnodes)*4;

    double oldCost = inst->zbest;
    double newCost = oldCost;
    

    //vector with the new edge combination
    int* solution = (int*)calloc(inst->nnodes, sizeof(int));
   

    //solution copies the actual local minimum
    memcpy(solution,inst->best_sol,sizeof(int)*inst->nnodes);

    int i,j; 

    int T=Tmax;
   
    do{
    
        i=rand()%inst->nnodes;
        do
        {
            j=rand()%inst->nnodes;

        } while (i==j);
        

        double deltaTemp = get_cost(i,j,inst)  +  get_cost(solution[j],solution[i],inst) -  (get_cost(i,solution[i],inst)  +  get_cost(j,solution[j],inst));    

        double prob=pow(e,(-deltaTemp/T));
        int random=rand() % 10;
       
        if( random< prob*10)
        {
            reverse(inst,solution,i,j);
            newCost += deltaTemp;
        }

        T= T*alpha;
       

        


        if(VERBOSE >= 10) {
            if(checkSol(inst,solution)) return 1;
            if(checkCost(inst,solution,newCost)) return 1;
        }

        //update solution
        if(inst->zbest == -1 || inst->zbest > newCost){
            updateSol(inst,newCost,solution);
        }
        
        

     }while(!timeOut(inst, tl));

    plot(inst,solution,"annealing");


    if(VERBOSE >= 10) printf("zbest: %f, cost: %f\n", inst->zbest,newCost);

    if(VERBOSE >= 1 && newCost<oldCost) 
        printf("Tabu IMROVEMENT: old cost %f --> new cost %f\n",oldCost, newCost);
    else
        printf("no improvment\n");
    return 0;
}
