#include "tsp.h"
double second();

void updateCost(instance *inst, double cost, int* solution)
{
    if(VERBOSE >= 10) printf("-------- Updating best solution --------\n");
    inst->zbest = cost;
    memcpy(inst->best_sol, solution, inst->nnodes * sizeof(int));
    if(VERBOSE >= 10)checkSol(inst);
}

int greedy(instance *inst,int startNode)
{
    if(!inst->flagCost)
        computeCost(inst);

    if(inst->seed != -1) 
        srand(inst->seed);

    //vector with the solutions
    int* solution = (int*)calloc(inst->nnodes, sizeof(int));

    //0-1 vector to memorize nodes already part of the solution
    int *visited = (int*)malloc(inst->nnodes * sizeof(int));

    do{

        //puts visited all at zeros
        memset(visited, 0, inst->nnodes * sizeof(int));
        

        //init
        int minIndex = -1;
        double minDist = INFBOUND;

        //start node at random
        int current = rand() % inst->nnodes;
        int start = current;

        visited[current] = 1;
        
        double cost = 0;

        for(int j=0; j<inst->nnodes-1; j++)
        {
            for(int i=0; i<inst->nnodes; i++)
            {
                if(visited[i]) continue;    //I skip nodes already part of the solution

                double actualDist = get_cost(current,i,inst);

                //if actual dist is better, update
                if(actualDist < minDist)
                {
                    minDist = actualDist;
                    minIndex = i;
                }
            }

            
            cost += minDist;
            
            //now I add the new edge
            solution[current] = minIndex;
            visited[minIndex] = 1;

            current = minIndex;
            minDist = INFBOUND;
        }

        //close the circuit last-first
        solution[current] = start;

        if(VERBOSE >= 10) printf("current cost: %f\n", cost);

        //if current cost is better, update best solution
        if(inst->zbest == -1 || inst->zbest > cost){
            updateCost(inst,cost,solution);
            inst->indexStart = start;
        }
        
    } while (!timeOut(inst));
    
    printf("BEST SOLUTION FOUND\nSTART: %d     COST: %f\n",inst->indexStart, inst->zbest);
    inst->timeEnd = second();
    plot(inst);
    return 0;
}

int grasp(instance *inst, int startNode)
{
    if(!inst->flagCost)
        computeCost(inst);

    if(inst->seed != -1) 
        srand(inst->seed);
        
    //vector with the solutions
    int* solution = (int*)calloc(inst->nnodes, sizeof(int));

    //0-1 vector to memorize nodes already part of the solution
    int *visited = (int*)malloc(inst->nnodes * sizeof(int));

    do {

        //puts visited all at zeros
        memset(visited, 0, inst->nnodes * sizeof(int));

        //init
        int minIndex = -1;
        int minIndex2 = -1;

        double minDist = INFBOUND;
        double minDist2 = INFBOUND;

        //start node at random
        int current = rand() % inst->nnodes;
        int start = current;

        printf("start %d - ",current);

        visited[current] = 1;
        
        double cost = 0;
        
        for(int j=0; j<inst->nnodes-1; j++)
        {
            for(int i=0; i<inst->nnodes; i++)
            {
                if(visited[i] == 1) continue;    //I skip nodes already part of the solution

                
                double actualDist = get_cost(current,i,inst);

                //if actual dist is better, update
                if( actualDist < minDist)
                {
                    //the current min becomes the second min
                    minDist2 = minDist; 
                    minIndex2 = minIndex;

                    //new min
                    minDist = actualDist;
                    minIndex = i;
                }
                else if(actualDist < minDist2)
                {
                    //new second min
                    minDist2 = actualDist;
                    minIndex2 = i;
                }
            }
            
            if((rand() % 10) <= (GRASP_RAND * 10))
            {
                cost += minDist;
                
                //now I add the new edge
                solution[current] = minIndex;
                visited[minIndex] = 1;

                current = minIndex;
            }
            else
            {
                //last iter with only one node to pair (no second min)
                if(j == inst->nnodes-2) minDist2 = minDist;

                cost += minDist2;
                
                //now I add the new edge
                solution[current] = minIndex2;
                visited[minIndex2] = 1;

                current = minIndex2;
            }

            minDist = INFBOUND;
            minDist2 = INFBOUND;
        }

        //close the circuit last-first
        solution[current] = start;

        if(VERBOSE >= 10) printf("current cost: %f\n", cost);

        //if current cost is better, update best solution
        if(inst->zbest == -1 || inst->zbest > cost){
            updateCost(inst,cost,solution);
            inst->indexStart = start;
        }
        if(cost > 100000000){
            updateCost(inst,cost,solution);
            inst->indexStart = start;
            break;
        }
    } while (!timeOut(inst));

    printf("BEST SOLUTION FOUND\nSTART: %d     COST: %f\n",inst->indexStart, inst->zbest);
    inst->timeEnd = second();
    plot(inst);
    return 0;
}