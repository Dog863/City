#include "../unity/unity.h"
#include "../banco.h"
#include "../hashfile.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void setUp(void) {}
void tearDown(void) {}

// ============================================================
// TESTES DA QUADRA
// ============================================================

void test_banco_add_quadra(void) {
    banco_set_output_dir(".");
    banco_init();
    
    banco_addQuadra("cep001", 10.0, 20.0, 50.0, 40.0);
    
    Quadra *q = banco_getQuadra("cep001");
    TEST_ASSERT_NOT_NULL(q);
    TEST_ASSERT_EQUAL_STRING("cep001", quadra_get_cep(q));
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 10.0f, (float)quadra_get_x(q));
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 20.0f, (float)quadra_get_y(q));
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 50.0f, (float)quadra_get_w(q));
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 40.0f, (float)quadra_get_h(q));
    free(q);
    
    banco_close();
    remove("quadras.hf");
    remove("pessoas.hf");
    remove("estado.txt");
}

void test_banco_add_quadra_duplicada(void) {
    banco_set_output_dir(".");
    banco_init();
    
    banco_addQuadra("cep001", 10.0, 20.0, 50.0, 40.0);
    banco_addQuadra("cep001", 20.0, 30.0, 60.0, 50.0);
    
    Quadra *q = banco_getQuadra("cep001");
    TEST_ASSERT_NOT_NULL(q);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 10.0f, (float)quadra_get_x(q));
    free(q);
    
    banco_close();
    remove("quadras.hf");
    remove("pessoas.hf");
    remove("estado.txt");
}

void test_banco_remove_quadra(void) {
    banco_set_output_dir(".");
    banco_init();
    
    banco_addQuadra("cep001", 10.0, 20.0, 50.0, 40.0);
    Quadra *q = banco_getQuadra("cep001");
    TEST_ASSERT_NOT_NULL(q);
    free(q);
    
    banco_removeQuadra("cep001");
    q = banco_getQuadra("cep001");
    TEST_ASSERT_NULL(q);
    
    banco_close();
    remove("quadras.hf");
    remove("pessoas.hf");
    remove("estado.txt");
}

void test_banco_add_pessoa(void) {
    banco_set_output_dir(".");
    banco_init();
    
    banco_addPessoa("12345678900", "Joao", "Silva", 'M', "01/01/1990");
    
    Pessoa *p = banco_getPessoa("12345678900");
    TEST_ASSERT_NOT_NULL(p);
    TEST_ASSERT_EQUAL_STRING("12345678900", pessoa_get_cpf(p));
    TEST_ASSERT_EQUAL_STRING("Joao", pessoa_get_nome(p));
    TEST_ASSERT_EQUAL_STRING("Silva", pessoa_get_sobrenome(p));
    TEST_ASSERT_EQUAL_CHAR('M', pessoa_get_sexo(p));
    TEST_ASSERT_EQUAL_STRING("01/01/1990", pessoa_get_nasc(p));
    free(p);
    
    banco_close();
    remove("quadras.hf");
    remove("pessoas.hf");
    remove("estado.txt");
}

void test_banco_morar(void) {
    banco_set_output_dir(".");
    banco_init();
    
    banco_addQuadra("cep001", 10.0, 20.0, 50.0, 40.0);
    banco_addPessoa("12345678900", "Joao", "Silva", 'M', "01/01/1990");
    banco_morar("12345678900", "cep001", 'N', 10, "Ap101");
    
    Pessoa *p = banco_getPessoa("12345678900");
    TEST_ASSERT_NOT_NULL(p);
    TEST_ASSERT_EQUAL_INT(1, pessoa_eh_morador(p));
    TEST_ASSERT_EQUAL_STRING("cep001", pessoa_get_cep(p));
    TEST_ASSERT_EQUAL_CHAR('N', pessoa_get_face(p));
    TEST_ASSERT_EQUAL_INT(10, pessoa_get_num(p));
    TEST_ASSERT_EQUAL_STRING("Ap101", pessoa_get_compl(p));
    free(p);
    
    banco_close();
    remove("quadras.hf");
    remove("pessoas.hf");
    remove("estado.txt");
}

void test_banco_despejar(void) {
    banco_set_output_dir(".");
    banco_init();
    
    banco_addQuadra("cep001", 10.0, 20.0, 50.0, 40.0);
    banco_addPessoa("12345678900", "Joao", "Silva", 'M', "01/01/1990");
    banco_morar("12345678900", "cep001", 'N', 10, "Ap101");
    banco_despejar("12345678900");
    
    Pessoa *p = banco_getPessoa("12345678900");
    TEST_ASSERT_NOT_NULL(p);
    TEST_ASSERT_EQUAL_INT(0, pessoa_eh_morador(p));
    free(p);
    
    banco_close();
    remove("quadras.hf");
    remove("pessoas.hf");
    remove("estado.txt");
}

void test_banco_mudar(void) {
    banco_set_output_dir(".");
    banco_init();
    
    banco_addQuadra("cep001", 10.0, 20.0, 50.0, 40.0);
    banco_addQuadra("cep002", 100.0, 20.0, 50.0, 40.0);
    banco_addPessoa("12345678900", "Joao", "Silva", 'M', "01/01/1990");
    banco_morar("12345678900", "cep001", 'N', 10, "Ap101");
    banco_mudar("12345678900", "cep002", 'S', 20, "Casa");
    
    Pessoa *p = banco_getPessoa("12345678900");
    TEST_ASSERT_NOT_NULL(p);
    TEST_ASSERT_EQUAL_STRING("cep002", pessoa_get_cep(p));
    TEST_ASSERT_EQUAL_CHAR('S', pessoa_get_face(p));
    TEST_ASSERT_EQUAL_INT(20, pessoa_get_num(p));
    free(p);
    
    banco_close();
    remove("quadras.hf");
    remove("pessoas.hf");
    remove("estado.txt");
}

void test_banco_quadra_residents(void) {
    banco_set_output_dir(".");
    banco_init();
    
    banco_addQuadra("cep001", 10.0, 20.0, 50.0, 40.0);
    banco_addPessoa("12345678900", "Joao", "Silva", 'M', "01/01/1990");
    banco_addPessoa("98765432100", "Maria", "Santos", 'F', "15/05/1985");
    banco_morar("12345678900", "cep001", 'N', 10, "Ap101");
    banco_morar("98765432100", "cep001", 'S', 20, "Casa");
    
    int counts[5];
    banco_getQuadraResidents("cep001", counts);
    
    TEST_ASSERT_EQUAL_INT(2, counts[0]); // Total
    TEST_ASSERT_EQUAL_INT(1, counts[1]); // Norte
    TEST_ASSERT_EQUAL_INT(1, counts[2]); // Sul
    TEST_ASSERT_EQUAL_INT(0, counts[3]); // Leste
    TEST_ASSERT_EQUAL_INT(0, counts[4]); // Oeste
    
    banco_close();
    remove("quadras.hf");
    remove("pessoas.hf");
    remove("estado.txt");
}

// ============================================================
// MAIN
// ============================================================

int main(void) {
    UNITY_BEGIN();
    
    RUN_TEST(test_banco_add_quadra);
    RUN_TEST(test_banco_add_quadra_duplicada);
    RUN_TEST(test_banco_remove_quadra);
    RUN_TEST(test_banco_add_pessoa);
    RUN_TEST(test_banco_morar);
    RUN_TEST(test_banco_despejar);
    RUN_TEST(test_banco_mudar);
    RUN_TEST(test_banco_quadra_residents);
    
    return UNITY_END();
}