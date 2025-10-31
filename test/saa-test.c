#include <stdlib.h>
#include <string.h>
#include <stf/stf.h>

#define SMB_IMPL
#include <smb/smb.h>

#define SAA_IMPL
#include <saa/saa.h>

STF_TEST_CASE(saa, create_arena_destroy_arena)
{
    static const size_t arena_page_size = 100;
    saa_arena arena = saa_arena_create(arena_page_size);
    STF_EXPECT(arena.pages != NULL, .failure_msg = "pages are NULL");
    saa_arena_destroy(&arena);
}

STF_TEST_CASE(saa, create_arena_push_some_bytes_destroy)
{
    static const size_t arena_page_size = 100;
    static const size_t push_size = 50;
    saa_arena arena = saa_arena_create(arena_page_size);
    STF_EXPECT(arena.pages != NULL, .failure_msg = "pages are NULL");
    char *pushed = saa_arena_push(&arena, push_size);
    STF_EXPECT(pushed != NULL, .failure_msg = "pushing did not return a valid pointer");
    saa_arena_destroy(&arena);
}

STF_TEST_CASE(saa, create_arena_push_some_bytes_write_destroy)
{
    static const size_t arena_page_size = 100;
    static const size_t push_size = sizeof(double);
    static const double value = 77.7;
    saa_arena arena = saa_arena_create(arena_page_size);
    STF_EXPECT(arena.pages != NULL, .failure_msg = "pages are NULL");
    double *pushed = saa_arena_push(&arena, push_size);
    STF_EXPECT(pushed != NULL, .failure_msg = "pushing did not return a valid pointer");
    memcpy(pushed, &value, sizeof(double));
    STF_EXPECT(*pushed == value, .failure_msg = "written value did not match");
    saa_arena_destroy(&arena);
}

STF_TEST_CASE(saa, arena_pushing_more_than_page_size)
{
    static const size_t arena_page_size = 100;
    static const size_t push_size = arena_page_size + 10;
    saa_arena arena = saa_arena_create(arena_page_size);
    char *pushed = saa_arena_push(&arena, push_size);
    STF_EXPECT(pushed == NULL, .failure_msg = "pushing was not supposed to return a valid pointer on a push with size greater than page size");
    saa_arena_destroy(&arena);
}

STF_TEST_CASE(saa, arena_pushing_100_bytes_on_100_byte_page_does_not_create_another_page)
{
    static const size_t arena_page_size = 100;
    static const size_t push_size = 50;
    saa_arena arena = saa_arena_create(arena_page_size);
    STF_EXPECT(arena.pages != NULL, .failure_msg = "pages are NULL");
    char *pushed = saa_arena_push(&arena, push_size);
    STF_EXPECT(pushed != NULL, .failure_msg = "pushing did not return a valid pointer");
    pushed = NULL;
    pushed = saa_arena_push(&arena, push_size);
    STF_EXPECT(pushed != NULL, .failure_msg = "pushing did not return a valid pointer");
    STF_EXPECT(arena.pages->next == NULL, .failure_msg = "pushing 100 bytes was not supposed to create a new page");
    saa_arena_destroy(&arena);
}

STF_TEST_CASE(saa, arena_pushing_2_doubles)
{
    static const size_t arena_page_size = sizeof(double) * 2;
    static const double push_value_a = 77.7;
    static const double push_value_b = 777.7;
    saa_arena arena = saa_arena_create(arena_page_size);
    STF_EXPECT(arena.pages != NULL, .failure_msg = "pages are NULL");
    double *pushed_a = saa_arena_push_value_double(&arena, push_value_a);
    STF_EXPECT(pushed_a != NULL, .failure_msg = "pushed_a is NULL");
    STF_EXPECT(*pushed_a == push_value_a, .failure_msg = "pushed_a value did not match");
    double *pushed_b = saa_arena_push_value_double(&arena, push_value_b);
    STF_EXPECT(pushed_b != NULL, .failure_msg = "pushed_b is NULL");
    STF_EXPECT(*pushed_b == push_value_b, .failure_msg = "pushed_b value did not match");
    STF_EXPECT(arena.pages->next == NULL, .failure_msg = "pushing two doubles was not suposed to create another page");
    saa_arena_destroy(&arena);
}

STF_TEST_CASE(saa, arena_pushing_string)
{
    static const size_t arena_page_size = 100;
    static const char *push_string = "yo, this be a string";
    saa_arena arena = saa_arena_create(arena_page_size);
    STF_EXPECT(arena.pages != NULL, .failure_msg = "pages are NULL");
    char *pushed = saa_arena_push_value_string(&arena, push_string);
    STF_EXPECT(strcmp(pushed, push_string) == 0, .failure_msg = "string don't match");
    saa_arena_destroy(&arena);
}

