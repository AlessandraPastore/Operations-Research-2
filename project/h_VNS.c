#include "utils.h"

//void reverse(instance *inst, int* old, int a1,int b);
int opt_2(instance *inst, double tl);


int VNS(instance *inst){

    
    //chiama greedy, chiama opt2 che trova bestSol

    //forse non serve perchè viene chiamato già in grasp
    if(inst->seed != -1) 
        srand(inst->seed);

    //tempo tra il plot di opt2 e l inizio di VNS
    double lostTime = second() - inst->timeEnd;
    int a,b,c,d,e;
    int a1,b1,c1,d1,e1;


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

        //old solution copy
        int* old = (int*)malloc(inst->nnodes * sizeof(int));
        memcpy(old,inst->best_sol,sizeof(int)*inst->nnodes);


        //create new path
        int i = a1;
        int j = a;
        int done = 0;

        while(done != 5){
            if(old[i] == a1){ inst->best_sol[j] = a1; j = a; i = a1; done++; continue; }
            if(old[i] == b1){ inst->best_sol[j] = b1; j = b; i = b1; done++; continue; }
            if(old[i] == c1){ inst->best_sol[j] = c1; j = c; i = c1; done++; continue; }
            if(old[i] == d1){ inst->best_sol[j] = d1; j = d; i = d1; done++; continue; }
            if(old[i] == e1){ inst->best_sol[j] = e1; j = e; i = e1; done++; continue; }
            i = old[i];
        }
        
        //new crossed edges
        plot(inst);

        free(old);

        printf("entering opt2");
        opt_2(inst, inst->timelimit); //to change tl


        
        
         
        



    } while (!timeOut(inst, inst->timelimit + lostTime));
    
    
    
    return 0;
}