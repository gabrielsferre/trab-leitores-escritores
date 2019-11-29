VERIFICADOR = log/verificador.py
TESTADOR_VERIFICADOR = log/teste_verificador.py
ARQUIVO_C = leitores_escritores.c
ARQUIVO_LOG = log/log.py

#deve ser preenchida no terminal, ao chamar make, com os
#argumentos do executável menos o nome do arquivo do log
ARGS = 

all: saida.out $(VERIFICADOR)
	./saida.out $(ARGS) $(ARQUIVO_LOG)
	python $(ARQUIVO_LOG)

saida.out: $(ARQUIVO_C)
	gcc -Wall $(ARQUIVO_C) -o saida.out -lpthread



testa-verificador: $(TESTADOR_VERIFICADOR) $(VERIFICADOR)
	python $(TESTADOR_VERIFICADOR)

clean:
	rm *.out
	rm $(ARQUIVO_LOG)
	rm output-leitores/*.txt 
