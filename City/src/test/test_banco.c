#include "../unity/unity.h"
#include "banco.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// Helper para comparar doubles com tolerância
static int double_equal(double a, double b, double tolerance) {
    return fabs(a - b) < tolerance;
}

void setUp(void) {
    banco_init();
}

void tearDown(void) {
    banco_close();
}

// Teste 1: Inserir e buscar quadra (usando inteiros para evitar double)
void test_banco_add_and_get_quadra(void) {
    banco_addQuadra("12345", 10, 20, 100, 50);
    Quadra *q = banco_getQuadra("12345");
    
    TEST_ASSERT_NOT_NULL(q);
    TEST_ASSERT_EQUAL_STRING("12345", quadra_get_cep(q));
    // Usar comparação inteira em vez de double
    TEST_ASSERT_TRUE(double_equal(10.0, quadra_get_x(q), 0.001));
    TEST_ASSERT_TRUE(double_equal(20.0, quadra_get_y(q), 0.001));
    TEST_ASSERT_TRUE(double_equal(100.0, quadra_get_w(q), 0.001));
    TEST_ASSERT_TRUE(double_equal(50.0, quadra_get_h(q), 0.001));
    
    free(q);
}

// Teste 2: Buscar quadra inexistente
void test_banco_get_nonexistent_quadra_returns_null(void) {
    Quadra *q = banco_getQuadra("99999");
    TEST_ASSERT_NULL(q);
}

// Teste 3: Remover quadra
void test_banco_remove_quadra(void) {
    banco_addQuadra("remover", 0, 0, 10, 10);
    Quadra *q = banco_getQuadra("remover");
    TEST_ASSERT_NOT_NULL(q);
    free(q);
    
    banco_removeQuadra("remover");
    q = banco_getQuadra("remover");
    TEST_ASSERT_NULL(q);
}

// Teste 4: Estilo das quadras
void test_banco_quadra_style(void) {
    banco_setQuadraStyle("red", "blue", 2.5);
    banco_addQuadra("style_test", 0, 0, 50, 50);
    
    Quadra *q = banco_getQuadra("style_test");
    TEST_ASSERT_NOT_NULL(q);
    TEST_ASSERT_EQUAL_STRING("red", quadra_get_fill(q));
    TEST_ASSERT_EQUAL_STRING("blue", quadra_get_stroke(q));
    TEST_ASSERT_TRUE(double_equal(2.5, quadra_get_stroke_width(q), 0.001));
    
    free(q);
}

// Teste 5: Inserir e buscar pessoa
void test_banco_add_and_get_pessoa(void) {
    banco_addPessoa("12345678900", "Joao", "Silva", 'M', "01/01/1990");
    Pessoa *p = banco_getPessoa("12345678900");
    
    TEST_ASSERT_NOT_NULL(p);
    TEST_ASSERT_EQUAL_STRING("12345678900", pessoa_get_cpf(p));
    TEST_ASSERT_EQUAL_STRING("Joao", pessoa_get_nome(p));
    TEST_ASSERT_EQUAL_STRING("Silva", pessoa_get_sobrenome(p));
    TEST_ASSERT_EQUAL_CHAR('M', pessoa_get_sexo(p));
    TEST_ASSERT_EQUAL_STRING("01/01/1990", pessoa_get_nasc(p));
    TEST_ASSERT_EQUAL_INT(0, pessoa_eh_morador(p));
    
    free(p);
}

// Teste 6: Pessoa passar a morar
void test_banco_pessoa_morar(void) {
    banco_addQuadra("cep123", 0, 0, 100, 100);
    banco_addPessoa("111", "Maria", "Santos", 'F', "02/02/1995");
    
    banco_morar("111", "cep123", 'N', 25, "Apt 101");
    
    Pessoa *p = banco_getPessoa("111");
    TEST_ASSERT_NOT_NULL(p);
    TEST_ASSERT_EQUAL_INT(1, pessoa_eh_morador(p));
    TEST_ASSERT_EQUAL_STRING("cep123", pessoa_get_cep(p));
    TEST_ASSERT_EQUAL_CHAR('N', pessoa_get_face(p));
    TEST_ASSERT_EQUAL_INT(25, pessoa_get_num(p));
    TEST_ASSERT_EQUAL_STRING("Apt 101", pessoa_get_compl(p));
    
    free(p);
}

// Teste 7: Despejar pessoa
void test_banco_pessoa_despejar(void) {
    banco_addQuadra("cep456", 0, 0, 100, 100);
    banco_addPessoa("222", "Pedro", "Oliveira", 'M', "03/03/1985");
    banco_morar("222", "cep456", 'S', 30, "Casa 2");
    
    // Verificar que mora antes
    Pessoa *p = banco_getPessoa("222");
    TEST_ASSERT_EQUAL_INT(1, pessoa_eh_morador(p));
    free(p);
    
    banco_despejar("222");
    
    p = banco_getPessoa("222");
    TEST_ASSERT_NOT_NULL(p);
    TEST_ASSERT_EQUAL_INT(0, pessoa_eh_morador(p));
    free(p);
}

