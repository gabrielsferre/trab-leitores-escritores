# -*- coding: utf-8 -*- 

leitores_lendo = set()
leitores_esperando = set()
escritores_escrevendo = set()
escritores_esperando = set()
vez_do_leitor = True
erro = False

def leitor_esperando():
    return len(leitores_esperando) > 0

def escritor_esperando():
    return len(escritores_esperando) > 0

def leitor_lendo():
    return len(leitores_lendo) > 0

def escritor_escrevendo():
    return len(escritores_escrevendo) > 0

def erro_encontrado():
    global erro
    erro = True

def get_erro():
    return erro

#funções que checam se as condições do problema estão sendo respeitadas
def escritores_simultaneos():
    if len(escritores_escrevendo) > 1:
        print("ERRO :: Escritores {} escrevendo ao mesmo tempo".format(escritores_escrevendo))
        erro_encontrado()

def leitor_escritor_simultaneos():
    if leitor_lendo() and escritor_escrevendo():
        print("ERRO :: Leitores {} lendo enquanto escritores {} estão escrevendo".format(leitores_lendo, escritores_escrevendo))
        erro_encontrado()

def sanity_check():
    escritores_simultaneos()
    leitor_escritor_simultaneos()

#decorator para as funções que serão chamadas no log
def log(func):
    def wrapper(*args, **kwargs):
        result = func(*args,**kwargs)
        sanity_check()
        return result
    return wrapper

#funções que serão chamadas no log gerado pelo programa concorrente
@log
def leitor_entrando(id):
    print("Leitor {} começando a ler".format(id))

    if (not vez_do_leitor) and escritor_esperando():
        print("ERRO :: Leitor {} começando a ler na vez dos escritores {} que esperavam".format(id, escritores_esperando))
        erro_encontrado()
    if id in leitores_lendo:
        print("ERRO :: Leitor {} voltando a ler sem ter registrado a saída".format(id))
        erro_encontrado()
    leitores_lendo.add(id)

@log
def escritor_entrando(id):
    print("Escritor {} começando a escrever".format(id))

    if vez_do_leitor and leitor_esperando():
        print("ERRO :: Escritor {} começando a escrever na vez dos leitores {} que esperavam".format(id, leitores_esperando))
        erro_encontrado()
    if id in leitores_lendo:
        print("ERRO :: Escritor {} voltando a escrever sem ter registrado a saída".format(id))
        erro_encontrado()
    escritores_escrevendo.add(id)

@log
def leitor_saindo(id):
    global vez_do_leitor
    print("Leitor {} parando de ler".format(id))

    if id not in leitores_lendo:
        print("ERRO :: Leitor {} parando de ler sem ter registrado a entrada".format(id))
        erro_encontrado()
    else:
        leitores_lendo.remove(id)
    vez_do_leitor = False

@log
def escritor_saindo(id):
    global vez_do_leitor
    print("Escritor {} parando de escrever".format(id))

    if id not in escritores_escrevendo:
        print("ERRO :: Escritor {} parando de escrever sem ter registrado a entrada".format(id))
        erro_encontrado()
    else:
        escritores_escrevendo.remove(id)
    vez_do_leitor = True

@log
def leitor_barrado(id):
    if not vez_do_leitor and escritor_esperando() and not escritor_escrevendo():
        print("IMPEDINDO INANIÇÃO :: Leitor {} barrado pois um escritor estava esperando".format(id))
    else:
        print("Leitor {} esperando".format(id))

    if id in leitores_esperando:
        print("ERRO :: Leitor {} entrando na espera sem ter registrado saída da espera".format(id))
        erro_encontrado()
    if not escritor_escrevendo() and (vez_do_leitor or not escritor_esperando()):
        print("ERRO :: Leitor {} esperando sem motivo".format(id))
        erro_encontrado()
    leitores_esperando.add(id)

@log
def escritor_barrado(id):
    if vez_do_leitor and leitor_esperando() and not leitor_lendo():
        print("IMPEDINDO INANIÇÃO :: Escritor {} barrado pois um leitor estava esperando".format(id))
    else:
        print("Escritor {} esperando".format(id))

    if id in escritores_esperando:
        print("ERRO :: Escritor {} entrando na espera sem ter registrado saída da espera".format(id))
        erro_encontrado()
    if not escritor_escrevendo() and not leitor_lendo() and (not vez_do_leitor or not leitor_esperando()):
        print("ERRO :: Escritor {} esperando sem motivo".format(id))
        erro_encontrado()
    escritores_esperando.add(id)

@log
def leitor_saindo_da_espera(id):
    if id not in leitores_esperando:
        print("ERRO :: Leitor {} saindo da espera sem ter registrado que estava esperando".format(id))
        erro_encontrado()
    else:
        leitores_esperando.remove(id)

@log
def escritor_saindo_da_espera(id):
    if id not in escritores_esperando:
        print("ERRO :: Escritor {} saindo da espera sem ter registrado que estava esperando".format(id))
        erro_encontrado()
    else:
        escritores_esperando.remove(id)
