#include "tsp.h"
#define GRASP_RAND 0.8
double dist(instance *inst,int index1,int index2) {

    return sqrt((inst->xcoord[index1]-inst->xcoord[index2])*(inst->xcoord[index1]-inst->xcoord[index2])+(inst->ycoord[index1]-inst->ycoord[index2])*(inst->ycoord[index1]-inst->ycoord[index2]));
 }

int greedy(instance *inst,int startNode)
{
    //vector with the solutions
    int* solution;
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
        for(i=j+1;i<inst->nnodes;i++)
        {
            double actualDist=dist(inst,j,i);
            if(actualDist<minDist)
            {
                minDist=actualDist;
                minIndex=i;
            }
        }
        cost+=minDist;
        //now I swap
        swap=solution[j+1];
        solution[j+1]=minIndex;
        solution[minIndex]=swap;
    }
    return cost;
}

int grasp(instance *inst, int startNode)
{
    return 0;//must be changed
}