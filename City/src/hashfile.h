#ifndef HASHFILE_H
#define HASHFILE_H

/**
 * @file hashfile.h
 * @brief Hashfile dinâmico em disco
 * 
 * Implementa uma tabela hash com encadeamento externo
 * Os dados são armazenados em arquivo binário
 * Suporta inserção, busca, remoção e rehash automático
 */

// Tipo opaco para o hashfile
typedef void* Hash;

/**
 * Cria um novo hashfile
 * @param filename Nome do arquivo onde os dados serão armazenados
 * @param size Número inicial de buckets
 * @param dataSize Tamanho de cada registro de dados
 * @return Ponteiro para o hashfile (NULL se erro)
 */
Hash hf_create(char *filename, int size, int dataSize);

/**
 * Insere um par chave-dado no hashfile
 * @param h Hashfile
 * @param key Chave (string)
 * @param data Ponteiro para o dado (será copiado)
 * @return 1 em sucesso, 0 em erro
 */
int hf_insert(Hash h, char *key, void *data);

/**
 * Busca um dado pela chave
 * @param h Hashfile
 * @param key Chave a buscar
 * @return Ponteiro para o dado (malloc) ou NULL. Deve ser liberado com free()
 */
void* hf_search(Hash h, char *key);

/**
 * Remove um par chave-dado do hashfile
 * @param h Hashfile
 * @param key Chave a remover
 * @return 1 se removido, 0 se não encontrado
 */
int hf_remove(Hash h, char *key);

/**
 * Gera um arquivo de dump (.hfd) com informações do hashfile
 * @param h Hashfile
 * @param filename Nome base para o arquivo dump
 */
void hf_dump(Hash h, const char *filename);

/**
 * Fecha o hashfile e libera recursos
 * @param h Hashfile
 */
void hf_close(Hash h);

#endif
