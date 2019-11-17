from verificador import *

def limpa_globais():
    global vez_do_leitor
    leitores_lendo.clear()
    leitores_esperando.clear()
    escritores_escrevendo.clear()
    escritores_esperando.clear()
    vez_do_leitor = True

def teste(func):
    def wrapper(*args, **kwargs):
        limpa_globais()
        func(*args,**kwargs)
    return wrapper

@teste
def dois_escritores():
    escritor_entrando(1)
    escritor_entrando(2)

@teste
def escritor_leitor():
    escritor_entrando(1)
    leitor_entrando(1)

@teste
def escritor_na_vez_do_leitor():
    leitor_entrando(1)
    escritor_barrado(1)
    leitor_saindo(1)
    leitor_entrando(2)

@teste
def leitor_na_vez_do_escritor():
    escritor_entrando(1)
    leitor_barrado(1)
    escritor_saindo(1)
    escritor_entrando(2)

@teste
def leitor_esperando_sem_motivo():
    leitor_entrando(1)
    leitor_barrado(2)

@teste
def escritor_esperando_sem_motivo():
    leitor_entrando(1)
    leitor_saindo(1)
    escritor_barrado(1)

print("Dois escritores simultâneos:")
dois_escritores()
print("")

print("Escritor e leitor simultâneos:")
escritor_leitor()
print("")

print("Escritor na vez do leitor:")
escritor_na_vez_do_leitor()
print("")

print("Leitor na vez do escritor:")
leitor_na_vez_do_escritor()
print("")

print("Leitor esperando sem motivo:")
leitor_esperando_sem_motivo()
print("")

print("Escritor esperando sem motivo:")
escritor_esperando_sem_motivo()
print("")