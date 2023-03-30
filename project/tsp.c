#include "utils.h"


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

 void plot(instance *inst, int *solution,char name[]){

    FILE *out = fopen(".\\output\\out.txt", "w");
    if (out == NULL) printf("input file not found!");


    int x=0;
    for(int i=0;i<inst->nnodes;i++){
        
            fprintf(out, "%f %f\n", inst->xcoord[solution[x]], inst->ycoord[solution[x]] );
            x = solution[x];
            
    }

   fprintf(out, "%f %f\n", inst->xcoord[solution[x]], inst->ycoord[solution[x]] );
   
    fclose(out);

    //da modificare o aggiungere una funzione per creare il file commands.txt
    //GNUPLOT to cmd
   FILE *commands = fopen(".\\plot\\commands.txt","w+");

   fprintf(commands,"set terminal png\n");
   fprintf(commands,"set output \"./plot/%s.png\"\n",name);
   fprintf(commands,"set style line 1 \\\n");
   fprintf(commands,"    linecolor rgb'#FF0000' \\\n");
   fprintf(commands,"    linetype 1 linewidth 2 \\\n");
   fprintf(commands,"    pointtype 7 pointsize 2\n");
   fprintf(commands,"plot \"./output/out.txt\" with linespoint linestyle 1\n");



   fclose(commands);
	system("gnuplot ./plot/commands.txt");
 }

int timeOut(instance *inst, double tl){
   return (second() - inst->timeStart) > tl;
}

int checkSol(instance *inst, int* sol){
   int *visited = (int*)calloc(inst->nnodes, sizeof(int));
   int error = 0;
   for(int i = 0 ; i < inst->nnodes;i++){
        visited[sol[i]]++;
    }

   for(int i = 0 ; i < inst->nnodes; i++){
        if(visited[i] != 1) {
         printf("ERROR IN THE SOLUTION: %d as %d\n\n",i, visited[i]);
         error = 1;
        }
   }
   free(visited);
   return error;

}

int checkCost(instance *inst, int* sol, double c){
   double cost = 0;
   for(int i = 0 ; i < inst->nnodes; i++){
        cost += get_cost(i,sol[i],inst);
   }

   //checks that the true cost of the solution and the computed cost are equal
   if(fabs(cost-c) > 0.000001) {
      printf("ERROR IN THE SOLUTION COST: %f as %f\n\n",cost, c);
      return 1;
   }

   return 0;

 } 

//updates best solution and zcost
void updateSol(instance *inst, double cost, int* solution)
{
    if(VERBOSE >= 10) printf("-------- Updating best solution --------\n");
    inst->zbest = cost;
    memcpy(inst->best_sol, solution, inst->nnodes * sizeof(int));
} 
 

//reverse path b -> a1
void reverse(instance *inst, int *solution, int a,int b)
{
    //printf("SWAP 2OPT: %d with %d\n",a,b);
    int a1 = solution[a];
    int b1 = solution[b];

    int* old = (int*)malloc(inst->nnodes * sizeof(int));
    memcpy(old,solution,sizeof(int)*inst->nnodes);

    
    solution[a]=b;
    solution[a1]=b1;
    
    int i=a1;
    
    while (i!=b)
        {
            solution[old[i]] = i;
            i = old[i];
        }

    
    free(old);

}