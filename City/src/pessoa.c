#include "pessoa.h"
#include "banco.h"

/**
 * @file pessoa.c
 * @brief Implementação do processamento do arquivo .pm
 */

/**
 * Insere uma nova pessoa no sistema
 * 
 * @param cpf CPF da pessoa
 * @param nome Primeiro nome
 * @param sobrenome Sobrenome
 * @param sexo Sexo
 * @param nasc Data de nascimento
 */
void pessoa_insert(char *cpf, char *nome, char *sobrenome, char sexo, char *nasc){
    // Repassa a operação para o banco de dados
    banco_addPessoa(cpf, nome, sobrenome, sexo, nasc);
}

/**
 * Registra que uma pessoa mora em um endereço
 * 
 * @param cpf CPF da pessoa
 * @param cep CEP da quadra
 * @param face Face (string, pegamos o primeiro caractere)
 * @param num Número do endereço
 * @param compl Complemento
 */
void pessoa_morar(char *cpf, char *cep, char *face, int num, char *compl){
    // face[0] pega o primeiro caractere da string face (ex: "N" -> 'N')
    banco_morar(cpf, cep, face[0], num, compl);
}
