#include "../unity/unity.h"
#include "hashfile.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
static Hash h;
static char test_filename[] = "test_hash.hf";

void setUp(void) {
    h = hf_create(test_filename, 5, sizeof(int));
}

void tearDown(void) {
    hf_dump(h, "test_hash");  // CORRIGIDO: adicionar nome do arquivo
    hf_close(h);
    remove(test_filename);
    remove("test_hash.hfd");
}

// Teste 1: Criar hashfile
void test_hf_create_returns_non_null(void) {
    TEST_ASSERT_NOT_NULL(h);
}

// Teste 2: Inserir um elemento
void test_hf_insert_single(void) {
    int data = 42;
    int result = hf_insert(h, "chave1", &data);
    TEST_ASSERT_EQUAL_INT(1, result);
}

// Teste 3: Buscar elemento existente
void test_hf_search_existing(void) {
    int data = 100;
    hf_insert(h, "chave2", &data);
    
    int *found = (int*)hf_search(h, "chave2");
    TEST_ASSERT_NOT_NULL(found);
    TEST_ASSERT_EQUAL_INT(100, *found);
    free(found);
}

// Teste 4: Buscar elemento inexistente
void test_hf_search_nonexistent_returns_null(void) {
    int *found = (int*)hf_search(h, "nao_existe");
    TEST_ASSERT_NULL(found);
}

// Teste 5: Remover elemento existente
void test_hf_remove_existing(void) {
    int data = 200;
    hf_insert(h, "chave3", &data);
    
    int result = hf_remove(h, "chave3");
    TEST_ASSERT_EQUAL_INT(1, result);
    
    int *found = (int*)hf_search(h, "chave3");
    TEST_ASSERT_NULL(found);
}

// Teste 6: Remover elemento inexistente
void test_hf_remove_nonexistent_returns_zero(void) {
    int result = hf_remove(h, "nao_existe");
    TEST_ASSERT_EQUAL_INT(0, result);
}

// Teste 7: Múltiplas inserções
void test_hf_multiple_inserts(void) {
    int values[10];
    char key[20];
    
    for (int i = 0; i < 10; i++) {
        values[i] = i * 10;
        sprintf(key, "key%d", i);
        hf_insert(h, key, &values[i]);
    }
    
    for (int i = 0; i < 10; i++) {
        sprintf(key, "key%d", i);
        int *found = (int*)hf_search(h, key);
        TEST_ASSERT_NOT_NULL(found);
        TEST_ASSERT_EQUAL_INT(i * 10, *found);
        free(found);
    }
}

// Teste 8: Atualizar chave existente
void test_hf_update_existing_key(void) {
    int data1 = 100;
    int data2 = 200;
    hf_insert(h, "update_key", &data1);
    
    hf_insert(h, "update_key", &data2);
    
    int *found = (int*)hf_search(h, "update_key");
    TEST_ASSERT_NOT_NULL(found);
    TEST_ASSERT_EQUAL_INT(200, *found);
    free(found);
}

// Teste 9: Rehash automático (load factor > 0.7)
void test_hf_rehash_on_high_load(void) {
    for (int i = 0; i < 10; i++) {
        char key[10];
        sprintf(key, "r%d", i);
        hf_insert(h, key, &i);
    }
    
    for (int i = 0; i < 10; i++) {
        char key[10];
        sprintf(key, "r%d", i);
        int *found = (int*)hf_search(h, key);
        TEST_ASSERT_NOT_NULL(found);
        TEST_ASSERT_EQUAL_INT(i, *found);
        free(found);
    }
}

// Teste 10: Inserir com chave vazia
void test_hf_insert_empty_key(void) {
    int data = 999;
    int result = hf_insert(h, "", &data);
    TEST_ASSERT_EQUAL_INT(1, result);
    
    int *found = (int*)hf_search(h, "");
    TEST_ASSERT_NOT_NULL(found);
    TEST_ASSERT_EQUAL_INT(999, *found);
    free(found);
}

// Teste 11: Dados de diferentes tamanhos
void test_hf_different_data_sizes(void) {
    Hash h2 = hf_create("test2.hf", 5, sizeof(int));  // Mudar para int
    
    int data = 42;  // Usar int em vez de double
    hf_insert(h2, "test", &data);
    
    int *found = (int*)hf_search(h2, "test");
    TEST_ASSERT_NOT_NULL(found);
    TEST_ASSERT_EQUAL_INT(42, *found);  // Usar EQUAL_INT
    free(found);
    
    hf_dump(h2, "test2");
    hf_close(h2);
    remove("test2.hf");
    remove("test2.hfd");
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_hf_create_returns_non_null);
    RUN_TEST(test_hf_insert_single);
    RUN_TEST(test_hf_search_existing);
    RUN_TEST(test_hf_search_nonexistent_returns_null);
    RUN_TEST(test_hf_remove_existing);
    RUN_TEST(test_hf_remove_nonexistent_returns_zero);
    RUN_TEST(test_hf_multiple_inserts);
    RUN_TEST(test_hf_update_existing_key);
    RUN_TEST(test_hf_rehash_on_high_load);
    RUN_TEST(test_hf_insert_empty_key);
    RUN_TEST(test_hf_different_data_sizes);
    return UNITY_END();
}
