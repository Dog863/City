#include "pessoa.h"
#include "banco.h"

void pessoa_insert(char *cpf, char *nome, char *sobrenome, char sexo, char *nasc){
    banco_addPessoa(cpf, nome, sobrenome, sexo, nasc);
}

void pessoa_morar(char *cpf, char *cep, char *face_str, int num, char *compl){
    char face_char = face_str[0];
    banco_morar(cpf, cep, face_char, num, compl);
}