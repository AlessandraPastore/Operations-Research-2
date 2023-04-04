#include "utils.h"

#define POPULATION 20
#define OFFSPRING_RATE 0.5
#define MUTATION_RATE 0.08
#define MAX_GEN 20


// member of the population
typedef struct {
    int* solution;      // tsp feasible tour
    double fitness;     // score of the member
    double wheelProb;   // fitness / totalFitness
    double sumProb;     // cumulative probability
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

int compareChromosomes(const void *lhs, const void *rhs) {
    const chromosome* lp = lhs;
    const chromosome* rp = rhs;

    return rp->fitness - lp->fitness;
}

int selectParent(chromosome *population){
    
    // Select a random number between 0 and 1
    double random = (double)rand() / RAND_MAX;

    // Use the roulette wheel to select a chromosome
    for(int i=POPULATION-1; i>=0; i--){
        if(random <= population[i].sumProb) return i;
    }

    print_error("ROULETTE WHEEL FAILED");
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

void mutate(instance *inst, chromosome *member){

    printf("\nSTART MUTATION\n");
    
    int* old = (int*)malloc(inst->nnodes * sizeof(int));
    memcpy(old,member->solution,sizeof(int)*inst->nnodes);

    int a = rand() % inst->nnodes;
    int a1 = member->solution[a];

    int b;
    do{b = rand() % inst->nnodes;} while(b == a || b == a1 || member->solution[b] == a);
    int b1 = member->solution[b];

    //update fitness
    member->fitness = member->fitness 
                        - get_cost(a,a1,inst)
                        - get_cost(b,b1,inst)
                        + get_cost(a,b,inst)
                        + get_cost(a1,b1,inst);

    //swap edges
    member->solution[a] = b;
    member->solution[a1] = b1;

    reverse2(member->solution, old, a1, b);

    free(old);

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
        //printf("Probability for n.%d: %f\n",i,population[i].wheelProb);
    }



    //to do
    int gen = 1;
    chromosome *offspring = (chromosome*)calloc(POPULATION * OFFSPRING_RATE, sizeof(chromosome));

    //sort population based on fitness. First we have the one with better fitness, last the worse
    qsort(population, POPULATION, sizeof(chromosome), compareChromosomes);

    population[0].sumProb = 1;
    for(int i=1;i<POPULATION;i++){
        population[i].sumProb = population[i-1].sumProb - population[i-1].wheelProb;
    }

    int *visited = (int*)calloc(POPULATION,sizeof(int));
    int count = 0;

    printf("\nSTARTING PARENTING\n",totalFit);
    
    //produce the entire offpring
    while(count < POPULATION * OFFSPRING_RATE){
        
        int p1,p2; 

        do{ p1 = selectParent(population); }while(visited[p1]);
        visited[p1] = 1;

        printf("parent 1: %d\n",p1);

        do{ p2 = selectParent(population); }while(p1 == p2 || visited[p2]);
        visited[p2] = 1;

        printf("parent 2: %d\n",p2);

        produceOffspring(inst,population[p1].solution, population[p2].solution, &offspring[count]);

        printf("offspring solution: %f\n",offspring[count].fitness);
        for(int i=0; i<inst->nnodes; i++){
            printf("%d ",offspring[count].solution[i]);
        }

        printf("\n\n");

        if(VERBOSE >= 10) {
            if(checkSol(inst,offspring[count].solution)) return 1;
            if(checkCost(inst,offspring[count].solution,offspring[count].fitness)) return 1;
        }
        
        count++;
    }

    //mutation
    double m = (double)rand() / RAND_MAX;
    int mutant = -1;
    if(m < MUTATION_RATE){
        mutant = rand() % POPULATION;
        mutate(inst, &population[mutant]);

        if(VERBOSE >= 10) {
            if(checkSol(inst,population[mutant].solution)) return 1;
            if(checkCost(inst,population[mutant].solution,population[mutant].fitness)) return 1;
        }

    }

    //elitism
    

    //create the new generation


    
        

    free(visited);
    free(offspring);
    free(population);
    return 0;
}