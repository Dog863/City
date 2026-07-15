#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "hashfile.h"

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

#define KEY_SIZE 50

typedef struct {
    char key[KEY_SIZE];
    long dataOffset;
    long next;
} Node;

typedef struct {
    FILE *file;
    FILE *header_file;
    int size;
    int count;
    int dataSize;
    long *buckets;
    char filename[PATH_MAX];
    char base_name[PATH_MAX];
} HashStruct;

static char hash_output_dir[PATH_MAX] = ".";
static char hash_base_name[PATH_MAX] = "";
static int base_name_defined = 0;

void hf_set_output_dir(const char *dir) {
    if (dir && dir[0] != '\0') {
        strncpy(hash_output_dir, dir, PATH_MAX - 1);
        hash_output_dir[PATH_MAX - 1] = '\0';
    }
}

void hf_set_base_name(const char *name) {
    if (name && name[0] != '\0') {
        strncpy(hash_base_name, name, PATH_MAX - 1);
        hash_base_name[PATH_MAX - 1] = '\0';
        base_name_defined = 1;
        printf("DEBUG: base_name definido como: %s\n", hash_base_name);
    }
}

int hashFunc(char *key, int size){
    int h = 0;
    for(int i=0; key[i]; i++)
        h = (h * 31 + key[i]) % size;
    return h;
}

void hf_rehash(HashStruct *hs){
    int newSize = hs->size * 2;
    long *newBuckets = malloc(sizeof(long) * newSize);
    
    if (!newBuckets) {
        fprintf(stderr, "Erro: Falha na alocação de memória para rehash\n");
        return;
    }
    
    for(int i = 0; i < newSize; i++) 
        newBuckets[i] = -1;
    
    for(int i = 0; i < hs->size; i++){
        long pos = hs->buckets[i];
        
        while(pos != -1){
            Node node;
            fseek(hs->file, pos, SEEK_SET);
            fread(&node, sizeof(Node), 1, hs->file);
            
            long next = node.next;
            int idx = hashFunc(node.key, newSize);
            
            node.next = newBuckets[idx];
            newBuckets[idx] = pos;
            
            fseek(hs->file, pos, SEEK_SET);
            fwrite(&node, sizeof(Node), 1, hs->file);
            
            pos = next;
        }
    }
    
    free(hs->buckets);
    hs->buckets = newBuckets;
    hs->size = newSize;
}

Hash hf_create(char *filename, int size, int dataSize){
    char fullpath[PATH_MAX];
    char headerpath[PATH_MAX];
    char base_name[PATH_MAX];
    char filename_clean[PATH_MAX];
    
    // Limpar o nome do arquivo (remover extensões)
    strncpy(filename_clean, filename, PATH_MAX - 1);
    filename_clean[PATH_MAX - 1] = '\0';
    char *dot = strrchr(filename_clean, '.');
    if (dot) *dot = '\0';
    
    // Verificar se base_name está definido
    if (base_name_defined && strlen(hash_base_name) > 0) {
        snprintf(base_name, sizeof(base_name), "%s-%s", hash_base_name, filename_clean);
        printf("DEBUG: Criando hashfile: %s (base: %s)\n", filename_clean, hash_base_name);
    } else {
        snprintf(base_name, sizeof(base_name), "%s", filename_clean);
        printf("DEBUG: Criando hashfile: %s (sem base)\n", filename_clean);
    }
    
    snprintf(fullpath, sizeof(fullpath), "%s/%s.hf", hash_output_dir, base_name);
    snprintf(headerpath, sizeof(headerpath), "%s/%s.hfc", hash_output_dir, base_name);
    
    // Verificar se o arquivo já existe e remover para evitar duplicação
    if (access(fullpath, F_OK) == 0) {
        printf("DEBUG: Removendo arquivo existente: %s\n", fullpath);
        remove(fullpath);
    }
    if (access(headerpath, F_OK) == 0) {
        printf("DEBUG: Removendo arquivo existente: %s\n", headerpath);
        remove(headerpath);
    }
    
    HashStruct *h = malloc(sizeof(HashStruct));
    if (!h) {
        fprintf(stderr, "Erro: Falha na alocação do hashfile\n");
        return NULL;
    }
    
    h->file = fopen(fullpath, "w+b");
    if (!h->file) {
        fprintf(stderr, "Erro: Não foi possível criar o arquivo %s\n", fullpath);
        free(h);
        return NULL;
    }
    
    // Criar arquivo de cabeçalho
    h->header_file = fopen(headerpath, "w");
    if (!h->header_file) {
        fprintf(stderr, "Erro: Não foi possível criar o arquivo %s\n", headerpath);
        fclose(h->file);
        free(h);
        return NULL;
    }
    
    strcpy(h->filename, base_name);
    strcpy(h->base_name, base_name);
    h->size = size;
    h->count = 0;
    h->dataSize = dataSize;
    
    h->buckets = malloc(sizeof(long) * size);
    if (!h->buckets) {
        fprintf(stderr, "Erro: Falha na alocação dos buckets\n");
        fclose(h->file);
        fclose(h->header_file);
        free(h);
        return NULL;
    }
    
    for(int i = 0; i < size; i++) 
        h->buckets[i] = -1;
    
    // Escrever cabeçalho
    fprintf(h->header_file, "HASHFILE HEADER\n");
    fprintf(h->header_file, "================\n");
    fprintf(h->header_file, "Filename: %s\n", base_name);
    fprintf(h->header_file, "Size: %d\n", size);
    fprintf(h->header_file, "DataSize: %d\n", dataSize);
    fprintf(h->header_file, "Count: %d\n", 0);
    fprintf(h->header_file, "Created: %s\n", __DATE__);
    fprintf(h->header_file, "================\n");
    fflush(h->header_file);
    
    return h;
}

