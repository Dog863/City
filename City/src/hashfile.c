#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "hashfile.h"

/**
 * @file hashfile.c
 * @brief Implementação do hashfile dinâmico
 * 
 * Estrutura do arquivo:
 * - Nós (Node) contêm: chave, offset dos dados, próximo nó
 * - Dados são armazenados em posições separadas
 * - Buckets são arrays em memória que apontam para o primeiro nó
 */

#define KEY_SIZE 50  // Tamanho máximo da chave

/**
 * Nó da lista encadeada (armazenado no disco)
 * Cada nó representa um elemento na tabela hash
 */
typedef struct {
    char key[KEY_SIZE];   // Chave (ex: CEP ou CPF)
    long dataOffset;      // Posição no arquivo onde os dados estão
    long next;            // Posição do próximo nó (-1 se fim da lista)
} Node;

/**
 * Estrutura do hashfile (mantida em memória)
 * Contém o arquivo, tabela de buckets e estatísticas
 */
typedef struct {
    FILE *file;           // Arquivo onde os dados são armazenados
    int size;             // Número de buckets (tamanho da tabela)
    int count;            // Número total de elementos armazenados
    int dataSize;         // Tamanho de cada dado (ex: sizeof(Quadra))
    long *buckets;        // Array de buckets (posições dos nós cabeça)
} HashStruct;

/**
 * Função hash: converte string em índice do bucket
 * Usa algoritmo de multiplicação por 31 (similar a Java)
 * 
 * @param key Chave a ser hasheada
 * @param size Número de buckets
 * @return Índice do bucket (0 a size-1)
 */
int hashFunc(char *key, int size){
    int h = 0;
    for(int i=0; key[i]; i++)
        h = (h * 31 + key[i]) % size;
    return h;
}

/**
 * Rehash: dobra o tamanho da tabela e redistribui os elementos
 * Chamado automaticamente quando o fator de carga > 0.7
 * 
 * @param hs Ponteiro para a estrutura do hashfile
 */
void hf_rehash(HashStruct *hs){
    int newSize = hs->size * 2;
    long *newBuckets = malloc(sizeof(long) * newSize);
    
    if (!newBuckets) {
        fprintf(stderr, "Erro: Falha na alocação de memória para rehash\n");
        return;
    }
    
    // Inicializa novos buckets com -1 (vazio)
    for(int i = 0; i < newSize; i++) 
        newBuckets[i] = -1;
    
    // Percorre todos os buckets antigos
    for(int i = 0; i < hs->size; i++){
        long pos = hs->buckets[i];
        
        // Percorre a lista encadeada do bucket
        while(pos != -1){
            Node node;
            // Lê o nó do disco
            fseek(hs->file, pos, SEEK_SET);
            fread(&node, sizeof(Node), 1, hs->file);
            
            long next = node.next;
            
            // Calcula novo índice para a chave
            int idx = hashFunc(node.key, newSize);
            
            // Insere no início da lista do novo bucket
            node.next = newBuckets[idx];
            newBuckets[idx] = pos;
            
            // Atualiza o nó no disco
            fseek(hs->file, pos, SEEK_SET);
            fwrite(&node, sizeof(Node), 1, hs->file);
            
            pos = next;
        }
    }
    
    // Substitui a tabela antiga pela nova
    free(hs->buckets);
    hs->buckets = newBuckets;
    hs->size = newSize;
}

/**
 * Cria um novo hashfile
 * 
 * @param filename Nome do arquivo onde os dados serão armazenados
 * @param size Número inicial de buckets
 * @param dataSize Tamanho de cada registro de dados
 * @return Ponteiro para o hashfile (NULL se erro)
 */
