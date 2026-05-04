#include "../unity/unity.h"
#include "geo.h"
#include "banco.h"
#include "svg.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

static char txt_file[] = "test_qry.txt";
static char svg_file[] = "test_qry.svg";

char* read_txt_file(void) {
    FILE *f = fopen(txt_file, "r");
    if (!f) return NULL;
    
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);
    
    char *content = (char*)malloc(size + 1);
    if (!content) {
        fclose(f);
        return NULL;
    }
    fread(content, 1, size, f);
    content[size] = '\0';
    fclose(f);
    
    return content;
}

void setUp(void) {
    banco_init();
    remove(txt_file);
    remove(svg_file);
    svg_init(svg_file);
    qry_init(txt_file);
}

void tearDown(void) {
    qry_close();
    svg_close();
    banco_close();
    remove(txt_file);
    remove(svg_file);
}

/* ==================== TESTE h? ==================== */

void test_qry_info_pessoa_morador(void) {
    banco_addQuadra("info_test", 0, 0, 100, 100);
    banco_addPessoa("info_cpf", "Joana", "Silva", 'F', "10/10/1990");
    banco_morar("info_cpf", "info_test", 'S', 15, "Casa");
    
    qry_infoPessoa("info_cpf");
    qry_close();
    qry_init(txt_file);
    
    char *content = read_txt_file();
    TEST_ASSERT_NOT_NULL(content);
    TEST_ASSERT_TRUE(strstr(content, "info_cpf") != NULL);
    TEST_ASSERT_TRUE(strstr(content, "Joana") != NULL);
    TEST_ASSERT_TRUE(strstr(content, "info_test/S/15") != NULL);
    free(content);
}

void test_qry_info_pessoa_sem_teto(void) {
    banco_addPessoa("sem_teto", "Jose", "Ninguem", 'M', "01/01/2000");
    
    qry_infoPessoa("sem_teto");
    qry_close();
    qry_init(txt_file);
    
    char *content = read_txt_file();
    TEST_ASSERT_NOT_NULL(content);
    TEST_ASSERT_TRUE(strstr(content, "sem_teto") != NULL);
    TEST_ASSERT_TRUE(strstr(content, "Sem-teto") != NULL);
    free(content);
}

void test_qry_info_pessoa_inexistente(void) {
    qry_infoPessoa("inexistente");
    qry_close();
    qry_init(txt_file);
    
    char *content = read_txt_file();
    // Pessoa inexistente não deve escrever nada
    TEST_ASSERT_NULL(content);  // Arquivo vazio
    free(content);
}

/* ==================== TESTE pq ==================== */

void test_qry_quadra_count(void) {
    banco_addQuadra("pq_test", 0, 0, 200, 100);
    banco_addPessoa("pq1", "P1", "T", 'M', "01/01/2000");
    banco_addPessoa("pq2", "P2", "T", 'F', "02/02/2000");
    
    banco_morar("pq1", "pq_test", 'N', 10, "");
    banco_morar("pq2", "pq_test", 'S', 20, "");
    
    qry_quadraCount("pq_test");
    qry_close();
    qry_init(txt_file);
    
    char *content = read_txt_file();
    TEST_ASSERT_NOT_NULL(content);
    TEST_ASSERT_TRUE(strstr(content, "Quadra pq_test:") != NULL);
    TEST_ASSERT_TRUE(strstr(content, "Norte: 1") != NULL);
    TEST_ASSERT_TRUE(strstr(content, "Sul: 1") != NULL);
    TEST_ASSERT_TRUE(strstr(content, "Total: 2") != NULL);
    free(content);
}

void test_qry_quadra_count_vazia(void) {
    banco_addQuadra("pq_empty", 0, 0, 100, 100);
    
    qry_quadraCount("pq_empty");
    qry_close();
    qry_init(txt_file);
    
    char *content = read_txt_file();
    TEST_ASSERT_NOT_NULL(content);
    TEST_ASSERT_TRUE(strstr(content, "Quadra pq_empty:") != NULL);
    TEST_ASSERT_TRUE(strstr(content, "Total: 0") != NULL);
    free(content);
}

/* ==================== TESTE censo ==================== */

void test_qry_censo(void) {
    banco_addQuadra("censo_q", 0, 0, 100, 100);
    
    banco_addPessoa("c1", "Homem1", "S", 'M', "01/01/2000");
    banco_addPessoa("c2", "Mulher1", "S", 'F', "02/02/2000");
    banco_addPessoa("c3", "Homem2", "S", 'M', "03/03/2000");
    
    banco_morar("c1", "censo_q", 'N', 1, "");
    banco_morar("c2", "censo_q", 'S', 2, "");
    
    qry_censo();
    qry_close();
    qry_init(txt_file);
    
    char *content = read_txt_file();
    TEST_ASSERT_NOT_NULL(content);
    TEST_ASSERT_TRUE(strstr(content, "Total habitantes: 3") != NULL);
    TEST_ASSERT_TRUE(strstr(content, "Total moradores: 2") != NULL);
    TEST_ASSERT_TRUE(strstr(content, "Total sem-teto: 1") != NULL);
    TEST_ASSERT_TRUE(strstr(content, "Homens: 2") != NULL);
    TEST_ASSERT_TRUE(strstr(content, "Mulheres: 1") != NULL);
    free(content);
}

/* ==================== TESTE mud ==================== */

