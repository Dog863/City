#ifndef PESSOA_H
#define PESSOA_H

/**
 * @file pessoa.h
 * @brief Processamento do arquivo .pm (pessoas e moradores)
 * 
 * Este módulo processa os comandos do arquivo de entrada .pm
 * que contém informações sobre habitantes da cidade
 */

/**
 * Insere uma nova pessoa no sistema
 * A pessoa começa como não-moradora (sem-teto)
 * 
 * @param cpf CPF da pessoa (identificador único)
 * @param nome Primeiro nome
 * @param sobrenome Sobrenome
 * @param sexo Sexo ('M' para masculino, 'F' para feminino)
 * @param nasc Data de nascimento no formato dd/mm/aaaa
 */
void pessoa_insert(char *cpf, char *nome, char *sobrenome, char sexo, char *nasc);

/**
 * Registra que uma pessoa mora em um endereço
 * 
 * @param cpf CPF da pessoa
 * @param cep CEP da quadra onde a pessoa vai morar
 * @param face Face da quadra ('N', 'S', 'L', 'O')
 * @param num Número do endereço (distância da âncora)
 * @param compl Complemento do endereço (ex: "apto 101", "casa 2")
 */
void pessoa_morar(char *cpf, char *cep, char *face, int num, char *compl);

#endif
