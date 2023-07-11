#include "../utils/utils.h"

#define POPULATION 100
#define OFFSPRING_RATE 0.2 //must be 0.5 or less
#define MUTATION_RATE 0.04
#define MAX_GEN 1000 


// member of the population
typedef struct {
	int* solution;      // tsp feasible tour
	double fitness;     // score of the member
	double wheelProb;   // fitness / totalFitness
	double sumProb;     // cumulative probability
} chromosome;

void computeFitness(instance* inst, int* solution, int* visited, double* cost) {

	int index = 0;
	int next = -1;
	do {
		next = solution[index];
		*cost += get_cost(index, next, inst);
		index = next;

	} while (index != 0);
}

//needed in qsort
int compareChromosomes(const void* lhs, const void* rhs) {
	const chromosome* lp = lhs;
	const chromosome* rp = rhs;

	return lp->fitness - rp->fitness;
}

//uses the wheel roulette method to select a parent
int selectParent(chromosome* population) {

	// Select a random number between 0 and 1
	double random = (double)rand() / RAND_MAX;

	// Use the roulette wheel to select a chromosome
	for (int i = POPULATION - 1; i >= 0; i--) {
		if (random <= population[i].sumProb) return i;
	}

	print_error("ROULETTE WHEEL FAILED");
	exit(1);
}

//produce the crossover given two parents
void produceOffspring(instance* inst, int* p1, int* p2, chromosome* offspring) {

	int breakPoint = rand() % (inst->nnodes - 2) + 1;

	int* visited = (int*)calloc(inst->nnodes, sizeof(int));

	int index = 0;
	visited[0] = 1;
	int temp = -1;

	//copy first half of p1 into the child
	for (int i = 0; i < breakPoint; i++) {
		temp = p1[index];
		offspring->solution[index] = temp;
		visited[temp] = 1;
		index = temp;
	}

	//copy second half of p1 into the child
	for (int i = breakPoint; i < inst->nnodes; i++) {
		temp = p2[temp];
		if (!visited[temp]) {
			offspring->solution[index] = temp;
			visited[temp] = 1;
			index = temp;
		}
	}

	offspring->solution[index] = 0;

	offspring->fitness = 0;

	//updates fitness of the newborn
	computeFitness(inst, offspring->solution, visited, &offspring->fitness);

	//repair solution using extra mileage
	computeExtra(inst, visited, offspring->solution, &offspring->fitness);

	free(visited);

}

//apply a mutation, aka swaps two edges at random
int mutate(instance* inst, chromosome* member) {

	if (VERBOSE >= 10) printf("\nSTART MUTATION\n");

	int* old = (int*)malloc(inst->nnodes * sizeof(int));
	memcpy(old, member->solution, sizeof(int) * inst->nnodes);

	int a = rand() % inst->nnodes;
	int a1 = member->solution[a];

	int b;
	do { b = rand() % inst->nnodes; } while (b == a || b == a1 || member->solution[b] == a);
	int b1 = member->solution[b];

	//update fitness
	member->fitness = member->fitness
		- get_cost(a, a1, inst)
		- get_cost(b, b1, inst)
		+ get_cost(a, b, inst)
		+ get_cost(a1, b1, inst);

	//swap edges
	member->solution[a] = b;
	member->solution[a1] = b1;

	//reverse path
	reverse2(member->solution, old, a1, b);

	if (checkCost(inst, member->solution, member->fitness)) return 1;

	free(old);
	return 0;

}


//initializes the starting population
int initMember(instance* inst, chromosome* member) {

	member->solution = (int*)calloc(inst->nnodes, sizeof(int));

	//inst->seed = rand();
	grasp(inst, 0, 0);

	//initialize each member with a random sol. from grasp
	memcpy(member->solution, inst->best_sol, sizeof(int) * inst->nnodes);
	member->fitness = inst->zbest;

	inst->zbest = -1;

	if (VERBOSE >= 10) {
		if (checkSol(inst, member->solution)) return 1;
		if (checkCost(inst, member->solution, member->fitness)) return 1;
	}

	return 0;
}

void freePopulation(chromosome* population) {
	for (int i = 0; i < POPULATION; i++) {
		free(population[i].solution);
	}
}

void freeOffspring(chromosome* offspring) {
	for (int i = 0; i < POPULATION * OFFSPRING_RATE; i++) {
		free(offspring[i].solution);
	}
}

//resets offspring and visited for the new generation
void reset(instance* inst, chromosome* offspring, int* visited) {

	for (int i = 0; i < POPULATION * OFFSPRING_RATE; i++) {
		memset(offspring[i].solution, 0, sizeof(int) * inst->nnodes);
		offspring[i].fitness = 0;
	}

	memset(visited, 0, sizeof(int) * POPULATION);
}