void test_qry_mudar_pessoa(void) {
    banco_addQuadra("origem", 0, 0, 100, 100);
    banco_addQuadra("destino", 200, 200, 100, 100);
    banco_addPessoa("mud_cpf", "Mudanca", "Teste", 'F', "01/01/2000");
    banco_morar("mud_cpf", "origem", 'N', 5, "");
    
    // Verifica endereço original
    Pessoa *p = banco_getPessoa("mud_cpf");
    TEST_ASSERT_EQUAL_STRING("origem", pessoa_get_cep(p));
    free(p);
    
    qry_mudarPessoa("mud_cpf", "destino", "S", 20, "Novo");
    
    // Verifica novo endereço
    p = banco_getPessoa("mud_cpf");
    TEST_ASSERT_NOT_NULL(p);
    TEST_ASSERT_EQUAL_STRING("destino", pessoa_get_cep(p));
    TEST_ASSERT_EQUAL_CHAR('S', pessoa_get_face(p));
    TEST_ASSERT_EQUAL_INT(20, pessoa_get_num(p));
    free(p);
}

/* ==================== TESTE nasce ==================== */

void test_qry_nasce(void) {
    int before = banco_getPessoaCount();
    
    qry_nasce("new_cpf", "Novo", "Nascido", 'M', "01/01/2020");
    qry_close();
    qry_init(txt_file);
    
    char *content = read_txt_file();
    TEST_ASSERT_NOT_NULL(content);
    TEST_ASSERT_TRUE(strstr(content, "Nasceu: new_cpf Novo") != NULL);
    free(content);
    
    TEST_ASSERT_EQUAL_INT(before + 1, banco_getPessoaCount());
    
    Pessoa *p = banco_getPessoa("new_cpf");
    TEST_ASSERT_NOT_NULL(p);
    TEST_ASSERT_EQUAL_STRING("Novo", pessoa_get_nome(p));
    free(p);
}

/* ==================== TESTE rip ==================== */

void test_qry_rip(void) {
    banco_addPessoa("to_die", "Morto", "Silva", 'M', "01/01/1990");
    
    int before = banco_getPessoaCount();
    
    qry_rip("to_die");
    qry_close();
    qry_init(txt_file);
    
    char *content = read_txt_file();
    TEST_ASSERT_NOT_NULL(content);
    TEST_ASSERT_TRUE(strstr(content, "Faleceu: to_die Morto") != NULL);
    free(content);
    
    TEST_ASSERT_EQUAL_INT(before - 1, banco_getPessoaCount());
    
    Pessoa *p = banco_getPessoa("to_die");
    TEST_ASSERT_NULL(p);
}

void test_qry_rip_com_endereco(void) {
    banco_addQuadra("rip_q", 0, 0, 100, 100);
    banco_addPessoa("to_die_addr", "Morto", "ComEnd", 'F', "01/01/1990");
    banco_morar("to_die_addr", "rip_q", 'N', 5, "");
    
    qry_rip("to_die_addr");
    qry_close();
    qry_init(txt_file);
    
    char *content = read_txt_file();
    TEST_ASSERT_NOT_NULL(content);
    TEST_ASSERT_TRUE(strstr(content, "Faleceu: to_die_addr Morto") != NULL);
    TEST_ASSERT_TRUE(strstr(content, "Endereco: rip_q/N/5") != NULL);
    free(content);
}

/* ==================== TESTE dspj ==================== */

void test_qry_despejar(void) {
    banco_addQuadra("dspj_q", 0, 0, 100, 100);
    banco_addPessoa("to_evict", "Despejado", "Teste", 'M', "01/01/2000");
    banco_morar("to_evict", "dspj_q", 'S', 10, "");
    
    // Verifica que mora
    Pessoa *p = banco_getPessoa("to_evict");
    TEST_ASSERT_EQUAL_INT(1, pessoa_eh_morador(p));
    free(p);
    
    qry_despejar("to_evict");
    qry_close();
    qry_init(txt_file);
    
    char *content = read_txt_file();
    TEST_ASSERT_NOT_NULL(content);
    TEST_ASSERT_TRUE(strstr(content, "Despejado: to_evict Despejado") != NULL);
    TEST_ASSERT_TRUE(strstr(content, "Endereco: dspj_q/S/10") != NULL);
    free(content);
    
    // Verifica que ficou sem-teto
    p = banco_getPessoa("to_evict");
    TEST_ASSERT_NOT_NULL(p);
    TEST_ASSERT_EQUAL_INT(0, pessoa_eh_morador(p));
    free(p);
}

/* ==================== TESTE rq ==================== */

void test_qry_remove_quadra(void) {
    banco_addQuadra("rq_test", 0, 0, 100, 100);
    banco_addPessoa("rq_p1", "Teste", "Um", 'M', "01/01/2000");
    banco_morar("rq_p1", "rq_test", 'N', 10, "");
    
    qry_removeQuadra("rq_test");
    
    Quadra *q = banco_getQuadra("rq_test");
    TEST_ASSERT_NULL(q);
    
    Pessoa *p = banco_getPessoa("rq_p1");
    TEST_ASSERT_NOT_NULL(p);
    TEST_ASSERT_EQUAL_INT(0, pessoa_eh_morador(p));
    free(p);
}

/* ==================== MAIN ==================== */

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_qry_info_pessoa_morador);
    RUN_TEST(test_qry_info_pessoa_sem_teto);
    RUN_TEST(test_qry_info_pessoa_inexistente);
    RUN_TEST(test_qry_quadra_count);
    RUN_TEST(test_qry_quadra_count_vazia);
    RUN_TEST(test_qry_censo);
    RUN_TEST(test_qry_mudar_pessoa);
    RUN_TEST(test_qry_nasce);
    RUN_TEST(test_qry_rip);
    RUN_TEST(test_qry_rip_com_endereco);
    RUN_TEST(test_qry_despejar);
    RUN_TEST(test_qry_remove_quadra);
    return UNITY_END();
}
