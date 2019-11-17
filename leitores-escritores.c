 #include "../include/conc_util.h"

#define N_LEITORES 4
#define N_ESCRITORES 2

#define TAM_VETOR 10000000

int vect[TAM_VETOR];

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t block_leitores = PTHREAD_COND_INITIALIZER;
pthread_cond_t block_escritores = PTHREAD_COND_INITIALIZER;

int contador_leitores = 0;
int leitores_esperando = 0;
int escritores_esperando = 0;
int escritor_escrevendo = 0;
int vez_do_leitor = 1;

double calcula_media(int* v, int tam) {
    double soma = 0;
    for(int i = 0; i < tam; i++) {
        soma += v[i];
    }
    return soma;
}

void escreve_no_vetor(int* v, int tam, int tid) {
    v[0] = tid;
    v[tam-1] = tid;
    for (int i = 1; i < tam - 1; i++) {
        v[i] = 2 * tid;
    }
}

void inicia_leitura(int tid) {
    Pthread_mutex_lock(&mutex);
    while(escritor_escrevendo || (escritores_esperando && !vez_do_leitor)) {
        leitores_esperando++;
        printf("Leitor %d foi bloqueado\n", tid);
        Pthread_cond_wait(&block_leitores, &mutex);
        leitores_esperando--;
    }
    contador_leitores++;
    printf("Leitor %d está lendo\n", tid);
    Pthread_mutex_unlock(&mutex);
}

void finaliza_leitura(int tid) {
    Pthread_mutex_lock(&mutex);
    contador_leitores--;
    vez_do_leitor = 0;
    printf("Leitor %d finalizando leitura\n", tid);
    if(escritores_esperando) {
        Pthread_cond_signal(&block_escritores);
    }
    Pthread_mutex_unlock(&mutex);
}

void inicia_escrita(int tid) {
    Pthread_mutex_lock(&mutex);
    while( escritor_escrevendo || contador_leitores > 0 || (leitores_esperando && vez_do_leitor) ) {
        escritores_esperando++;
        printf("Escritor %d foi bloqueado\n", tid);
        Pthread_cond_wait(&block_escritores, &mutex);
        escritores_esperando--;
    }
    escritor_escrevendo = 1;
    printf("Escritor %d está escrevendo\n", tid);
    Pthread_mutex_unlock(&mutex);
}

void finaliza_escrita(int tid) {
    Pthread_mutex_lock(&mutex);
    escritor_escrevendo = 0;
    vez_do_leitor = N_LEITORES;
    printf("Escritor %d finalizando escrita\n", tid);
    if( leitores_esperando ) {
        Pthread_cond_broadcast(&block_leitores);
    }
    Pthread_mutex_unlock(&mutex);
}

void* leitura(void* arg) {

    int tid = *(int*)arg;

    while(1) {
        inicia_leitura(tid);

        //calcula média
        double media = calcula_media(vect, TAM_VETOR);
        printf("Media calculada por %d: %.3f\n", tid, media);

        finaliza_leitura(tid);
    }

    pthread_exit(NULL);
}

void* escrita(void* arg) {

    int tid = *(int*)arg;

    while(1) {
        inicia_escrita(tid);
        
        escreve_no_vetor(vect, TAM_VETOR, tid);

        finaliza_escrita(tid);
    }

    pthread_exit(NULL);
}


int main() {

    pthread_t leitores[N_LEITORES];
    pthread_t escritores[N_ESCRITORES];

    pthread_t leitores_tid[N_LEITORES];
    pthread_t escritores_tid[N_ESCRITORES];

    //cria leitores
    for(int i = 0; i < N_LEITORES; i++) {
        leitores_tid[i] = i;
        Pthread_create(leitores + i, NULL, leitura, leitores_tid + i);
    }

    //cria escritores
    for(int i = 0; i < N_ESCRITORES; i++) {
        escritores_tid[i] = i;
        Pthread_create(escritores + i, NULL, escrita, escritores_tid + i);
    }

    //coleta leitores
    for(int i = 0; i < N_LEITORES; i++) {
        Pthread_join(leitores[i], NULL);
    }

    //coleta escritores
    for(int i = 0; i < N_ESCRITORES; i++) {
        Pthread_join(escritores[i], NULL);
    }

 }