Hash hf_create(char *filename, int size, int dataSize){
    // Aloca estrutura principal
    HashStruct *h = malloc(sizeof(HashStruct));
    if (!h) {
        fprintf(stderr, "Erro: Falha na alocação do hashfile\n");
        return NULL;
    }
    
    // Abre arquivo para leitura/escrita binária
    h->file = fopen(filename, "w+b");
    if (!h->file) {
        fprintf(stderr, "Erro: Não foi possível criar o arquivo %s\n", filename);
        free(h);
        return NULL;
    }
    
    // Inicializa campos
    h->size = size;
    h->count = 0;
    h->dataSize = dataSize;
    
    // Aloca tabela de buckets
    h->buckets = malloc(sizeof(long) * size);
    if (!h->buckets) {
        fprintf(stderr, "Erro: Falha na alocação dos buckets\n");
        fclose(h->file);
        free(h);
        return NULL;
    }
    
    // Inicializa buckets com -1 (vazio)
    for(int i = 0; i < size; i++) 
        h->buckets[i] = -1;
    
    return h;
}

/**
 * Insere um par chave-dado no hashfile
 * 
 * @param h Hashfile
 * @param key Chave (string)
 * @param data Ponteiro para o dado (será copiado)
 * @return 1 em sucesso, 0 em erro
 */
int hf_insert(Hash h, char *key, void *data){
    HashStruct *hs = (HashStruct*) h;
    
    // Posiciona no final do arquivo para escrever os dados
    fseek(hs->file, 0, SEEK_END);
    long dataPos = ftell(hs->file);
    
    // Escreve os dados no arquivo
    fwrite(data, hs->dataSize, 1, hs->file);
    
    // Cria o nó
    Node node;
    strcpy(node.key, key);
    node.dataOffset = dataPos;
    
    // Calcula o bucket
    int idx = hashFunc(key, hs->size);
    node.next = hs->buckets[idx];
    
    // Escreve o nó no arquivo
    long nodePos = ftell(hs->file);
    fwrite(&node, sizeof(Node), 1, hs->file);
    
    // Atualiza o bucket para apontar para o novo nó
    hs->buckets[idx] = nodePos;
    hs->count++;
    
    // Verifica se precisa fazer rehash (fator de carga > 0.7)
    if ((float)hs->count / hs->size > 0.7)
        hf_rehash(hs);
    
    return 1;
}

/**
 * Busca um dado pela chave
 * 
 * @param h Hashfile
 * @param key Chave a buscar
 * @return Ponteiro para o dado (malloc) ou NULL. Deve ser liberado com free()
 */
void* hf_search(Hash h, char *key){
    HashStruct *hs = (HashStruct*) h;
    
    // Calcula o bucket
    int idx = hashFunc(key, hs->size);
    long pos = hs->buckets[idx];
    
    // Percorre a lista encadeada
    while(pos != -1){
        Node node;
        fseek(hs->file, pos, SEEK_SET);
        fread(&node, sizeof(Node), 1, hs->file);
        
        // Compara a chave
        if(strcmp(node.key, key) == 0){
            // Encontrou! Lê os dados
            void *data = malloc(hs->dataSize);
            fseek(hs->file, node.dataOffset, SEEK_SET);
            fread(data, hs->dataSize, 1, hs->file);
            return data;
        }
        pos = node.next;
    }
    return NULL;
}

/**
 * Remove um par chave-dado do hashfile
 * 
 * @param h Hashfile
 * @param key Chave a remover
 * @return 1 se removido, 0 se não encontrado
 */
int hf_remove(Hash h, char *key){
    HashStruct *hs = (HashStruct*) h;
    
    // Calcula o bucket
    int idx = hashFunc(key, hs->size);
    long pos = hs->buckets[idx];
    long prev = -1;
    
    // Percorre a lista encadeada
    while(pos != -1){
        Node node;
        fseek(hs->file, pos, SEEK_SET);
        fread(&node, sizeof(Node), 1, hs->file);
        
        if(strcmp(node.key, key) == 0){
            // Encontrou! Remove da lista
            if(prev == -1) {
                // É o primeiro da lista
                hs->buckets[idx] = node.next;
            } else {
                // Atualiza o nó anterior para pular o atual
                Node p;
                fseek(hs->file, prev, SEEK_SET);
                fread(&p, sizeof(Node), 1, hs->file);
                p.next = node.next;
                fseek(hs->file, prev, SEEK_SET);
                fwrite(&p, sizeof(Node), 1, hs->file);
            }
            hs->count--;
            return 1;
        }
        prev = pos;
        pos = node.next;
    }
    return 0;
}

