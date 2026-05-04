#include "../unity/unity.h"
#include "geo.h"
#include "banco.h"
#include "svg.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

static char svg_file[] = "test_geo.svg";

static int double_equal(double a, double b, double tolerance) {
    return fabs(a - b) < tolerance;
}

void setUp(void) {
    banco_init();
    svg_init(svg_file);
}

void tearDown(void) {
    svg_close();
    banco_close();
    remove(svg_file);
}

void test_geo_insert_quadra(void) {
    geo_insertQuadra("geo123", 10.0, 20.0, 100.0, 50.0);
    
    Quadra *q = banco_getQuadra("geo123");
    TEST_ASSERT_NOT_NULL(q);
    TEST_ASSERT_EQUAL_STRING("geo123", quadra_get_cep(q));
    TEST_ASSERT_TRUE(double_equal(10.0, quadra_get_x(q), 0.001));
    TEST_ASSERT_TRUE(double_equal(20.0, quadra_get_y(q), 0.001));
    TEST_ASSERT_TRUE(double_equal(100.0, quadra_get_w(q), 0.001));
    TEST_ASSERT_TRUE(double_equal(50.0, quadra_get_h(q), 0.001));
    
    free(q);
}

void test_geo_multiple_quadras(void) {
    geo_insertQuadra("q1", 0, 0, 10, 10);
    geo_insertQuadra("q2", 100, 100, 20, 20);
    geo_insertQuadra("q3", 200, 200, 30, 30);
    
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

void test_geo_quadra_style(void) {
    geo_setQuadraStyle("yellow", "green", "3.0px");
    geo_insertQuadra("styled", 50, 50, 80, 40);
    
    Quadra *q = banco_getQuadra("styled");
    TEST_ASSERT_NOT_NULL(q);
    TEST_ASSERT_EQUAL_STRING("yellow", quadra_get_fill(q));
    TEST_ASSERT_EQUAL_STRING("green", quadra_get_stroke(q));
    TEST_ASSERT_EQUAL_STRING("3.0px", quadra_get_stroke_width(q));
    
    free(q);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_geo_insert_quadra);
    RUN_TEST(test_geo_multiple_quadras);
    RUN_TEST(test_geo_quadra_style);
    return UNITY_END();
}
