#include "unity.h"
#include "svg.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static char svg_file[] = "test_output.svg";

char* read_svg_file(void) {
    FILE *f = fopen(svg_file, "r");
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
    remove(svg_file);
}

void tearDown(void) {
    remove(svg_file);
}

void test_svg_init_creates_file(void) {
    svg_init(svg_file);
    svg_close();
    
    FILE *f = fopen(svg_file, "r");
    TEST_ASSERT_NOT_NULL(f);
    fclose(f);
}

void test_svg_has_opening_tag(void) {
    svg_init(svg_file);
    svg_close();
    
    char *content = read_svg_file();
    TEST_ASSERT_NOT_NULL(content);
    TEST_ASSERT_TRUE(strstr(content, "<svg") != NULL);
    free(content);
}

void test_svg_rect_draws_correctly(void) {
    svg_init(svg_file);
    svg_rect(10.5, 20.5, 100.0, 50.0, "red");
    svg_close();
    
    char *content = read_svg_file();
    TEST_ASSERT_NOT_NULL(content);
    TEST_ASSERT_TRUE(strstr(content, "rect") != NULL);
    TEST_ASSERT_TRUE(strstr(content, "red") != NULL);
    free(content);
}

void test_svg_text_draws_correctly(void) {
    svg_init(svg_file);
    svg_text(30.0, 40.0, "Hello World");
    svg_close();
    
    char *content = read_svg_file();
    TEST_ASSERT_NOT_NULL(content);
    TEST_ASSERT_TRUE(strstr(content, "text") != NULL);
    TEST_ASSERT_TRUE(strstr(content, "Hello World") != NULL);
    free(content);
}

void test_svg_line_draws_correctly(void) {
    svg_init(svg_file);
    svg_line(0, 0, 100, 100, "black");
    svg_close();
    
    char *content = read_svg_file();
    TEST_ASSERT_NOT_NULL(content);
    TEST_ASSERT_TRUE(strstr(content, "line") != NULL);
    TEST_ASSERT_TRUE(strstr(content, "black") != NULL);
    free(content);
}

void test_svg_circle_draws_correctly(void) {
    svg_init(svg_file);
    svg_circle(50.0, 60.0, 10.0, "blue");
    svg_close();
    
    char *content = read_svg_file();
    TEST_ASSERT_NOT_NULL(content);
    TEST_ASSERT_TRUE(strstr(content, "circle") != NULL);
    TEST_ASSERT_TRUE(strstr(content, "blue") != NULL);
    free(content);
}

void test_svg_multiple_shapes(void) {
    svg_init(svg_file);
    svg_rect(0, 0, 10, 10, "red");
    svg_circle(5, 5, 2, "blue");
    svg_line(0, 0, 10, 10, "green");
    svg_text(2, 8, "Test");
    svg_close();
    
    char *content = read_svg_file();
    TEST_ASSERT_NOT_NULL(content);
    TEST_ASSERT_TRUE(strstr(content, "rect") != NULL);
    TEST_ASSERT_TRUE(strstr(content, "circle") != NULL);
    TEST_ASSERT_TRUE(strstr(content, "line") != NULL);
    TEST_ASSERT_TRUE(strstr(content, "text") != NULL);
    free(content);
}

void test_svg_has_closing_tag(void) {
    svg_init(svg_file);
    svg_rect(0, 0, 10, 10, "red");
    svg_close();
    
    char *content = read_svg_file();
    TEST_ASSERT_NOT_NULL(content);
    TEST_ASSERT_TRUE(strstr(content, "</svg>") != NULL);
    free(content);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_svg_init_creates_file);
    RUN_TEST(test_svg_has_opening_tag);
    RUN_TEST(test_svg_rect_draws_correctly);
    RUN_TEST(test_svg_text_draws_correctly);
    RUN_TEST(test_svg_line_draws_correctly);
    RUN_TEST(test_svg_circle_draws_correctly);
    RUN_TEST(test_svg_multiple_shapes);
    RUN_TEST(test_svg_has_closing_tag);
    return UNITY_END();
}