/**
 * Gera um arquivo de dump (.hfd) com informações do hashfile
 * Útil para debug e visualização da estrutura da tabela hash
 * 
 * @param h Hashfile
 * @param base_filename Nome base para o arquivo dump
 */
void hf_dump(Hash h, const char *base_filename) {
    HashStruct *hs = (HashStruct*) h;
    char dumpname[256];
    
    // Criar nome do arquivo dump
    if (base_filename && base_filename[0] != '\0') {
        snprintf(dumpname, sizeof(dumpname), "%s.hfd", base_filename);
    } else {
        snprintf(dumpname, sizeof(dumpname), "hashfile_dump.hfd");
    }
    
    FILE *dump = fopen(dumpname, "w");
    if (!dump) {
        fprintf(stderr, "Warning: Could not create dump file %s\n", dumpname);
        return;
    }
    
    // Cabeçalho do dump
    fprintf(dump, "========================================\n");
    fprintf(dump, "HASHFILE DUMP\n");
    fprintf(dump, "========================================\n");
    fprintf(dump, "File: %s\n", base_filename ? base_filename : "unknown");
    fprintf(dump, "Size (buckets): %d\n", hs->size);
    fprintf(dump, "Count (elements): %d\n", hs->count);
    fprintf(dump, "Load factor: %.2f (%.1f%%)\n", 
            (float)hs->count / hs->size, 
            ((float)hs->count / hs->size) * 100);
    fprintf(dump, "Data size: %d bytes\n", hs->dataSize);
    fprintf(dump, "========================================\n\n");
    
    // Lista cada bucket
    for (int i = 0; i < hs->size; i++) {
        fprintf(dump, "Bucket %3d:", i);
        long pos = hs->buckets[i];
        int bucket_count = 0;
        
        while (pos != -1) {
            Node node;
            fseek(hs->file, pos, SEEK_SET);
            if (fread(&node, sizeof(Node), 1, hs->file) != 1) {
                fprintf(dump, "\n  [ERROR] Failed to read node at offset %ld\n", pos);
                break;
            }
            
            if (bucket_count == 0) {
                fprintf(dump, "\n");
            }
            fprintf(dump, "    [%d] key='%s' data_offset=%ld next=%ld\n", 
                    bucket_count, node.key, node.dataOffset, node.next);
            pos = node.next;
            bucket_count++;
        }
        
        if (bucket_count == 0) {
            fprintf(dump, " empty\n");
        } else {
            fprintf(dump, "    (total: %d node(s))\n", bucket_count);
        }
    }
    
    // Estatísticas finais
    fprintf(dump, "\n========================================\n");
    fprintf(dump, "STATISTICS\n");
    fprintf(dump, "========================================\n");
    fprintf(dump, "Total buckets: %d\n", hs->size);
    fprintf(dump, "Total nodes: %d\n", hs->count);
    fprintf(dump, "Average bucket length: %.2f\n", 
            hs->size > 0 ? (float)hs->count / hs->size : 0);
    fprintf(dump, "Empty buckets: ");
    
    int empty_count = 0;
    for (int i = 0; i < hs->size; i++) {
        if (hs->buckets[i] == -1) empty_count++;
    }
    fprintf(dump, "%d (%.1f%%)\n", empty_count, 
            hs->size > 0 ? (float)empty_count / hs->size * 100 : 0);
    
    fclose(dump);
}

/**
 * Fecha o hashfile e libera recursos
 * 
 * @param h Hashfile
 */
void hf_close(Hash h){
    HashStruct *hs = (HashStruct*) h;
    if (hs->file) fclose(hs->file);
    if (hs->buckets) free(hs->buckets);
    free(hs);
}
