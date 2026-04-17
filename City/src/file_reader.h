#ifndef FILE_READER_H
#define FILE_READER_H

/**
 * @file file_reader.h
 * @brief Leitura dos arquivos de entrada (.geo, .pm, .qry)
 */

/**
 * Lê arquivo .geo (descrição da cidade)
 * Comandos:
 *   q cep x y w h - insere quadra
 *   cq sw fill stroke - define estilo das quadras
 */
void readGeo(char* file);

/**
 * Lê arquivo .pm (pessoas e moradores)
 * Comandos:
 *   p cpf nome sobrenome sexo nasc - insere pessoa
 *   m cpf cep face num compl - pessoa mora em endereço
 */
void readPm(char* file);

/**
 * Lê arquivo .qry (consultas)
 * Comandos:
 *   rq cep - remove quadra
 *   h? cpf - info pessoa
 *   mud cpf cep face num compl - muda endereço
 *   censo - estatísticas
 *   pq cep - conta moradores por face
 *   nasce cpf nome sobrenome sexo nasc - nova pessoa
 *   rip cpf - pessoa falece
 *   dspj cpf - despeja pessoa
 */
void readQry(char* file);

#endif