// Teste 8: Contar moradores por face (CORRIGIDO)
void test_banco_get_quadra_residents(void) {
    banco_addQuadra("cep789", 0, 0, 100, 100);
    banco_addPessoa("333", "Ana", "Costa", 'F', "04/04/2000");
    banco_addPessoa("444", "Carlos", "Lima", 'M', "05/05/1998");
    banco_addPessoa("555", "Beatriz", "Rocha", 'F', "06/06/2001");
    
    banco_morar("333", "cep789", 'N', 10, "");
    banco_morar("444", "cep789", 'S', 20, "");
    banco_morar("555", "cep789", 'L', 30, "");
    
    int counts[5];
    banco_getQuadraResidents("cep789", counts);
    
    // Verificar individualmente para mensagens mais claras
    TEST_ASSERT_EQUAL_INT_MESSAGE(3, counts[0], "Total residents should be 3");
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, counts[1], "North face should have 1");
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, counts[2], "South face should have 1");
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, counts[3], "East face (L) should have 1");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, counts[4], "West face (O) should have 0");
}

// Teste 9: Remover quadra afeta moradores (CORRIGIDO)
void test_banco_remove_quadra_affects_residents(void) {
    banco_addQuadra("cep999", 0, 0, 100, 100);
    banco_addPessoa("666", "Lucas", "Pereira", 'M', "07/07/1992");
    banco_morar("666", "cep999", 'O', 15, "");
    
    // Verificar que mora antes
    Pessoa *p = banco_getPessoa("666");
    TEST_ASSERT_NOT_NULL(p);
    TEST_ASSERT_EQUAL_INT(1, pessoa_eh_morador(p));
    free(p);
    
    // Remover quadra
    banco_removeQuadra("cep999");
    
    // Verificar que agora é sem-teto
    p = banco_getPessoa("666");
    TEST_ASSERT_NOT_NULL(p);
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, pessoa_eh_morador(p), "Resident should become homeless after quadra removal");
    free(p);
}

// Teste 10: Mudar de endereço
void test_banco_pessoa_mudar(void) {
    banco_addQuadra("cep111", 0, 0, 100, 100);
    banco_addQuadra("cep222", 200, 200, 100, 100);
    banco_addPessoa("777", "Fernanda", "Alves", 'F', "08/08/1997");
    
    banco_morar("777", "cep111", 'N', 5, "");
    banco_mudar("777", "cep222", 'S', 10, "Novo endereco");
    
    Pessoa *p = banco_getPessoa("777");
    TEST_ASSERT_NOT_NULL(p);
    TEST_ASSERT_EQUAL_STRING("cep222", pessoa_get_cep(p));
    TEST_ASSERT_EQUAL_CHAR('S', pessoa_get_face(p));
    TEST_ASSERT_EQUAL_INT(10, pessoa_get_num(p));
    
    free(p);
}

// Teste 11: Contagem de pessoas
void test_banco_pessoa_count(void) {
    int initial = banco_getPessoaCount();
    
    banco_addPessoa("888", "Teste1", "Um", 'M', "01/01/2000");
    banco_addPessoa("999", "Teste2", "Dois", 'F', "02/02/2000");
    
    TEST_ASSERT_EQUAL_INT(initial + 2, banco_getPessoaCount());
}

// Teste 12: Buscar por índice
void test_banco_get_pessoa_index(void) {
    banco_addPessoa("aaa", "Indice", "Teste", 'M', "01/01/2000");
    banco_addPessoa("bbb", "Outro", "Teste", 'F', "02/02/2000");
    
    Pessoa *p1 = banco_getPessoaIndex(0);
    Pessoa *p2 = banco_getPessoaIndex(1);
    
    TEST_ASSERT_NOT_NULL(p1);
    TEST_ASSERT_NOT_NULL(p2);
    TEST_ASSERT_EQUAL_STRING("aaa", pessoa_get_cpf(p1));
    TEST_ASSERT_EQUAL_STRING("bbb", pessoa_get_cpf(p2));
}

// Teste 13: Remover pessoa
void test_banco_remove_pessoa(void) {
    banco_addPessoa("remove_me", "Remover", "Teste", 'M', "01/01/2000");
    
    Pessoa *p = banco_getPessoa("remove_me");
    TEST_ASSERT_NOT_NULL(p);
    free(p);
    
    banco_removePessoa("remove_me");
    
    p = banco_getPessoa("remove_me");
    TEST_ASSERT_NULL(p);
}

// Teste 14: Múltiplas quadras
void test_banco_multiple_quadras(void) {
    banco_addQuadra("q1", 0, 0, 10, 10);
    banco_addQuadra("q2", 100, 100, 20, 20);
    banco_addQuadra("q3", 200, 200, 30, 30);
    
    Quadra *q1 = banco_getQuadra("q1");
    Quadra *q2 = banco_getQuadra("q2");
    Quadra *q3 = banco_getQuadra("q3");
    
    TEST_ASSERT_NOT_NULL(q1);
    TEST_ASSERT_NOT_NULL(q2);
    TEST_ASSERT_NOT_NULL(q3);
    
    free(q1);
    free(q2);
    free(q3);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_banco_add_and_get_quadra);
    RUN_TEST(test_banco_get_nonexistent_quadra_returns_null);
    RUN_TEST(test_banco_remove_quadra);
    RUN_TEST(test_banco_quadra_style);
    RUN_TEST(test_banco_add_and_get_pessoa);
    RUN_TEST(test_banco_pessoa_morar);
    RUN_TEST(test_banco_pessoa_despejar);
    RUN_TEST(test_banco_get_quadra_residents);
    RUN_TEST(test_banco_remove_quadra_affects_residents);
    RUN_TEST(test_banco_pessoa_mudar);
    RUN_TEST(test_banco_pessoa_count);
    RUN_TEST(test_banco_get_pessoa_index);
    RUN_TEST(test_banco_remove_pessoa);
    RUN_TEST(test_banco_multiple_quadras);
    return UNITY_END();
}