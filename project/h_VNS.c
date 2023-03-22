#include "utils.h"


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
        //swap them
        //repeat

        int a = rand() % inst->nnodes;

        do{ b = rand() % inst->nnodes; }while(a == b);
        do{ c = rand() % inst->nnodes; }while(a == c || b == c);
        do{ d = rand() % inst->nnodes; }while(a == d || b == d || c == d);
        do{ e = rand() % inst->nnodes; }while(a == e || b == e || c == e || d == e);


        a1 = inst->best_sol[a];
        b1 = inst->best_sol[b];
        c1 = inst->best_sol[c];
        d1 = inst->best_sol[d];
        e1 = inst->best_sol[e];

        //swap edges
        inst->best_sol[a] = b1;
        inst->best_sol[c] = e;
        
         
        



    } while (!timeOut(inst, inst->timelimit + lostTime));
    
    
    
    return 0;
}