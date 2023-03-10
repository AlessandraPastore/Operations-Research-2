#include "tsp.h"
double dist(instance *inst,int index1,int index2) {

    return sqrt((inst->xcoord[index1]-inst->xcoord[index2])*(inst->xcoord[index1]-inst->xcoord[index2])+(inst->ycoord[index1]-inst->ycoord[index2])*(inst->ycoord[index1]-inst->ycoord[index2]));
 }
 void computeCost(instance *inst)
 {
    int i=0;
    int j=0;
    for(i=0;i<inst->nnodes;i++)
        for(j=0;j<inst->nnodes;j++) 
            if(i!=j)
                inst->cost[i*inst->nnodes+j]=dist(inst,i,j);

    inst->flagCost=1;
 }