#include "../unity/unity.h"
#include "../pessoa.h"
#include "../banco.h"
#include "../hashfile.h"  // <-- ADICIONAR ESTE INCLUDE
#include <stdio.h>
#include <stdlib.h>

void setUp(void) {}
void tearDown(void) {}

void clean_test_files(void) {
    remove("teste-quadras.hf");
    remove("teste-quadras.hfc");
    remove("teste-quadras.hfd");
    remove("teste-pessoas.hf");
    remove("teste-pessoas.hfc");
    remove("teste-pessoas.hfd");
    remove("estado.txt");
}

void test_pessoa_insert(void) {
    clean_test_files();
    
    hf_set_output_dir(".");
    hf_set_base_name("teste");
    banco_set_output_dir(".");
    banco_init();
    
    pessoa_insert("12345678900", "Joao", "Silva", 'M', "01/01/1990");
    
    Pessoa *p = banco_getPessoa("12345678900");
    TEST_ASSERT_NOT_NULL(p);
    TEST_ASSERT_EQUAL_STRING("12345678900", pessoa_get_cpf(p));
    TEST_ASSERT_EQUAL_STRING("Joao", pessoa_get_nome(p));
    free(p);
    
    banco_close();
    clean_test_files();
}

void test_pessoa_morar(void) {
    clean_test_files();
    
    hf_set_output_dir(".");
    hf_set_base_name("teste");
    banco_set_output_dir(".");
    banco_init();
    
    banco_addQuadra("cep001", 10.0, 20.0, 50.0, 40.0);
    pessoa_insert("12345678900", "Joao", "Silva", 'M', "01/01/1990");
    pessoa_morar("12345678900", "cep001", "N", 10, "Ap101");
    
    Pessoa *p = banco_getPessoa("12345678900");
    TEST_ASSERT_NOT_NULL(p);
    TEST_ASSERT_EQUAL_INT(1, pessoa_eh_morador(p));
    TEST_ASSERT_EQUAL_STRING("cep001", pessoa_get_cep(p));
    free(p);
    
    banco_close();
    clean_test_files();
}

int main(void) {
    UNITY_BEGIN();
    
    RUN_TEST(test_pessoa_insert);
    RUN_TEST(test_pessoa_morar);
    
    return UNITY_END();
}