//main function
int genetic(instance* inst) {

	chromosome* population = (chromosome*)calloc(POPULATION, sizeof(chromosome));
	double totalFit = 0;

	// initialize population
	for (int i = 0; i < POPULATION; i++) {
		if (initMember(inst, &population[i])) return 1;
		totalFit += population[i].fitness;
	}

	if (VERBOSE >= 1) printf("\n-- POPULATION INITIALIZED SUCCESS --\n");
	inst->timeStart = second();

	int gen = 1;

	//initializes offspring
	chromosome* offspring = (chromosome*)calloc(POPULATION * OFFSPRING_RATE, sizeof(chromosome));
	for (int i = 0; i < POPULATION * OFFSPRING_RATE; i++) {
		offspring[i].solution = (int*)calloc(inst->nnodes, sizeof(int));
	}

	//initializes the 0-1 array to select the parents
	int* visited = (int*)calloc(POPULATION, sizeof(int));

	//initializes the mutant
	chromosome mutant;
	mutant.solution = (int*)calloc(inst->nnodes, sizeof(int));

	
	

	do {

		//compute the wheel roulette probabilities
		for (int i = 0; i < POPULATION; i++) {
			population[i].wheelProb = population[i].fitness / totalFit;
		}

		//sort population based on fitness. First we have the one with better fitness, last the worse
		qsort(population, POPULATION, sizeof(chromosome), compareChromosomes);

		

		//check to update the best solution
		if (inst->zbest == -1 || inst->zbest > population[0].fitness) {
			updateSol(inst, population[0].fitness, population[0].solution);
		}

		

		//compute the cumulative probabilities to use in the wheel roulette
		population[0].sumProb = 1;
		for (int i = 1; i < POPULATION; i++) {
			population[i].sumProb = population[i - 1].sumProb - population[i - 1].wheelProb;
		}


		int count = 0;

		//produce the entire offpring
		while (count < POPULATION * OFFSPRING_RATE) {

			

			int p1, p2; //the parents

			do { p1 = selectParent(population); } while (visited[p1]);
			visited[p1] = 1;

			do { p2 = selectParent(population); } while (p1 == p2 || visited[p2]);
			visited[p2] = 1;

			//crossover
			produceOffspring(inst, population[p1].solution, population[p2].solution, &offspring[count]);

			if (VERBOSE >= 10) {
				if (checkSol(inst, offspring[count].solution)) return 1;
				if (checkCost(inst, offspring[count].solution, offspring[count].fitness)) return 1;
			}

			count++;
		}

		//try to apply a mutation
		double m = (double)rand() / RAND_MAX;
		int mutantIndex = -1;

		if (m < MUTATION_RATE) {
			mutantIndex = rand() % (POPULATION - 1) + 1;   //assures that the first element remains elite

			memcpy(mutant.solution, population[mutantIndex].solution, sizeof(int) * inst->nnodes);
			mutant.fitness = population[mutantIndex].fitness;


			mutate(inst, &mutant);

			if (VERBOSE >= 10) {
				if (checkSol(inst, mutant.solution)) return 1;
				if (checkCost(inst, mutant.solution, mutant.fitness)) return 1;
			}

		}

		//create the new generation keeping the elitism
		int offset = ((POPULATION - POPULATION * OFFSPRING_RATE));

		//copy the offspring at the end of the population, killing all worse chromosomes
		for (int i = offset; i < POPULATION; i++) {
			memcpy(population[i].solution, offspring[i - offset].solution, sizeof(int) * inst->nnodes);
			population[i].fitness = offspring[i - offset].fitness;
		}


		//add mutant to the survivors (if there is). It adds it right before the newly added offspring
		if (mutantIndex != -1) {
			memcpy(population[(int)(POPULATION * OFFSPRING_RATE) - 1].solution, mutant.solution, sizeof(int) * inst->nnodes);
			population[(int)(POPULATION * OFFSPRING_RATE) - 1].fitness = mutant.fitness;
		}

		for (int i = 0; i < POPULATION; i++) {
			if (checkCost(inst, population[i].solution, population[i].fitness))return 1;
		}

		gen++;

		totalFit = 0;

		//if there is a new generation to start, resets the useful arrays and updates the total fitness
		if (gen <= MAX_GEN) {
			reset(inst, offspring, visited);
			for (int i = 0; i < POPULATION; i++) {
				totalFit += population[i].fitness;
			}
		}

	} while (gen <= MAX_GEN && !timeOut(inst, inst->timelimit));

	printf("timelimit: %f\n", inst->timelimit);
	printf("gen:%d", gen);

	//checks last generation for the best solution
	//sort population based on fitness. First we have the one with better fitness, last the worse
	qsort(population, POPULATION, sizeof(chromosome), compareChromosomes);

	if (inst->zbest == -1 || inst->zbest > population[0].fitness) {
		updateSol(inst, population[0].fitness, population[0].solution);
	}

	plot(inst, inst->best_sol, "GENETIC");
	if (VERBOSE >= 1)printf("BEST GENETIC COST FOUND: %f", inst->zbest);
	if (VERBOSE >= 10)printf("generation: %d", gen);






	free(visited);

	free(mutant.solution);

	freeOffspring(offspring);
	free(offspring);

	freePopulation(population);
	free(population);
	return 0;
}