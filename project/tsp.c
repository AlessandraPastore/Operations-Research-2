#include "tsp.h"
void print_error(const char* err);
double second();

double dist(instance *inst,int index1,int index2) {

    return sqrt((inst->xcoord[index1]-inst->xcoord[index2])*(inst->xcoord[index1]-inst->xcoord[index2])+(inst->ycoord[index1]-inst->ycoord[index2])*(inst->ycoord[index1]-inst->ycoord[index2]));
 }

void computeCost(instance *inst)
 {
    
    for(int i=0;i<inst->nnodes;i++)
        for(int j=0;j<i;j++) 
             inst->cost[i*inst->nnodes+j] = inst->cost[j*inst->nnodes+i] = dist(inst,i,j);

    inst->flagCost = 1;
    inst->zbest = -1;
    inst->best_sol = (int *) calloc(inst->nnodes, sizeof(int));
 }

 double get_cost(int i, int j, instance *inst){
    if( i < 0 || j < 0 || i >= inst->nnodes || j >= inst->nnodes ) {
        printf("get_cost(): unexpected params i: %d, j= %dd",i,j);
    }
    return inst->cost[i * inst->nnodes + j];
 }

 void plot(instance *inst){

    FILE *out = fopen(".\\output\\out.txt", "w");
    if (out == NULL) printf("input file not found!");


    int x=0;
    for(int i=0;i<inst->nnodes;i++){
        
            fprintf(out, "%f %f\n", inst->xcoord[inst->best_sol[x]], inst->ycoord[inst->best_sol[x]] );
            x = inst->best_sol[x];
            
    }

   fprintf(out, "%f %f\n", inst->xcoord[inst->best_sol[x]], inst->ycoord[inst->best_sol[x]] );
   
    fclose(out);

    //da modificare o aggiungere una funzione per creare il file commands.txt
    //GNUPLOT to cmd
	system("gnuplot ./plot/commands.txt");
 }

int timeOut(instance *inst){
   double t = second();
   return (t - inst->timeStart) > inst->timelimit;
}

int checkSol(instance *inst, int* sol){
   int *visited = (int*)calloc(inst->nnodes, sizeof(int));
   for(int i = 0 ; i < inst->nnodes;i++){
        visited[sol[i]]++;
    }

   for(int i = 0 ; i < inst->nnodes; i++){
        if(visited[i] != 1) printf("ERROR IN THE SOLUTION: %d as %d\n\n",i, visited[i]);
   }
   free(visited);
   return 0;

}
 