#include "thread.h"
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <valgrind/valgrind.h>

#define STACK_SIZE 64*1024

#if VERBOSE
#define DEBUG(...) { \
    printf(__VA_ARGS__); \
    thread_t np; \
    printf("queue : "); \
    STAILQ_FOREACH(np, &runqueue, next_thread) \
    printf("%d ", np->id); \
    printf("\n"); \
    }
#else 
#define DEBUG(...) 
#endif

int thread_id = 0;

STAILQ_HEAD(, thread_t) runqueue;

thread_t thread_main;
thread_t current_thread;
ucontext_t context_main;

void change_to_next_thread(){
    if (!STAILQ_EMPTY(&runqueue)){
        thread_t previous_thread = current_thread;
        current_thread = STAILQ_FIRST(&runqueue);
        swapcontext(previous_thread->context, current_thread->context);        
    }
}

/* recuperer l'identifiant du thread courant.
 */
thread_t thread_self(void){
    return current_thread;
}

void * wrapper(void* (*func)(void*), void * funcarg){
    void * retval = func(funcarg);
    if (current_thread->etat == VIVANT){ // Pas eu d'appel à thread_exit dans func 
        thread_exit(retval);
    }
    return retval;
}

/* creer un nouveau thread qui va exécuter la fonction func avec l'argument funcarg.
 * renvoie 0 en cas de succès, -1 en cas d'erreur.
 */
int thread_create(thread_t *newthread, void *(*func)(void *), void *funcarg){    
    thread_t thread = malloc(sizeof(struct thread_t));
    thread->id = thread_id++;
    thread->etat = VIVANT;
    thread->retval = NULL;
    thread->waiting_thread = NULL;

    ucontext_t * uc = malloc(sizeof(ucontext_t));
    getcontext(uc);
    uc->uc_stack.ss_size = STACK_SIZE;
    uc->uc_stack.ss_sp = malloc(uc->uc_stack.ss_size);
    thread->valgrind_stackid = VALGRIND_STACK_REGISTER(uc->uc_stack.ss_sp,uc->uc_stack.ss_sp + uc->uc_stack.ss_size);
    uc->uc_link = NULL;
    makecontext(uc, (void (*)(void)) wrapper, 2, func, funcarg);
    thread->context = uc;
    *newthread = thread;
    
    STAILQ_INSERT_TAIL(&runqueue, *newthread, next_thread); //Ajout du thread a la runqueue
    DEBUG("Thread %d created\n", thread->id);
    
    return 0;
}


/* passer la main à un autre thread.
 */
int thread_yield(void){
    if (STAILQ_EMPTY(&runqueue)){
        return 0;
    }   
    DEBUG("Before thread %d yield\n", current_thread->id);
    STAILQ_REMOVE_HEAD(&runqueue, next_thread);
    STAILQ_INSERT_TAIL(&runqueue, current_thread, next_thread);
    DEBUG("After thread %d yield\n", current_thread->id);
    change_to_next_thread();
    return 0;
}


/* attendre la fin d'exécution d'un thread.
 * la valeur renvoyée par le thread est placée dans *retval.
 * si retval est NULL, la valeur de retour est ignorée.
 */
int thread_join(thread_t thread, void **retval){    
    thread_t t = current_thread->waiting_thread;
    while(t != NULL){                
        if (t == thread){
            return -1;
        }
        t = t->waiting_thread;
    }
    
    if (thread->etat == VIVANT){
        thread->waiting_thread = thread_self();
        DEBUG("Before Thread %d join thread %d\n", current_thread->id, thread->id);
        STAILQ_REMOVE_HEAD(&runqueue, next_thread);
        DEBUG("After Thread %d join thread %d\n", current_thread->id, thread->id);
        change_to_next_thread();        
    }
    if (retval != NULL){
        *retval = thread->retval;
    }
    free_thread(thread);
    return 0;
}



/* terminer le thread courant en renvoyant la valeur de retour retval.
 * cette fonction ne retourne jamais.
 *
 * L'attribut noreturn aide le compilateur à optimiser le code de
 * l'application (élimination de code mort). Attention à ne pas mettre
 * cet attribut dans votre interface tant que votre thread_exit()
 * n'est pas correctement implémenté (il ne doit jamais retourner).
 */
