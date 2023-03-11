#include "tsp.h"


void init_extra_mileage(instance *inst, int* visited);
void diameter(instance *inst, int *a, int *b);
int extra_mileage(instance *inst);
void plot(instance *inst);


void init_extra_mileage(instance *inst, int* visited){
    
    //compute costs
    if(!inst->flagCost) computeCost(inst);


    //find diameter and put it on visited and init best_sol
    int a,b;
    diameter(inst, &a, &b);

    visited[a] = visited[b] = 1;
    inst->best_sol[a] = b;
    inst->best_sol[b] = a;

    printf("best sol: %d %d\n", a, b);


}

//Sets in a and b the index of the nodes of the diameter (max cost)
void diameter(instance *inst, int *a, int *b)
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
}



int extra_mileage(instance *inst){


    int *visited = (int*)calloc(inst->nnodes, sizeof(int));
    inst->best_sol = (int *) calloc(inst->nnodes, sizeof(double));

    init_extra_mileage(inst,visited);

    int selected = 2;

/*
    //keep adding edges until all nodes are covered
    while(selected != inst->nnodes) {

        //scan selected edges
        //search for a visited node
        for(int i=0; i<inst->nnodes; i++){
            if(!visited[i]) continue;   //skip non visited nodes

            //search for a visited node with connected edge
            for(int j=0; j<inst->nnodes; j++){
                if(!visited[j]) continue;   //skip non visited nodes
                if(i == j) continue;    //skip auto loops
                if(!inst->best_sol[i*inst->nnodes + j]) continue; //skip non edges

                //search for the node z such that c_ij > c_iz + c_zj
                for(int z=0; z<inst->nnodes; z++){
                    if(visited[z]) continue;    //skip visited nodes

                    double newCost = get_cost(i,z,inst) + get_cost(z,j,inst) - get_cost(i,j,inst);


                }
                
            }
        }
    }
*/

    //take generic node, non visited
    for(int z=0; z<inst->nnodes; z++){
        if(visited[z]) continue;    //skip visited nodes

        double min = INFBOUND;
        int imin = -1;
        int jmin = -1;

        //for every visited  node i
        for(int i=0; i<inst->nnodes; i++){
            if(!visited[i]) continue;   //skip non visited nodes

            double extraM = get_cost(i,z,inst) + get_cost(z,inst->best_sol[i],inst) - get_cost(i,inst->best_sol[i],inst);
            
            if(extraM < min){
                min = extraM;
                imin = i;
            }

        }


        jmin = inst->best_sol[imin];
        inst->best_sol[imin] = z;
        inst->best_sol[z] = jmin;
        visited[z]=1;

        printf("best sol: %d %d %d\n", imin, z, jmin);
        
        
    }
    
    /*
    for(int i=0; i<inst->nnodes ; i++){
            
            printf("%f %f\n", inst->xcoord[inst->best_sol[x]], inst->ycoord[inst->best_sol[x]] );
            //fprintf(out_file, "%f %f\n", inst->xcoord[inst->best_sol[x]], inst->ycoord[inst->best_sol[x]] );
            printf("%f\n",inst->best_sol[x]);
            x = inst->best_sol[x];
        }

    */

int x=0;
    for(int i=0;i<inst->nnodes;i++){
        
            //printf("%d\n", inst->best_sol[i] );
            
            printf("%f %f\n", inst->xcoord[inst->best_sol[x]], inst->ycoord[inst->best_sol[x]] );
            fflush(NULL);
            x = inst->best_sol[x];
            
    }

    //plot(inst);


    return 0;
}