#include "include/conc_util.h" // arquivo contendo os includes gerais
#include "include/timer.h"

#include <string.h>

#define NOME_DO_ARQUIVO_DO_LEITOR "output-leitores/%d.txt"

//frases que serão printadas no log
#define IMPORT_SCRIPT_PYTHON "from verificador import *\n"
#define CHECA_ERROS "if not get_erro(): print(\"\\nPassou no teste!\\n\")\n"
#define TEMPO_EXECUCAO "print(\"Tempo: %.4f segundos\\n\")\n"
#define LEITOR_ENTRANDO "leitor_entrando(%d)\n"
#define LEITOR_SAINDO "leitor_saindo(%d)\n"
#define LEITOR_ESPERANDO "leitor_barrado(%d)\n"
#define LEITOR_SAINDO_ESPERA "leitor_saindo_da_espera(%d)\n"
#define ESCRITOR_ENTRANDO "escritor_entrando(%d)\n"
#define ESCRITOR_SAINDO "escritor_saindo(%d)\n"
#define ESCRITOR_ESPERANDO "escritor_barrado(%d)\n"
#define ESCRITOR_SAINDO_ESPERA "escritor_saindo_da_espera(%d)\n"

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
int vez_do_leitor = 1; // variavel de controle para auxiliar na logica de alternancia

//entrada (menos o nome do arquivo de log)
int numero_leitores = 0;
int numero_escritores = 0;
int numero_leituras = 0;
int numero_escritas = 0;

//arquivos onde os leitores printam o que leram
FILE** arquivos_leitores;

//arquivo do log
FILE* arquivo_log;

void inicia_leitura(int id) {
    Pthread_mutex_lock(&mutex);
    
    // ao inves de somente checar se existe um escritor ativo (como no caso usual), também é checado se
    // temos escritores aguardando E nao for vez do leitor
    // esse OU vai garantir que os leitores nao mantenham escritores em inanicao, devido a uma logica de alternancia
    while(escritor_escrevendo || (escritores_esperando && !vez_do_leitor)) {
        leitores_esperando++;
        // printf("Leitor %d foi bloqueado\n", id+1);
        fprintf(arquivo_log, LEITOR_ESPERANDO, id+1);
        Pthread_cond_wait(&block_leitores, &mutex);
        fprintf(arquivo_log, LEITOR_SAINDO_ESPERA, id+1);
        leitores_esperando--;
    }
    contador_leitores++;
    // printf("Leitor %d está lendo\n", id+1);
    fprintf(arquivo_log, LEITOR_ENTRANDO, id+1);
    Pthread_mutex_unlock(&mutex);
}

void finaliza_leitura(int id) {
    Pthread_mutex_lock(&mutex);
    contador_leitores--;
    vez_do_leitor = 0;  //passa a vez para os escritores
    // printf("Leitor %d finalizando leitura\n", id+1);
    fprintf(arquivo_log, LEITOR_SAINDO, id+1);
    if(escritores_esperando) {
        Pthread_cond_signal(&block_escritores);
    }
    Pthread_mutex_unlock(&mutex);
}

void inicia_escrita(int id) {
    Pthread_mutex_lock(&mutex);
    
    // ao inves de somente checar se existe escritor ativo ou leitores ativos, agora também checa-se se
    // existem leitores aguardando E se é a vez de leitores
    // essas checagens vao garantir que os escritores nao mantenham os leitores em inanicao
    while( escritor_escrevendo || contador_leitores > 0 || (leitores_esperando && vez_do_leitor) ) {
        escritores_esperando++;
        // printf("Escritor %d foi bloqueado\n", id+1);
        fprintf(arquivo_log, ESCRITOR_ESPERANDO, id+1);
        Pthread_cond_wait(&block_escritores, &mutex);
        fprintf(arquivo_log, ESCRITOR_SAINDO_ESPERA, id+1);
        escritores_esperando--;
    }
    escritor_escrevendo = 1;
    // printf("Escritor %d está escrevendo\n", id+1);
    fprintf(arquivo_log, ESCRITOR_ENTRANDO, id+1);
    Pthread_mutex_unlock(&mutex);
}

