 #include "../include/conc_util.h"

#define NOME_DO_ARQUIVO_DE_LOG "log/log.py"
#define NOME_DO_ARQUIVO_DO_LEITOR(id) "output-leitores/"(#id)".txt"

//frases que serão printadas no log
#define LEITOR_ENTRANDO "leitor_entrando(%d)\n"
#define LEITOR_SAINDO "leitor_saindo(%d)\n"
#define LEITOR_ESPERANDO "leitor_barrado(%d)\n"
#define ESCRITOR_ENTRANDO "escritor_entrando(%d)\n"
#define ESCRITOR_SAINDO "escritor_saindo(%d)\n"
#define ESCRITOR_ESPERANDO "escritor_barrado(%d)\n"

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t block_leitores = PTHREAD_COND_INITIALIZER;
pthread_cond_t block_escritores = PTHREAD_COND_INITIALIZER;

//variável que será lida pelos leitores e sobreescrita pelos escritores
int compartilhada = 0;

//variáveis de estado usadas nas threads
int contador_leitores = 0;      //conta quantos leitores estão lendo
int leitores_esperando = 0;     //leitores presos no cond_wait
int escritores_esperando = 0;   //escritores presos no cond_wait
int escritor_escrevendo = 0;
int vez_do_leitor = 1;

//entrada (menos o nome do arquivo de log)
int numero_leitores = 0;
int numero_escritores = 0;
int numero_leituras = 0;
int numero_escritas = 0;

//arquivos onde os leitores printam o que leram
FILE* arquivos_leitores;

//arquivo do log
FILE arquivo_log;

void inicia_leitura(int id) {
    Pthread_mutex_lock(&mutex);
    while(escritor_escrevendo || (escritores_esperando && !vez_do_leitor)) {
        leitores_esperando++;
        printf("Leitor %d foi bloqueado\n", id);
        Pthread_cond_wait(&block_leitores, &mutex);
        leitores_esperando--;
    }
    contador_leitores++;
    printf("Leitor %d está lendo\n", id);
    Pthread_mutex_unlock(&mutex);
}

void finaliza_leitura(int id) {
    Pthread_mutex_lock(&mutex);
    contador_leitores--;
    vez_do_leitor = 0;  //passa a vez para os escritores
    printf("Leitor %d finalizando leitura\n", id);
    if(escritores_esperando) {
        Pthread_cond_signal(&block_escritores);
    }
    Pthread_mutex_unlock(&mutex);
}

void inicia_escrita(int id) {
    Pthread_mutex_lock(&mutex);
    while( escritor_escrevendo || contador_leitores > 0 || (leitores_esperando && vez_do_leitor) ) {
        escritores_esperando++;
        printf("Escritor %d foi bloqueado\n", id);
        Pthread_cond_wait(&block_escritores, &mutex);
        escritores_esperando--;
    }
    escritor_escrevendo = 1;
    printf("Escritor %d está escrevendo\n", id);
    Pthread_mutex_unlock(&mutex);
}

void finaliza_escrita(int id) {
    Pthread_mutex_lock(&mutex);
    escritor_escrevendo = 0;
    vez_do_leitor = 1;  //passa a vez para os leitores
    printf("Escritor %d finalizando escrita\n", id);
    if(leitores_esperando) {
        Pthread_cond_broadcast(&block_leitores);
    }
    Pthread_mutex_unlock(&mutex);
}

void* leitura(void* arg) {

    int id = *(int*)arg;

    while(1) {
        inicia_leitura(id);
        int valor_lido = compartilhada;
        finaliza_leitura(id);
    }

    pthread_exit(NULL);
}

void* escrita(void* arg) {

    int id = *(int*)arg;

    while(1) {
        inicia_escrita(id);
        compartilhada = id;
        finaliza_escrita(id);
    }

    pthread_exit(NULL);
}


int main() {
    
    arquivos_leitores = (FILE*)Malloc(sizeof(FILE) * numero_leitores);

    pthread_t *leitores = (pthread_t*)Malloc(sizeof(pthread_t) * numero_leitores);
    pthread_t *escritores = (pthread_t*)Malloc(sizeof(pthread_t) * numero_escritores);

    int *leitores_id = (int*)Malloc(sizeof(int) * numero_leitores);
    int *escritores_id = (int*)Malloc(sizeof(int) * numero_escritores);

    //cria leitores
    for(int i = 0; i < numero_leitores; i++) {
        leitores_id[i] = i;
        Pthread_create(leitores + i, NULL, leitura, leitores_id + i);
    }

    //cria escritores
    for(int i = 0; i < numero_escritores; i++) {
        escritores_id[i] = i;
        Pthread_create(escritores + i, NULL, escrita, escritores_id + i);
    }

    //coleta leitores
    for(int i = 0; i < numero_leitores; i++) {
        Pthread_join(leitores[i], NULL);
    }

    //coleta escritores
    for(int i = 0; i < numero_escritores; i++) {
        Pthread_join(escritores[i], NULL);
    }

    free(arquivos_leitores);
    free(leitores);
    free(escritores);
    free(leitores_id);
    free(escritores_id);
 }