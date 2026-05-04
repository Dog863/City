#include "unity.h"
#include "qry.h"
#include "banco.h"
#include "svg.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char txt_file[] = "test_output.txt";
static char svg_file[] = "test_output.svg";

// Função auxiliar para ler o conteúdo do ficheiro gerado pelo qry
char* get_txt_content(void) {
    // É importante fechar o sistema qry antes de ler se não usar setvbuf
    qry_close(); 
    
    FILE *f = fopen(txt_file, "r");
    if (!f) return NULL;
    
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    rewind(f);
    
    char *content = malloc(size + 1);
    if (content) {
        size_t read_bytes = fread(content, 1, size, f);
        content[read_bytes] = '\0';
    }
    fclose(f);
    
    // Reabre para o próximo teste se necessário
    qry_init(txt_file);
    return content;
}

void setUp(void) {
    banco_init();
    svg_init(svg_file);
    qry_init(txt_file);
}

void tearDown(void) {
    qry_close();
    svg_close();
    banco_close();
}

// --- Testes das Funções ---

void test_qry_nasce(void) {
    qry_nasce("123.456", "Jose", "Silva", 'M', "01/01/2000");
    
    char *res = get_txt_content();
    TEST_ASSERT_NOT_NULL(res);
    TEST_ASSERT_TRUE(strstr(res, "Nasceu: 123.456 Jose") != NULL);
    
    Pessoa *p = banco_getPessoa("123.456");
    TEST_ASSERT_NOT_NULL(p);
    free(res);
}

void test_qry_infoPessoa_morador(void) {
    banco_addQuadra("Q100", 10, 10, 50, 50);
    banco_addPessoa("111", "Maria", "Oliveira", 'F', "10/10/1990");
    banco_morar("111", "Q100", 'N', 5, "Fundo");
    
    qry_infoPessoa("111");
    
    char *res = get_txt_content();
    TEST_ASSERT_TRUE(strstr(res, "111 Maria Oliveira") != NULL);
    TEST_ASSERT_TRUE(strstr(res, "Mora em Q100/N/5") != NULL);
    free(res);
}

void test_qry_mudarPessoa(void) {
    banco_addQuadra("Q_ORIGEM", 0, 0, 100, 100);
    banco_addQuadra("Q_DESTINO", 200, 200, 100, 100);
    banco_addPessoa("999", "Mudar", "Teste", 'M', "01/01/1980");
    banco_morar("999", "Q_ORIGEM", 'S', 10, "A");
    
    qry_mudarPessoa("999", "Q_DESTINO", "N", 20, "B");
    
    Pessoa *p = banco_getPessoa("999");
    TEST_ASSERT_EQUAL_STRING("Q_DESTINO", pessoa_get_cep(p));
    TEST_ASSERT_EQUAL_INT(20, pessoa_get_num(p));
}

void test_qry_removeQuadra(void) {
    banco_addQuadra("Q_REMOVE", 50, 50, 20, 20);
    banco_addPessoa("p_rem", "Joao", "Rem", 'M', "2000");
    banco_morar("p_rem", "Q_REMOVE", 'L', 5, "");
    
    qry_removeQuadra("Q_REMOVE");
    
    char *res = get_txt_content();
    TEST_ASSERT_TRUE(strstr(res, "p_rem Joao") != NULL);
    TEST_ASSERT_NULL(banco_getQuadra("Q_REMOVE"));
    free(res);
}

void test_qry_censo(void) {
    banco_addPessoa("p1", "H1", "S", 'M', "x");
    banco_addPessoa("p2", "M1", "S", 'F', "y");
    banco_addQuadra("qc", 0, 0, 10, 10);
    banco_morar("p1", "qc", 'N', 1, "");
    
    qry_censo();
    
    char *res = get_txt_content();
    TEST_ASSERT_TRUE(strstr(res, "Total habitantes: 2") != NULL);
    TEST_ASSERT_TRUE(strstr(res, "Total moradores: 1") != NULL);
    free(res);
}

void test_qry_rip(void) {
    banco_addPessoa("rip_me", "Defunto", "Silva", 'M', "1950");
    qry_rip("rip_me");
    
    char *res = get_txt_content();
    TEST_ASSERT_TRUE(strstr(res, "Faleceu: rip_me Defunto") != NULL);
    TEST_ASSERT_NULL(banco_getPessoa("rip_me"));
    free(res);
}

void test_qry_despejar(void) {
    banco_addQuadra("qd", 0, 0, 10, 10);
    banco_addPessoa("out", "Despejado", "Silva", 'M', "1990");
    banco_morar("out", "qd", 'S', 2, "");
    
    qry_despejar("out");
    
    char *res = get_txt_content();
    TEST_ASSERT_TRUE(strstr(res, "Despejado: out Despejado") != NULL);
    Pessoa *p = banco_getPessoa("out");
    TEST_ASSERT_FALSE(pessoa_eh_morador(p));
    free(res);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_qry_nasce);
    RUN_TEST(test_qry_infoPessoa_morador);
    RUN_TEST(test_qry_mudarPessoa);
    RUN_TEST(test_qry_removeQuadra);
    RUN_TEST(test_qry_censo);
    RUN_TEST(test_qry_rip);
    RUN_TEST(test_qry_despejar);
    return UNITY_END();
}