STF_TEST_CASE(saa, arena_pushing_150_bytes_on_100_byte_page_does_create_another_page)
{
    static const size_t arena_page_size = 100;
    static const size_t push_size = 50;
    saa_arena arena = saa_arena_create(arena_page_size);
    STF_EXPECT(arena.pages != NULL, .failure_msg = "pages are NULL");
    char *pushed = saa_arena_push(&arena, push_size);
    STF_EXPECT(pushed != NULL, .failure_msg = "pushing did not return a valid pointer");
    pushed = NULL;
    pushed = saa_arena_push(&arena, push_size);
    STF_EXPECT(pushed != NULL, .failure_msg = "pushing did not return a valid pointer");
    STF_EXPECT(arena.pages->next == NULL, .failure_msg = "second page is not NULL");
    pushed = NULL;
    pushed = saa_arena_push(&arena, push_size);
    STF_EXPECT(pushed != NULL, .failure_msg = "pushing did not return a valid pointer");
    STF_EXPECT(arena.pages->next != NULL, .failure_msg = "second page is NULL");
    saa_arena_destroy(&arena);
}

STF_TEST_CASE(saa, arena_pushing_with_macro)
{
    static const size_t arena_page_size = 100;
    static const float pushed_float = 77.7;
    saa_arena arena = saa_arena_create(arena_page_size);
    float *pushed = saa_arena_push_value(&arena, pushed_float);
    STF_EXPECT(*pushed == pushed_float, .failure_msg = "values did not match");
    char *pushed_string = saa_arena_push_value(&arena, "this was pushed");
    STF_EXPECT(strcmp(pushed_string, "this was pushed") == 0, .failure_msg = "values did not match");
    saa_arena_destroy(&arena);
}

STF_TEST_CASE(saa, arena_pushing_many_strings)
{
    static const size_t arena_page_size = 100;
    const char *some_text = "something";
    saa_arena arena = saa_arena_create(arena_page_size);
    char *pushed_string = saa_arena_push_value_strings(&arena, "this", " is many ", "strings ", some_text);
    STF_EXPECT(strcmp(pushed_string, "this is many strings something") == 0, .failure_msg = "values did not match");
    saa_arena_destroy(&arena);
}

STF_TEST_CASE(saa, arena_pushing_too_many_string)
{
    static const size_t arena_page_size = 10;
    const char *some_text = "something";
    saa_arena arena = saa_arena_create(arena_page_size);
    char *pushed_string = saa_arena_push_value_strings(&arena, "this", " is many ", "strings ", some_text);
    STF_EXPECT(pushed_string == NULL, .failure_msg = "on pushing string with summary lenght > page size was supposed to return an invalid pointer");
    saa_arena_destroy(&arena);
}

STF_TEST_CASE(saa, arena_pushing_one_string)
{
    static const size_t arena_page_size = 10;
    saa_arena arena = saa_arena_create(arena_page_size);
    char *pushed_string = saa_arena_push_value_strings(&arena, "this");
    STF_EXPECT(strcmp(pushed_string, "this") == 0, .failure_msg = "values did not match");
    saa_arena_destroy(&arena);
}

STF_TEST_CASE(saa, arena_pushing_two_string)
{
    static const size_t arena_page_size = 50;
    saa_arena arena = saa_arena_create(arena_page_size);
    char *pushed_string = saa_arena_push_value_strings(&arena, "this ", "and this");
    STF_EXPECT(strcmp(pushed_string, "this and this") == 0, .failure_msg = "values did not match");
    saa_arena_destroy(&arena);
}

STF_TEST_CASE(saa, arena_pushing_three_string)
{
    static const size_t arena_page_size = 50;
    saa_arena arena = saa_arena_create(arena_page_size);
    char *pushed_string = saa_arena_push_value_strings(&arena, "\"", "gabagoool", "\"");
    STF_EXPECT(strcmp(pushed_string, "\"gabagoool\"") == 0, .failure_msg = "values did not match");
    saa_arena_destroy(&arena);
}


STF_TEST_CASE(saa, benchmark_push)
{
    static const size_t arena_page_size = 200;
    saa_arena arena = saa_arena_create(arena_page_size);
    SMB_BENCHMARK_BEGIN(saa, construct_saa)
    (void)saa_arena_push(&arena, 50);
    SMB_BENCHMARK_END;
    saa_arena_destroy(&arena);
    STF_EXPECT(true, .failure_msg = "you will never see this");
}

STF_TEST_CASE(saa, benchmark_push_strings)
{
    static const size_t arena_page_size = 200;
    saa_arena arena = saa_arena_create(arena_page_size);
    SMB_BENCHMARK_BEGIN(saa, push_strings)
    (void)saa_arena_push_value_strings(&arena, "\"", "gabagoool", "\"", "this", "one");
    SMB_BENCHMARK_END;
    saa_arena_destroy(&arena);
    STF_EXPECT(true, .failure_msg = "you will never see this");
}

int main(void)
{
    return STF_RUN_TESTS();
}
