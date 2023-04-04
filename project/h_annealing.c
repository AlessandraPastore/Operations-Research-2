#include "utils.h"

#define alpha 0.99
#define e 2.71828
#define Tmax 5000
#define Tmin 100


int annealing(instance *inst,double tl)
{
    if(VERBOSE >= 10) printf("--- Starting SIMULATED ANNEALING ---\n");
    if(!inst->flagCost)
        computeCost(inst);
    
    double delta;
    double oldCost = inst->zbest;
    double newCost = oldCost;
    

    //vector with the new edge combination
    int* solution = (int*)calloc(inst->nnodes, sizeof(int));
   

    //solution copies the actual local minimum
    memcpy(solution,inst->best_sol,sizeof(int)*inst->nnodes);

    int i,j; 

    int T=Tmax;
    int iteration=0;
    do{
    
        i=rand()%inst->nnodes;
        do
        {
            j=rand()%inst->nnodes;

        } while (i==j);
        

        double deltaTemp = get_cost(i,j,inst)  +  get_cost(solution[j],solution[i],inst) -  (get_cost(i,solution[i],inst)  +  get_cost(j,solution[j],inst));    

        int prob=pow(e,(-delta/T));

        if(rand() % 10 < prob*10)
        {
            reverse(inst,solution,i,j);
            newCost += deltaTemp;
        }

        T= T*pow(alpha,iteration)+Tmin;
        iteration++;

        if(T<Tmin)
            T=Tmax; 


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