void thread_exit(void *retval){
    current_thread->retval = retval;
    current_thread->etat = MORT;
    DEBUG("Before thread %d exit\n", current_thread->id);
    STAILQ_REMOVE_HEAD(&runqueue, next_thread);
    DEBUG("After thread %d exit\n", current_thread->id);
    if (current_thread->waiting_thread != NULL){
        //STAILQ_INSERT_TAIL(&runqueue, (thread_t) current_thread->waiting_thread, next_thread);
        STAILQ_INSERT_HEAD(&runqueue, (thread_t) current_thread->waiting_thread, next_thread);
        DEBUG("Inserting waiting thread %d in queue\n", current_thread->waiting_thread->id);
    }
    
    if (STAILQ_EMPTY(&runqueue)){ // thread qui finit le programme    
        if (current_thread == thread_main){ //if main called thread_exit
            free_thread(current_thread);
        } else{
            setcontext(&context_main);
        }
    }
    
    change_to_next_thread();
    exit(0);
}

void free_thread(thread_t t){
    if (t != thread_main){
        VALGRIND_STACK_DEREGISTER(t->valgrind_stackid);
        free(t->context->uc_stack.ss_sp);
    }
    free(t->context);
    free(t);
    t = NULL;
}

/* Interface possible pour les mutex */
//typedef struct thread_mutex { int dummy; } thread_mutex_t;
int thread_mutex_init(thread_mutex_t *mutex);
int thread_mutex_destroy(thread_mutex_t *mutex);
int thread_mutex_lock(thread_mutex_t *mutex);
int thread_mutex_unlock(thread_mutex_t *mutex);

__attribute__((constructor))
void init_var() {
    STAILQ_INIT(&runqueue); 
    thread_main = malloc(sizeof(struct thread_t));
    thread_main->id = thread_id++;
    thread_main->etat = VIVANT;
    thread_main->retval = NULL;
    thread_main->waiting_thread = NULL;
    ucontext_t * uc = malloc(sizeof(ucontext_t));
    getcontext(uc);
    /*
    uc->uc_stack.ss_size = STACK_SIZE;
    uc->uc_stack.ss_sp = malloc(uc->uc_stack.ss_size);
    thread_main->valgrind_stackid = VALGRIND_STACK_REGISTER(uc->uc_stack.ss_sp,uc->uc_stack.ss_sp + uc->uc_stack.ss_size);
    */
    uc->uc_link = NULL;
    thread_main->context = uc;
    current_thread = thread_main;

    STAILQ_INSERT_TAIL(&runqueue, current_thread, next_thread); //Ajout du thread a la runqueue

    //quand on join thread_main, permet de revenir sur la pile du main pour désallouer le dernier thread
    getcontext(&context_main);
    if (current_thread != thread_main){
        free_thread(current_thread);
        exit(0);
    }
}

__attribute__((destructor))
void destroy_var(){
    if (!STAILQ_EMPTY(&runqueue)){
        DEBUG("Destroy all var\n");
        thread_t t1, t2;
        t1 = STAILQ_FIRST(&runqueue);
        while (t1 !=NULL){
            t2 = STAILQ_NEXT(t1, next_thread);
            free_thread(t1);
            t1 = t2;
        } 
    }
}


int thread_mutex_init(thread_mutex_t *mutex){    
    if (mutex == NULL){
        return -1;
    }
    mutex->etat = UNLOCKED;
    STAILQ_INIT(&mutex->wqueue);
    return 0;
}

int thread_mutex_destroy(thread_mutex_t *mutex){
    if (mutex == NULL){
        return -1;
    }
    mutex->etat = DESTROYED;
    return 0;
    }
 

int thread_mutex_lock(thread_mutex_t *mutex){    
    if (mutex == NULL || mutex->etat == DESTROYED){
        return -1;
    }
    //printf("%d\n", mutex->etat);
    if (mutex->etat == UNLOCKED){
        mutex->etat = LOCKED;
    }else{      
        while (mutex->etat == LOCKED){
        DEBUG("Before thread %d try to take the mutex\n", current_thread->id)
        STAILQ_REMOVE_HEAD(&runqueue, next_thread);
        DEBUG("After thread %d try to take the mutex\n", current_thread->id)
        STAILQ_INSERT_TAIL(&mutex->wqueue, current_thread, next);
        change_to_next_thread();
        }
    } 
    return 0;
}


int thread_mutex_unlock(thread_mutex_t *mutex){
    if (mutex == NULL || mutex->etat == DESTROYED){
        return -1;
    }

    mutex->etat = UNLOCKED;
    if (!STAILQ_EMPTY(&mutex->wqueue)){
        thread_t thread = STAILQ_FIRST(&mutex->wqueue);
        STAILQ_REMOVE_HEAD(&mutex->wqueue, next);
        DEBUG("Before thread %d let the mutex\n", current_thread->id)
        STAILQ_INSERT_TAIL(&runqueue, thread, next_thread);
        DEBUG("After thread %d let the mutex\n", current_thread->id)
    }
    return 0;
}
