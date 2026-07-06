#include "../unity/unity.h"
#include "../hashfile.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void setUp(void) {}
void tearDown(void) {}

void test_hashfile_criar(void) {
    Hash h = hf_create("teste.hf", 10, sizeof(int));
    TEST_ASSERT_NOT_NULL(h);
    hf_close(h);
    remove("teste.hf");
}

void test_hashfile_inserir_buscar(void) {
    Hash h = hf_create("teste.hf", 10, sizeof(int));
    TEST_ASSERT_NOT_NULL(h);
    
    int valor = 42;
    int resultado = hf_insert(h, "chave1", &valor);
    TEST_ASSERT_EQUAL_INT(1, resultado);
    
    int *buscado = (int*)hf_search(h, "chave1");
    TEST_ASSERT_NOT_NULL(buscado);
    TEST_ASSERT_EQUAL_INT(42, *buscado);
    free(buscado);
    
    hf_close(h);
    remove("teste.hf");
}

void test_hashfile_inserir_multiplos(void) {
    Hash h = hf_create("teste.hf", 5, sizeof(int));
    TEST_ASSERT_NOT_NULL(h);
    
    for (int i = 0; i < 10; i++) {
        int valor = i * 10;
        char key[20];
        sprintf(key, "chave%d", i);
        hf_insert(h, key, &valor);
    }
    
    for (int i = 0; i < 10; i++) {
        char key[20];
        sprintf(key, "chave%d", i);
        int *buscado = (int*)hf_search(h, key);
        TEST_ASSERT_NOT_NULL(buscado);
        TEST_ASSERT_EQUAL_INT(i * 10, *buscado);
        free(buscado);
    }
    
    hf_close(h);
    remove("teste.hf");
}

void test_hashfile_remover(void) {
    Hash h = hf_create("teste.hf", 10, sizeof(int));
    TEST_ASSERT_NOT_NULL(h);
    
    int valor = 42;
    hf_insert(h, "chave1", &valor);
    
    int *buscado = (int*)hf_search(h, "chave1");
    TEST_ASSERT_NOT_NULL(buscado);
    free(buscado);
    
    int resultado = hf_remove(h, "chave1");
    TEST_ASSERT_EQUAL_INT(1, resultado);
    
    buscado = (int*)hf_search(h, "chave1");
    TEST_ASSERT_NULL(buscado);
    
    hf_close(h);
    remove("teste.hf");
}

void test_hashfile_chave_inexistente(void) {
    Hash h = hf_create("teste.hf", 10, sizeof(int));
    TEST_ASSERT_NOT_NULL(h);
    
    int *buscado = (int*)hf_search(h, "chave_inexistente");
    TEST_ASSERT_NULL(buscado);
    
    hf_close(h);
    remove("teste.hf");
}

int main(void) {
    UNITY_BEGIN();
    
    RUN_TEST(test_hashfile_criar);
    RUN_TEST(test_hashfile_inserir_buscar);
    RUN_TEST(test_hashfile_inserir_multiplos);
    RUN_TEST(test_hashfile_remover);
    RUN_TEST(test_hashfile_chave_inexistente);
    
    return UNITY_END();
}