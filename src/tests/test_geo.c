#include "../unity/unity.h"
#include "../geo.h"
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

void test_geo_insert_quadra(void) {
    clean_test_files();
    
    hf_set_output_dir(".");
    hf_set_base_name("teste");
    banco_set_output_dir(".");
    banco_init();
    
    geo_insertQuadra("cep001", 10.0, 20.0, 50.0, 40.0);
    
    Quadra *q = banco_getQuadra("cep001");
    TEST_ASSERT_NOT_NULL(q);
    TEST_ASSERT_EQUAL_STRING("cep001", quadra_get_cep(q));
    free(q);
    
    banco_close();
    clean_test_files();
}

void test_geo_set_quadra_style(void) {
    clean_test_files();
    
    hf_set_output_dir(".");
    hf_set_base_name("teste");
    banco_set_output_dir(".");
    banco_init();
    
    geo_setQuadraStyle("red", "blue", "2.0");
    geo_insertQuadra("cep001", 10.0, 20.0, 50.0, 40.0);
    
    Quadra *q = banco_getQuadra("cep001");
    TEST_ASSERT_NOT_NULL(q);
    TEST_ASSERT_EQUAL_STRING("red", quadra_get_fill(q));
    TEST_ASSERT_EQUAL_STRING("blue", quadra_get_stroke(q));
    free(q);
    
    banco_close();
    clean_test_files();
}

int main(void) {
    UNITY_BEGIN();
    
    RUN_TEST(test_geo_insert_quadra);
    RUN_TEST(test_geo_set_quadra_style);
    
    return UNITY_END();
}
