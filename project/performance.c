#include "utils.h"
#define MAX_COORD 10000

void randomInstance(instance * inst){

    printf("...RANDOM\n");
    
    if(inst->seed != -1) 
        srand(inst->seed);

    for(int i=0; i<inst->nnodes; i++){
        inst->xcoord[i] = rand() % MAX_COORD;
        inst->ycoord[i] = rand() % MAX_COORD;
    }

    computeCost(inst);

}

void initInstance(instance * inst){

    printf("...INIT\n");

    inst->nnodes = 5000;

    inst->xcoord = (double*)calloc(inst->nnodes, sizeof(double));
    inst->ycoord = (double*)calloc(inst->nnodes, sizeof(double));

    inst->cost=(double*)calloc(inst->nnodes*inst->nnodes,sizeof(double));
    inst->flagCost = 0;

}

int call(instance *inst, char name[]){
    
    inst->timeStart = second();
    strcpy(inst->heuristic, name);
    if(heuristic(inst)) {
        print_error("ERROR IN HEUR"); 
        return 1;
    }

    return 0;
}

int performance(instance *inst){
    
    initInstance(inst);
    FILE *out = fopen(".\\output\\perf.txt", "w");
    if (out == NULL) printf("input file not found!");
    
    for(int i=0; i<10; i++){
        
        randomInstance(inst);

        if(call(inst,"GREEDY")) return 1;

        printf("%d,%f\n",i,inst->zbest);
        fprintf(out,"%d,%f",i,inst->zbest);

        inst->best_sol = (int *) calloc(inst->nnodes, sizeof(int));
        inst->zbest = -1;

        if(call(inst,"GREEDYGRASP")) return 1;

        printf(",%f\n",inst->zbest);
        fprintf(out,",%f",inst->zbest);


        inst->best_sol = (int *) calloc(inst->nnodes, sizeof(int));
        inst->zbest = -1;

        if(call(inst,"EXTRAMILEAGE")) return 1;

        printf(",%f\n",inst->zbest);
        fprintf(out,",%f\n",inst->zbest);

    }

    free_instance(inst);
    fclose(out);
    return 0;
}