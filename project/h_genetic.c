#include "utils.h"

#define POPULATION 20
#define OFFSPRING_RATE 0.5
#define MAX_GEN 20


// member of the population
typedef struct {
    int* solution;  // tsp feasible tour
    double fitness; // score of the member
    double wheelProb;
} chromosome;

void computeFitness(instance *inst, int *solution, int *visited, double *cost){
    
    int index = 0;
    int next = -1;
    do{
        next = solution[index];
        *cost += get_cost(index,next,inst);
        index = next;
        
    }while(index != 0);
}

int selectParent(chromosome *population){
    
    double sum = 0;
    // Select a random number between 0 and 1
    double random = (double)rand() / RAND_MAX;

    // Use the roulette wheel to select a chromosome
    for(int i=0; i<POPULATION; i++){
        sum += population[i].wheelProb;

        if(random <= sum) return i;
    }

    exit(1);
}

void produceOffspring(instance *inst, int *p1, int *p2, chromosome *offspring){

    printf("\nSTART OFFSPRING\n");
    int breakPoint = rand() % inst->nnodes;
    printf("break: %d\n",breakPoint);

    int *visited = (int*)calloc(inst->nnodes,sizeof(int));
    offspring->solution = (int*)calloc(inst->nnodes,sizeof(int));

    
    int index = 0;
    visited[0] = 1;
    int temp = -1;
    
    //copy first half of p1 into the child
    for(int i=0; i<breakPoint; i++){
        temp = p1[index];
        offspring->solution[index] = temp;
        visited[temp] = 1;
        index = temp;
    }

    //copy second half of p1 into the child
    for(int i=breakPoint; i<inst->nnodes; i++){
        temp = p2[temp];
        if(!visited[temp]) {
            offspring->solution[index] = temp;
            visited[temp] = 1;
            index = temp;
        }
    }

    offspring->solution[index] = 0;

    offspring->fitness = 0;

    computeFitness(inst, offspring->solution, visited, &offspring->fitness);

    //repair solution using extra mileage
    computeExtra(inst, visited, offspring->solution, &offspring->fitness);

    free(visited);

}



void initMember(instance *inst, chromosome *member){

        member->solution = (int*)calloc(inst->nnodes,sizeof(int));

        //inst->seed = rand();
        grasp(inst,0,0);

        //initialize each member with a random sol. from grasp
        memcpy(member->solution,inst->best_sol,sizeof(int)*inst->nnodes);
        member->fitness = inst->zbest;
        
        inst->zbest = -1;
}

int genetic(instance *inst){

    chromosome *population = (chromosome*)calloc(POPULATION, sizeof(chromosome));
    double totalFit = 0;

    // initialize population
    for(int i=0; i<POPULATION; i++){
        initMember(inst,&population[i]);
        totalFit += population[i].fitness;

        for(int j=0;j<inst->nnodes;j++){
            printf("%d ",population[i].solution[j]);
        }
    }

    printf("\n-- POPULATION INITIALIZED SUCCESS --\n");
    printf("Total fitness: %f\n\n",totalFit);

    for(int i=0; i<POPULATION; i++){
        population[i].wheelProb = population[i].fitness / totalFit;
        printf("Probability for n.%d: %f\n",i,population[i].wheelProb);
    }

    //to do
    int gen = 1;
    

    int *visited = (int*)calloc(POPULATION,sizeof(int));
    int count = 0;

    printf("\nSTARTING PARENTING\n",totalFit);
    
    while(count < POPULATION * OFFSPRING_RATE){
        
        int p1,p2; 

        do{ p1 = selectParent(population); }while(visited[p1]);
        visited[p1] = 1;

        printf("parent 1: %d\n",p1);

        do{ p2 = selectParent(population); }while(p1 == p2 || visited[p2]);
        visited[p2] = 1;

        printf("parent 2: %d\n",p2);

        chromosome offspring;

        produceOffspring(inst,population[p1].solution, population[p2].solution, &offspring);

        printf("offspring solution: %f\n",offspring.fitness);
        for(int i=0; i<inst->nnodes; i++){
            printf("%d ",offspring.solution[i]);
        }

        printf("\n\n");


        if(VERBOSE >= 10) {
            if(checkSol(inst,offspring.solution)) return 1;
            if(checkCost(inst,offspring.solution,offspring.fitness)) return 1;
        }
        

        count++;
        
    }
    
        

    free(visited);
    free(population);
    return 0;
}