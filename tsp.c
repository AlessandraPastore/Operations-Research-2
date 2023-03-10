#include "tsp.h"

double dist(instance *inst,int index1,int index2) {

    return sqrt((inst->xcoord[index1]-inst->xcoord[index2])*(inst->xcoord[index1]-inst->xcoord[index2])+(inst->ycoord[index1]-inst->ycoord[index2])*(inst->ycoord[index1]-inst->ycoord[index2]));
 }

void computeCost(instance *inst)
 {
    int i=0;
    int j=0;
    for(i=0;i<inst->nnodes;i++)
        for(j=0;j<i;j++) 
            if(i!=j)
                inst->cost[i*inst->nnodes+j]=inst->cost[j*inst->nnodes+i]=dist(inst,i,j);

    inst->flagCost=1;
 }