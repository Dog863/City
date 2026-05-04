#include "pessoa.h"
#include "banco.h"

void pessoa_insert(char *cpf, char *nome, char *sobrenome, char sexo, char *nasc){
    banco_addPessoa(cpf, nome, sobrenome, sexo, nasc);
}

void pessoa_morar(char *cpf, char *cep, char *face, int num, char *compl){
    banco_morar(cpf, cep, face[0], num, compl);
}