void finaliza_escrita(int id) {
    Pthread_mutex_lock(&mutex);
    escritor_escrevendo = 0;
    vez_do_leitor = 1;  //passa a vez para os leitores
    // printf("Escritor %d finalizando escrita\n", id+1);
    fprintf(arquivo_log, ESCRITOR_SAINDO, id+1);
    if(leitores_esperando) {
        Pthread_cond_broadcast(&block_leitores);
    }
    if(escritores_esperando) {
        Pthread_cond_signal(&block_escritores);
    }
    Pthread_mutex_unlock(&mutex);
}

// funcoes para as threads leitores/escritoras seguindo a logica usual de leitor-escritor
void* leitura(void* arg) {

    int id = *(int*)arg;
    int i = 0;
    while( i++ < numero_leituras ) {
        inicia_leitura(id);
        int valor_lido = compartilhada;
        finaliza_leitura(id);

        //escreve no arquivo
        fprintf(arquivos_leitores[id], "%d\n", valor_lido);

        //faz qualquer coisa
        int boba = 100000;
        while(boba-- > 0);
    }

    pthread_exit(NULL);
}

void* escrita(void* arg) {

    int id = *(int*)arg;
    int i = 0;
    while( i++ < numero_escritas ) {
        inicia_escrita(id);
        compartilhada = id;
        finaliza_escrita(id);

        //faz qualquer coisa
        int boba = 100000;
        while(boba-- > 0);
    }

    pthread_exit(NULL);
}


int main(int argc, char* argv[]) {

    if(argc != 6) {
        printf("Número de argumentos inválido!\n");
        printf("Você deve inserir a quantidade de leitores e escritores, o número de leituras e escritas,"
                "e o nome do arquivo de log, nessa ordem\n");
        exit(0);
    }

    //lê entrada
    numero_leitores = atoi(argv[1]);
    numero_escritores = atoi(argv[2]);
    numero_leituras = atoi(argv[3]);
    numero_escritas = atoi(argv[4]);
    char nome_do_arquivo_de_log[256];
    strncpy(nome_do_arquivo_de_log, argv[5], 255);

    // faz alocacoes necessarias
    arquivos_leitores = (FILE**)Malloc(sizeof(FILE*) * numero_leitores);

    pthread_t *leitores = (pthread_t*)Malloc(sizeof(pthread_t) * numero_leitores);
    pthread_t *escritores = (pthread_t*)Malloc(sizeof(pthread_t) * numero_escritores);

    int* leitores_id = (int*)Malloc(sizeof(int) * numero_leitores);
    int* escritores_id = (int*)Malloc(sizeof(int) * numero_escritores);

    //abre arquivo de log e escreve seu cabeçalho
    arquivo_log = fopen(nome_do_arquivo_de_log, "w");
    fprintf(arquivo_log, IMPORT_SCRIPT_PYTHON);

    //começa cálculo do tempo de execução
    double tempo_tmp;
    double tempo;
    GET_TIME(tempo_tmp);

    //cria leitores
    for(int i = 0; i < numero_leitores; i++) {
        leitores_id[i] = i;

        //abre arquivo do leitor
        char nome_arq_leitor[256];
        snprintf(nome_arq_leitor, 255, NOME_DO_ARQUIVO_DO_LEITOR, i+1);
        arquivos_leitores[i] = fopen(nome_arq_leitor, "w");

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
        fclose(arquivos_leitores[i]);
    }

    //coleta escritores
    for(int i = 0; i < numero_escritores; i++) {
        Pthread_join(escritores[i], NULL);
    }

    //tempo de execução
    GET_TIME(tempo);
    tempo = tempo - tempo_tmp;

    // imprime no arquivo de log as referencias de checagem de erro e tempo de execucao
    fprintf(arquivo_log, CHECA_ERROS);
    fprintf(arquivo_log, TEMPO_EXECUCAO, tempo);
    fclose(arquivo_log);

    // libera as variaveis que foram alocadas
    free(arquivos_leitores);
    free(leitores);
    free(escritores);
    free(leitores_id);
    free(escritores_id);
 }
