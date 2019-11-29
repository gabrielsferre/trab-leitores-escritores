#### Usando o makefile
Para usar o makefile, basta executar o comando make passando para a variável ARGS o número de leitores, escritores,
leituras e escritas nessa ordem.
Ex: rodando o programa com 6 leitores, 5 escritores, com 10 leituras e 11 escritas:

      make ARGS="6 5 10 11"

O código em C será compilado e executado, gerando o script Python dentro na pasta 'log', e em seguida esse script
também será executado.

#### Sem makefile
Primeiro, compilar o código C usando a pthread:

      gcc -Wall leitores_escritores.c -o saida.out -lpthread
      
Em seguida, executar a saída gerada passando o número de leitores, escritores, leituras, escritas e o nome do arquivo de log:

      ./saida.out 6 5 10 11 log/log.py

Agora só resta executar o log. Ele deve estar na mesma pasta que o arquivo 'verificador.py':

      python3 log/log.py
