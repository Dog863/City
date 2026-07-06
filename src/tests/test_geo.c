#include "../unity/unity.h"
#include "../geo.h"
#include "../banco.h"
#include <stdio.h>
#include <stdlib.h>

void setUp(void) {}
void tearDown(void) {}

void test_geo_insert_quadra(void) {
    banco_set_output_dir(".");
    banco_init();
    
    geo_insertQuadra("cep001", 10.0, 20.0, 50.0, 40.0);
    
    Quadra *q = banco_getQuadra("cep001");
    TEST_ASSERT_NOT_NULL(q);
    TEST_ASSERT_EQUAL_STRING("cep001", quadra_get_cep(q));
    free(q);
    
    banco_close();
    remove("quadras.hf");
    remove("pessoas.hf");
    remove("estado.txt");
}

void test_geo_set_quadra_style(void) {
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
    remove("quadras.hf");
    remove("pessoas.hf");
    remove("estado.txt");
}

int main(void) {
    UNITY_BEGIN();
    
    RUN_TEST(test_geo_insert_quadra);
    RUN_TEST(test_geo_set_quadra_style);
    
    return UNITY_END();
}