#include "utils.h"

//void reverse(instance *inst, int* old, int a1,int b);
//int opt_2(instance *inst, double tl);



int VNS(instance *inst){

    if(VERBOSE >= 10) printf("--- Starting VNS ---\n");

    
    //chiama greedy, chiama opt2 che trova bestSol

    //forse non serve perchè viene chiamato già in grasp
    if(inst->seed != -1) 
        srand(inst->seed);

    //tempo tra il plot di opt2 e l inizio di VNS
    double lostTime = second() - inst->timeEnd;
    int a,b,c,d,e;
    int a1,b1,c1,d1,e1;

    //vector with the new edge combination
    int* solution = (int*)calloc(inst->nnodes, sizeof(int));
    double cost = inst->zbest;
    
    //old solution copy
    int* oldSol = (int*)malloc(inst->nnodes * sizeof(int));

    //solution copies the actual local minimum
    memcpy(solution,inst->best_sol,sizeof(int)*inst->nnodes);
    


    do{

        //copy solution in oldSol
        memcpy(oldSol,solution,sizeof(int)*inst->nnodes);
        
        //select 5 random edges
        int a = rand() % inst->nnodes;

        do{ b = rand() % inst->nnodes; }while(a == b);
        do{ c = rand() % inst->nnodes; }while(a == c || b == c);
        do{ d = rand() % inst->nnodes; }while(a == d || b == d || c == d);
        do{ e = rand() % inst->nnodes; }while(a == e || b == e || c == e || d == e);


        //save a-a1 etc current edges
        a1 = solution[a];
        b1 = solution[b];
        c1 = solution[c];
        d1 = solution[d];
        e1 = solution[e];

        cost = cost - get_cost(a,a1,inst) 
                    - get_cost(b,b1,inst) 
                    - get_cost(c,c1,inst) 
                    - get_cost(d,d1,inst) 
                    - get_cost(e,e1,inst);


        //create new crossed paths
        int i = a1;
        int j = a;
        int done = 0;

        while(done != 5){

            if(oldSol[i] == a1) {    

                printf("SWAP: %d,%d with %d\n",j,i,a1);
                solution[j] = i = a1; 
                cost = cost + get_cost(j,i,inst); 
                j = a; 
                done++; 
                continue; 
            }    

            if(oldSol[i] == b1) { 
                printf("SWAP: %d,%d with %d\n",j,i,b1);
                solution[j] = i = b1; 
                cost = cost + get_cost(j,i,inst);
                j = b; 
                done++; 
                continue; 
            }

            if(oldSol[i] == c1) {   
                printf("SWAP: %d,%d with %d\n",j,i,c1); 
                solution[j] = i = c1; 
                cost = cost + get_cost(j,i,inst); 
                j = c; 
                done++; 
                continue; 
            }  

            if(oldSol[i] == d1) {   
                printf("SWAP: %d,%d with %d\n",j,i,d1); 
                solution[j] = i = d1; 
                cost = cost + get_cost(j,i,inst); 
                j = d; 
                done++; 
                continue; 
            }  

            if(oldSol[i] == e1) {    
                printf("SWAP: %d,%d with %d\n",j,i,e1);
                solution[j] = i = e1; 
                cost = cost + get_cost(j,i,inst); 
                j = e; 
                done++; 
                continue; 
            }  

            i = oldSol[i];
        }

        if(VERBOSE >= 10) {
            if(checkSol(inst,solution)) return 1;
            if(checkCost(inst,solution,cost)) return 1;
        }
        
        //new crossed edges
        printf("new edges\n");
        plot(inst, solution);

        //printf("- - entering opt2 - -\n");
        opt_2(inst, inst->timelimit, solution, &cost); //to change tl

        



    } while (!timeOut(inst, inst->timelimit + lostTime));
    
    free(oldSol);
    free(solution);
    
    
    return 0;
}
