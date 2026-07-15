#include "../unity/unity.h"
#include "../qry.h"
#include "../banco.h"
#include "../hashfile.h"  // <-- ADICIONAR ESTE INCLUDE
#include "../svg.h"
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
    remove("teste.txt");
}

void test_qry_init_close(void) {
    qry_init("teste.txt");
    qry_close();
    remove("teste.txt");
}

void test_qry_info_pessoa(void) {
    clean_test_files();
    
    hf_set_output_dir(".");
    hf_set_base_name("teste");
    banco_set_output_dir(".");
    banco_init();
    qry_init("teste.txt");
    
    banco_addPessoa("12345678900", "Joao", "Silva", 'M', "01/01/1990");
    qry_infoPessoa("12345678900");
    
    qry_close();
    banco_close();
    clean_test_files();
    remove("teste.txt");
}

void test_qry_nasce(void) {
    clean_test_files();
    
    hf_set_output_dir(".");
    hf_set_base_name("teste");
    banco_set_output_dir(".");
    banco_init();
    
    qry_nasce("12345678900", "Joao", "Silva", 'M', "01/01/1990");
    
    Pessoa *p = banco_getPessoa("12345678900");
    TEST_ASSERT_NOT_NULL(p);
    TEST_ASSERT_EQUAL_STRING("Joao", pessoa_get_nome(p));
    free(p);
    
    banco_close();
    clean_test_files();
}

void test_qry_despejar(void) {
    clean_test_files();
    
    hf_set_output_dir(".");
    hf_set_base_name("teste");
    banco_set_output_dir(".");
    banco_init();
    qry_init("teste.txt");
    
    banco_addQuadra("cep001", 10.0, 20.0, 50.0, 40.0);
    banco_addPessoa("12345678900", "Joao", "Silva", 'M', "01/01/1990");
    banco_morar("12345678900", "cep001", 'N', 10, "Ap101");
    
    qry_despejar("12345678900");
    
    Pessoa *p = banco_getPessoa("12345678900");
    TEST_ASSERT_NOT_NULL(p);
    TEST_ASSERT_EQUAL_INT(0, pessoa_eh_morador(p));
    free(p);
    
    qry_close();
    banco_close();
    clean_test_files();
    remove("teste.txt");
}

int main(void) {
    UNITY_BEGIN();
    
    RUN_TEST(test_qry_init_close);
    RUN_TEST(test_qry_info_pessoa);
    RUN_TEST(test_qry_nasce);
    RUN_TEST(test_qry_despejar);
    
    return UNITY_END();
}