int hf_insert(Hash h, char *key, void *data){
    HashStruct *hs = (HashStruct*) h;
    
    fseek(hs->file, 0, SEEK_END);
    long dataPos = ftell(hs->file);
    fwrite(data, hs->dataSize, 1, hs->file);
    
    Node node;
    strcpy(node.key, key);
    node.dataOffset = dataPos;
    
    int idx = hashFunc(key, hs->size);
    node.next = hs->buckets[idx];
    
    long nodePos = ftell(hs->file);
    fwrite(&node, sizeof(Node), 1, hs->file);
    
    hs->buckets[idx] = nodePos;
    hs->count++;
    
    // Atualizar cabeçalho
    fseek(hs->header_file, 0, SEEK_SET);
    fprintf(hs->header_file, "HASHFILE HEADER\n");
    fprintf(hs->header_file, "================\n");
    fprintf(hs->header_file, "Filename: %s\n", hs->base_name);
    fprintf(hs->header_file, "Size: %d\n", hs->size);
    fprintf(hs->header_file, "DataSize: %d\n", hs->dataSize);
    fprintf(hs->header_file, "Count: %d\n", hs->count);
    fprintf(hs->header_file, "================\n");
    fflush(hs->header_file);
    
    if ((float)hs->count / hs->size > 0.7)
        hf_rehash(hs);
    
    return 1;
}

void* hf_search(Hash h, char *key){
    HashStruct *hs = (HashStruct*) h;
    
    int idx = hashFunc(key, hs->size);
    long pos = hs->buckets[idx];
    
    while(pos != -1){
        Node node;
        fseek(hs->file, pos, SEEK_SET);
        fread(&node, sizeof(Node), 1, hs->file);
        
        if(strcmp(node.key, key) == 0){
            void *data = malloc(hs->dataSize);
            fseek(hs->file, node.dataOffset, SEEK_SET);
            fread(data, hs->dataSize, 1, hs->file);
            return data;
        }
        pos = node.next;
    }
    return NULL;
}

int hf_remove(Hash h, char *key){
    HashStruct *hs = (HashStruct*) h;
    
    int idx = hashFunc(key, hs->size);
    long pos = hs->buckets[idx];
    long prev = -1;
    
    while(pos != -1){
        Node node;
        fseek(hs->file, pos, SEEK_SET);
        fread(&node, sizeof(Node), 1, hs->file);
        
        if(strcmp(node.key, key) == 0){
            if(prev == -1) {
                hs->buckets[idx] = node.next;
            } else {
                Node p;
                fseek(hs->file, prev, SEEK_SET);
                fread(&p, sizeof(Node), 1, hs->file);
                p.next = node.next;
                fseek(hs->file, prev, SEEK_SET);
                fwrite(&p, sizeof(Node), 1, hs->file);
            }
            hs->count--;
            
            // Atualizar cabeçalho
            fseek(hs->header_file, 0, SEEK_SET);
            fprintf(hs->header_file, "HASHFILE HEADER\n");
            fprintf(hs->header_file, "================\n");
            fprintf(hs->header_file, "Filename: %s\n", hs->base_name);
            fprintf(hs->header_file, "Size: %d\n", hs->size);
            fprintf(hs->header_file, "DataSize: %d\n", hs->dataSize);
            fprintf(hs->header_file, "Count: %d\n", hs->count);
            fprintf(hs->header_file, "================\n");
            fflush(hs->header_file);
            
            return 1;
        }
        prev = pos;
        pos = node.next;
    }
    return 0;
}

void hf_dump(Hash h, const char *base_filename) {
    HashStruct *hs = (HashStruct*) h;
    char dumpname[PATH_MAX];
    
    // Usar o nome base do hashfile se disponível
    char final_name[PATH_MAX];
    if (strlen(hs->base_name) > 0) {
        snprintf(final_name, sizeof(final_name), "%s", hs->base_name);
    } else {
        snprintf(final_name, sizeof(final_name), "%s", base_filename);
    }
    
    snprintf(dumpname, sizeof(dumpname), "%s/%s.hfd", hash_output_dir, final_name);
    
    FILE *dump = fopen(dumpname, "w");
    if (!dump) {
        fprintf(stderr, "Warning: Could not create dump file %s\n", dumpname);
        return;
    }
    
    fprintf(dump, "========================================\n");
    fprintf(dump, "HASHFILE DUMP\n");
    fprintf(dump, "========================================\n");
    fprintf(dump, "File: %s\n", final_name);
    fprintf(dump, "Output dir: %s\n", hash_output_dir);
    fprintf(dump, "Size (buckets): %d\n", hs->size);
    fprintf(dump, "Count (elements): %d\n", hs->count);
    fprintf(dump, "Load factor: %.2f (%.1f%%)\n", 
            (float)hs->count / hs->size, 
            ((float)hs->count / hs->size) * 100);
    fprintf(dump, "Data size: %d bytes\n", hs->dataSize);
    fprintf(dump, "========================================\n\n");
    
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

void hf_close(Hash h){
    HashStruct *hs = (HashStruct*) h;
    if (hs->file) fclose(hs->file);
    if (hs->header_file) fclose(hs->header_file);
    if (hs->buckets) free(hs->buckets);
    free(hs);
}
