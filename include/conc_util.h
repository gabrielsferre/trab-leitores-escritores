#ifndef CONC_UTIL
#define CONC_UTIL

#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>

void* Malloc(size_t size) {
    void* p = malloc(size);
    if(!p) {
        printf("Falha ao alocar memoria\n");
        exit(0);
    }
    return p;
}

void* Calloc(size_t nmemb, size_t size) {
    void* p = calloc(nmemb, size);
    if(!p) {
        printf("Falha ao alocar memoria\n");
        exit(0);
    }
    return p;
}

int Pthread_create(pthread_t* thread, pthread_attr_t* attr, void*(*start_routine)(void*), void* arg) {
    int result = pthread_create(thread, attr, start_routine, arg);
    if(result) {
        printf("Falha ao criar thread\n");
        exit(0);
    }
    return result;
}

int Pthread_join(pthread_t thread, void** retval) {
    int result = pthread_join(thread, retval);
    if(result) {
        printf("Falha ao juntar thread\n");
        exit(0);
    }
    return result;
}

int Pthread_mutex_init(pthread_mutex_t *restrict mutex, const pthread_mutexattr_t *restrict attr) {
    int result = pthread_mutex_init(mutex, attr);
    if(result) {
        printf("Falha ao inicializar mutex\n");
        exit(0);
    }
    return result;
}

int Pthread_mutex_destroy(pthread_mutex_t *mutex) {
    int result = pthread_mutex_destroy(mutex);
    if(result) {
        printf("Falha ao destruir mutex\n");
        exit(0);
    }
    return result;
}

int Pthread_mutex_lock(pthread_mutex_t *mutex) {
    int result = pthread_mutex_lock(mutex);
    if(result) {
        printf("Falha ao usar pthread_mutex_lock\n");
        exit(0);
    }
    return result;
}

int Pthread_mutex_unlock(pthread_mutex_t *mutex) {
    int result = pthread_mutex_unlock(mutex);
    if(result) {
        printf("Falha ao usar pthread_mutex_unlock\n");
        exit(0);
    }
    return result;
}

int Pthread_cond_init(pthread_cond_t *restrict cond, const pthread_condattr_t *restrict attr) {
    int result = pthread_cond_init(cond, attr);
    if(result) {
        printf("Falha ao inicializar condition\n");
        exit(0);
    }
    return result;
}

int Pthread_cond_destroy(pthread_cond_t *cond) {
    int result = pthread_cond_destroy(cond);
    if(result) {
        printf("Falha ao destruir condition\n");
        exit(0);
    }
    return result;
}

int Pthread_cond_wait(pthread_cond_t *restrict cond, pthread_mutex_t *restrict mutex) {
    int result = pthread_cond_wait(cond, mutex);
    if(result) {
        printf("Falha ao usar pthread_cond_wait\n");
        exit(0);
    }
    return result;
}

int Pthread_cond_signal(pthread_cond_t *cond) {
    int result = pthread_cond_signal(cond);
    if(result) {
        printf("Falha ao usar pthread_cond_signal\n");
        exit(0);
    }
    return result;
}

int Pthread_cond_broadcast(pthread_cond_t *cond) {
    int result = pthread_cond_broadcast(cond);
    if(result) {
        printf("Falha ao usar pthread_cond_broadcast\n");
        exit(0);
    }
    return result;
}

int Sem_init(sem_t *sem, int pshared, unsigned int value) {
    int result = sem_init(sem, pshared, value);
    if(result){
        printf("Falha ao inicializar semaforo\n");
        exit(0);
    }
    return result;
}

int Sem_destroy(sem_t *sem) {
    int result = sem_destroy(sem);
    if(result){
        printf("Falha ao destruir semaforo\n");
        exit(0);
    }
    return result;
}

int Sem_post(sem_t *sem) {
    int result = sem_post(sem);
    if(result){
        printf("Falha ao usar sem_post\n");
        exit(0);
    }
    return result;
}

int Sem_wait(sem_t *sem) {
    int result = sem_wait(sem);
    if(result){
        printf("Falha ao usar sem_wait\n");
        exit(0);
    }
    return result;
}

#endif