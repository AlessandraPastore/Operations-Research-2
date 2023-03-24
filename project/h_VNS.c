#include "utils.h"

//void reverse(instance *inst, int* old, int a1,int b);
//int opt_2(instance *inst, double tl);


int VNS(instance *inst){

    
    //chiama greedy, chiama opt2 che trova bestSol

    //forse non serve perchè viene chiamato già in grasp
    if(inst->seed != -1) 
        srand(inst->seed);

    //tempo tra il plot di opt2 e l inizio di VNS
    double lostTime = second() - inst->timeEnd;
    int a,b,c,d,e;
    int a1,b1,c1,d1,e1;

    //vector with the new edge combination
    int* newSol = (int*)calloc(inst->nnodes, sizeof(int));
    
    //old solution copy
    int* oldSol = (int*)malloc(inst->nnodes * sizeof(int));
    memcpy(oldSol,inst->best_sol,sizeof(int)*inst->nnodes);


    do{
        
        //select 5 random edges
        int a = rand() % inst->nnodes;

        do{ b = rand() % inst->nnodes; }while(a == b);
        do{ c = rand() % inst->nnodes; }while(a == c || b == c);
        do{ d = rand() % inst->nnodes; }while(a == d || b == d || c == d);
        do{ e = rand() % inst->nnodes; }while(a == e || b == e || c == e || d == e);

        printf("5 edges selected");


        a1 = inst->best_sol[a];
        b1 = inst->best_sol[b];
        c1 = inst->best_sol[c];
        d1 = inst->best_sol[d];
        e1 = inst->best_sol[e];



        //create new path
        int i = a1;
        int j = a;
        int done = 0;

        while(done != 5){
            if(oldSol[i] == a1){ inst->best_sol[j] = i = a1; j = a; done++; continue; }    
            if(oldSol[i] == b1){ inst->best_sol[j] = i = b1; j = b; done++; continue; }
            if(oldSol[i] == c1){ inst->best_sol[j] = i = c1; j = c; done++; continue; }
            if(oldSol[i] == d1){ inst->best_sol[j] = i = d1; j = d; done++; continue; }
            if(oldSol[i] == e1){ inst->best_sol[j] = i = e1; j = e; done++; continue; }
            i = oldSol[i];
        }
        
        //new crossed edges
        //plot(inst);

        printf("entering opt2");
        //opt_2(inst, inst->timelimit); //to change tl

        free(oldSol);
        free(newSol);

        
        
         
        



    } while (!timeOut(inst, inst->timelimit + lostTime));
    
    
    
    return 0;
}