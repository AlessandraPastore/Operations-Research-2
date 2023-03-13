#include "tsp.h"
#define GRASP_RAND 0.8

void updateCost(instance *inst, int cost, int* solution)
{
    inst->zbest=cost;
    int i,j;
    for(i=0;i<inst->nnodes;i++)
        for(j=0;j<inst->nnodes;j++)
            if(solution[j]==i)
            {
                if(j+1<inst->nnodes)
                    inst->best_sol[i]=solution[j+1];
                    else
                    inst->best_sol[i]=solution[0];
                j=inst->nnodes;
            }
}
int greedy(instance *inst,int startNode)
{
    if(inst->flagCost==0)
        computeCost(inst);
    //vector with the solutions
    int* solution;
    solution=(int*)calloc(inst->nnodes, sizeof(int));
    int i;
    for(i=0;i<inst->nnodes;i++)
        solution[i]=i;
    
    //now I swap the first element with the starting node pass to the function
    solution[0]=startNode;
    solution[startNode]=0;
    int minIndex=0;
    double minDist=2147483647;
    int j=0;
    int cost=0;
    int swap;
    for(j=0;j<inst->nnodes-1;j++)
    {
        minDist=2147483647;
        for(i=j+1;i<inst->nnodes;i++)
        {
            double actualDist=get_cost(solution[i],solution[j],inst);
            if( actualDist<minDist)
            {
                minDist=actualDist;
                minIndex=i;
            }
        }
        cost+=minDist;
        //now I swap
        swap=solution[j+1];
        solution[j+1]=solution[minIndex];
        solution[minIndex]=swap;
    }
    for(i=0;i<inst->nnodes;i++)
        printf("\n besto sol: %d\n",solution[i]);
    if(inst->zbest=-1 || inst->zbest>cost)
        updateCost(inst,cost,solution);
    plot(inst);
    return 0;
}

int grasp(instance *inst, int startNode)
{
    return 0;//must be changed
}