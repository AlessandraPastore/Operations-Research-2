#include "utils.h"

//Sets in a and b the index of the nodes of the diameter (max cost)
double diameter(instance *inst, int *a, int *b)
{
    double maxCost = 0;
    for(int i = 0; i < inst->nnodes; i++) {
        for(int j=i+1; j < inst->nnodes; j++){
            double current = get_cost(i,j,inst);

            if(current > maxCost){
                maxCost = current;
                *a = i;
                *b = j;
            }
        }
    }

    return maxCost;
}

int extra_mileage(instance *inst){

    //compute costs
    if(!inst->flagCost) computeCost(inst);

    //vector with the solutions
    int* solution = (int*)calloc(inst->nnodes, sizeof(int));

    //0-1 vector to memorize nodes already part of the solution
    int *visited = (int*)calloc(inst->nnodes, sizeof(int));

    int a,b;
    double cost = 2*diameter(inst, &a, &b);


    visited[a] = visited[b] = 1;
    solution[a] = b;
    solution[b] = a;
    
    

    //take generic node, non visited
    for(int z=0; z<inst->nnodes; z++){
        if(visited[z]) continue;    //skip visited nodes

        double min = INFBOUND;
        int imin = -1;
        int jmin = -1;

        //for every visited  node i
        for(int i=0; i<inst->nnodes; i++){
            if(!visited[i]) continue;   //skip non visited nodes

            double extraM = get_cost(i,z,inst) + get_cost(z,solution[i],inst) - get_cost(i,solution[i],inst);
            
            if(extraM < min){
                min = extraM;
                imin = i;
            }

        }


        jmin = solution[imin];
        solution[imin] = z;
        solution[z] = jmin;
        visited[z]=1;

        cost += min;

        
    }

    inst->timeEnd = second();


    //update of best solution
    inst->zbest = cost;
    memcpy(inst->best_sol, solution, inst->nnodes * sizeof(int));

    if(VERBOSE >= 10) {
        checkSol(inst,solution);
        checkCost(inst,solution,cost);
        printf("BEST SOLUTION FOUND\nCOST: %f\n",inst->zbest);
    }

    plot(inst);

    free(visited);
    free(solution);

    return 0;
}