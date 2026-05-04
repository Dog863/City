#include "../unity/unity.h"
#include "pessoa.h"
#include "banco.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void setUp(void) {
    banco_init();
}

void tearDown(void) {
    banco_close();
}

// Teste 1: Inserir pessoa
void test_pessoa_insert(void) {
    pessoa_insert("123", "Joao", "Silva", 'M', "01/01/1990");
    
    Pessoa *p = banco_getPessoa("123");
    TEST_ASSERT_NOT_NULL(p);
    TEST_ASSERT_EQUAL_STRING("123", pessoa_get_cpf(p));
    TEST_ASSERT_EQUAL_STRING("Joao", pessoa_get_nome(p));
    TEST_ASSERT_EQUAL_STRING("Silva", pessoa_get_sobrenome(p));
    TEST_ASSERT_EQUAL_CHAR('M', pessoa_get_sexo(p));
    TEST_ASSERT_EQUAL_STRING("01/01/1990", pessoa_get_nasc(p));
    
    free(p);
}

// Teste 2: Pessoa morar
void test_pessoa_morar(void) {
    banco_addQuadra("cep_test", 0, 0, 100, 100);
    pessoa_insert("456", "Maria", "Santos", 'F', "02/02/1995");
    pessoa_morar("456", "cep_test", "N", 25, "Apt 1");
    
    Pessoa *p = banco_getPessoa("456");
    TEST_ASSERT_NOT_NULL(p);
    TEST_ASSERT_EQUAL_INT(1, pessoa_eh_morador(p));
    TEST_ASSERT_EQUAL_STRING("cep_test", pessoa_get_cep(p));
    TEST_ASSERT_EQUAL_CHAR('N', pessoa_get_face(p));
    TEST_ASSERT_EQUAL_INT(25, pessoa_get_num(p));
    
    free(p);
}

// Teste 3: Múltiplas pessoas
void test_pessoa_multiple_inserts(void) {
    pessoa_insert("p1", "Um", "Um", 'M', "01/01/2000");
    pessoa_insert("p2", "Dois", "Dois", 'F', "02/02/2000");
    pessoa_insert("p3", "Tres", "Tres", 'M', "03/03/2000");
    
    TEST_ASSERT_EQUAL_INT(3, banco_getPessoaCount());
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_pessoa_insert);
    RUN_TEST(test_pessoa_morar);
    RUN_TEST(test_pessoa_multiple_inserts);
    return UNITY_END();
}