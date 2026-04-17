#include "unity.h"
#include "svg.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static char svg_file[] = "test_output.svg";

void setUp(void) {
    remove(svg_file);
}

void tearDown(void) {
    svg_close();
    remove(svg_file);
}

char* read_file(const char *filename) {
    FILE *f = fopen(filename, "r");
    if (!f) return NULL;
    
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);
    
    char *content = (char*)malloc(size + 1);
    fread(content, 1, size, f);
    content[size] = '\0';
    fclose(f);
    
    return content;
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
    char *content = read_file(svg_file);
    TEST_ASSERT_NOT_NULL(content);
    TEST_ASSERT_TRUE(strstr(content, "<svg") != NULL);
    free(content);
}

void test_svg_rect_works(void) {
    svg_init(svg_file);
    svg_rect(10, 20, 100, 50, "red");
    svg_close();
    char *content = read_file(svg_file);
    TEST_ASSERT_NOT_NULL(content);
    TEST_ASSERT_TRUE(strstr(content, "rect") != NULL);
    TEST_ASSERT_TRUE(strstr(content, "red") != NULL);
    free(content);
}

void test_svg_text_works(void) {
    svg_init(svg_file);
    svg_text(30, 40, "Hello");
    svg_close();
    char *content = read_file(svg_file);
    TEST_ASSERT_NOT_NULL(content);
    TEST_ASSERT_TRUE(strstr(content, "text") != NULL);
    TEST_ASSERT_TRUE(strstr(content, "Hello") != NULL);
    free(content);
}

void test_svg_line_works(void) {
    svg_init(svg_file);
    svg_line(0, 0, 100, 100, "black");
    svg_close();
    char *content = read_file(svg_file);
    TEST_ASSERT_NOT_NULL(content);
    TEST_ASSERT_TRUE(strstr(content, "line") != NULL);
    TEST_ASSERT_TRUE(strstr(content, "black") != NULL);
    free(content);
}

void test_svg_circle_works(void) {
    svg_init(svg_file);
    svg_circle(50, 60, 10, "blue");
    svg_close();
    char *content = read_file(svg_file);
    TEST_ASSERT_NOT_NULL(content);
    TEST_ASSERT_TRUE(strstr(content, "circle") != NULL);
    TEST_ASSERT_TRUE(strstr(content, "blue") != NULL);
    free(content);
}

void test_svg_multiple_shapes(void) {
    svg_init(svg_file);
    svg_rect(0, 0, 10, 10, "red");
    svg_circle(5, 5, 2, "blue");
    svg_close();
    char *content = read_file(svg_file);
    TEST_ASSERT_NOT_NULL(content);
    TEST_ASSERT_TRUE(strstr(content, "rect") != NULL);
    TEST_ASSERT_TRUE(strstr(content, "circle") != NULL);
    free(content);
}

void test_svg_closing_tag(void) {
    svg_init(svg_file);
    svg_rect(0, 0, 10, 10, "red");
    svg_close();
    char *content = read_file(svg_file);
    TEST_ASSERT_NOT_NULL(content);
    TEST_ASSERT_TRUE(strstr(content, "</svg>") != NULL);
    free(content);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_svg_init_creates_file);
    RUN_TEST(test_svg_has_opening_tag);
    RUN_TEST(test_svg_rect_works);
    RUN_TEST(test_svg_text_works);
    RUN_TEST(test_svg_line_works);
    RUN_TEST(test_svg_circle_works);
    RUN_TEST(test_svg_multiple_shapes);
    RUN_TEST(test_svg_closing_tag);
    return UNITY_END();
}