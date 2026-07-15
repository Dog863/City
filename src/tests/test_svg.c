#include "../unity/unity.h"
#include "../svg.h"
#include <stdio.h>
#include <stdlib.h>

void setUp(void) {}
void tearDown(void) {}

void test_svg_init_close(void) {
    svg_init("teste.svg");
    svg_close();
    remove("teste.svg");
}

void test_svg_rect(void) {
    svg_init("teste.svg");
    svg_rect(10.0, 20.0, 50.0, 40.0, "red");
    svg_close();
    
    FILE *f = fopen("teste.svg", "r");
    TEST_ASSERT_NOT_NULL(f);
    fclose(f);
    remove("teste.svg");
}

void test_svg_text(void) {
    svg_init("teste.svg");
    svg_text(10.0, 20.0, "Teste");
    svg_close();
    
    FILE *f = fopen("teste.svg", "r");
    TEST_ASSERT_NOT_NULL(f);
    fclose(f);
    remove("teste.svg");
}

void test_svg_line(void) {
    svg_init("teste.svg");
    svg_line(10.0, 20.0, 30.0, 40.0, "red");
    svg_close();
    
    FILE *f = fopen("teste.svg", "r");
    TEST_ASSERT_NOT_NULL(f);
    fclose(f);
    remove("teste.svg");
}

void test_svg_circle(void) {
    svg_init("teste.svg");
    svg_circle(50.0, 50.0, 10.0, "blue");
    svg_close();
    
    FILE *f = fopen("teste.svg", "r");
    TEST_ASSERT_NOT_NULL(f);
    fclose(f);
    remove("teste.svg");
}

int main(void) {
    UNITY_BEGIN();
    
    RUN_TEST(test_svg_init_close);
    RUN_TEST(test_svg_rect);
    RUN_TEST(test_svg_text);
    RUN_TEST(test_svg_line);
    RUN_TEST(test_svg_circle);
    
    return UNITY_END();
}
