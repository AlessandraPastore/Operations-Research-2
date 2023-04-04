#include "utils.h"

//void reverse(instance *inst, int* old, int a1,int b);
//int opt_2(instance *inst, double tl);

//reverse path b -> a1
void reverse2(int *solution, int *old, int a1, int b)
{   
    int i = a1;
      
    while (i != b)
        {
            solution[old[i]] = i;
            i = old[i];
        }
}


void kick(instance *inst, int* solution, double* cost){
    int a,b,c,d,e;
    int a1,b1,c1,d1,e1;

    //select 5 random edges
    a = rand() % inst->nnodes;

    do{ b = rand() % inst->nnodes; }while(a == b);
    do{ c = rand() % inst->nnodes; }while(a == c || b == c);
    do{ d = rand() % inst->nnodes; }while(a == d || b == d || c == d);
    do{ e = rand() % inst->nnodes; }while(a == e || b == e || c == e || d == e);

    //order the nodes based on current tour
    int start = a;
    int curr = solution[a];
    int count = 1;
    int order[5];
    order[0] = start;

    while (curr != a) {
        if (curr == b) order[count++] = b;
        else if (curr == c) order[count++] = c;
        else if (curr == d) order[count++] = d;
        else if (curr == e) order[count++] = e;

        if (count == 5) break;
        curr = solution[curr];
    }

    //save corresponding pairs in order
    a1 = solution[order[0]];
    b1 = solution[order[1]];
    c1 = solution[order[2]];
    d1 = solution[order[3]];
    e1 = solution[order[4]];

    //removes old edge cost 
    *cost = *cost - get_cost(order[0],a1,inst) 
                    - get_cost(order[1],b1,inst) 
                    - get_cost(order[2],c1,inst) 
                    - get_cost(order[3],d1,inst) 
                    - get_cost(order[4],e1,inst);

    int* old = (int*)malloc(inst->nnodes * sizeof(int));
    memcpy(old,solution,sizeof(int)*inst->nnodes);

    //swaps the edges to create a new path
    solution[order[0]] = order[3];
    solution[c1] = order[1];
    solution[a1] = d1;
    solution[order[4]] = b1;
    solution[order[2]] = e1;

    reverse2(solution, old,a1,order[1]);
    reverse2(solution, old, c1,order[3]);

    //add new edge cost
    *cost = *cost + get_cost(order[0],order[3],inst) 
                    + get_cost(order[1],c1,inst) 
                    + get_cost(a1,d1,inst) 
                    + get_cost(order[4],b1,inst) 
                    + get_cost(order[2],e1,inst);

    free(old);

}



int VNS(instance *inst){

    if(VERBOSE >= 10) printf("--- Starting VNS ---\n");

    if(!inst->flagCost)
        computeCost(inst);

    if(inst->seed != -1) 
        srand(inst->seed);

    //vector with the new edge combination
    int* solution = (int*)calloc(inst->nnodes, sizeof(int));
    double cost = inst->zbest;
    
    double oldCost;

    //solution copies the actual local minimum
    memcpy(solution,inst->best_sol,sizeof(int)*inst->nnodes);

    do{

        oldCost = cost;

        kick(inst,solution,&cost);
        kick(inst,solution,&cost);

        if(VERBOSE >= 10) {
            if(checkSol(inst,solution)) return 1;
            if(checkCost(inst,solution,cost)) return 1;
        }

        if(VERBOSE >= 10) printf("VNS REGRESSION: old cost %f --> new cost %f\n", oldCost,cost );
        
        //new crossed edges
        plot(inst, solution,"VNS_kick");

        //printf("- - entering opt2 - -\n");
        opt_2(inst, inst->timelimit, solution, &cost); //to change tl


    } while (!timeOut(inst, inst->timelimit));
    
    
    free(solution);
    

    if(VERBOSE >= 1) printf("VNS BEST SOL COST: %f \n", inst->zbest);
    
    //best sol found
    plot(inst, inst->best_sol,"VNS_BestSol");

    
    
    return 0;
}
