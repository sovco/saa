#ifndef SAA_H
#define SAA_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

typedef struct saa_arena_page_t saa_arena_page;
typedef struct saa_arena_t saa_arena;

struct saa_arena_page_t
{
    char *data;
    saa_arena_page *next;
    size_t capacity;
};

struct saa_arena_t
{
    saa_arena_page *pages;
    const size_t page_size;
};

static inline saa_arena saa_arena_create(const size_t size);
static inline void *saa_arena_push(const saa_arena *restrict arena, size_t lenght);
static inline bool saa_arena_last_page_able_fit_lenght(const saa_arena *restrict arena, size_t lenght);
static inline double *saa_arena_push_value_double(const saa_arena *restrict arena, double value);
static inline float *saa_arena_push_value_float(const saa_arena *restrict arena, float value);
static inline int *saa_arena_push_value_int(const saa_arena *restrict, int value);
static inline bool *saa_arena_push_value_bool(const saa_arena *restrict arena, int value);
static inline char *saa_arena_push_value_string(const saa_arena *restrict arena, const char *restrict value);
static inline void *saa_arena_push_arbitrary(const saa_arena *restrict arena, const void *restrict value, size_t lenght);
static inline void saa_arena_destroy(const saa_arena *arena);

#define saa_arena_push_value_strings(arena, ...) \
    __saa_arena_push_value_strings(arena, (const char *[]){ __VA_ARGS__, NULL })
static inline char *__saa_arena_push_value_strings(const saa_arena *restrict arena, const char **value);

#define saa_arena_push_value(arena, type) _Generic((type), \
    float: saa_arena_push_value_float,                     \
    double: saa_arena_push_value_double,                   \
    int: saa_arena_push_value_int,                         \
    char *: saa_arena_push_value_string,                   \
    char **: __saa_arena_push_value_strings)(arena, type)

#ifdef __cplusplus
}// extern "C"
#endif

#ifdef SAA_IMPL

#ifdef __cplusplus
extern "C" {
#endif

#include <string.h>

static inline saa_arena_page *__saa_allocate_arena_page(const size_t page_size)
{
    saa_arena_page *ret = malloc(sizeof(*ret));
    *ret = (saa_arena_page){ .data = (char *)malloc(sizeof(char) * page_size), .capacity = 0, .next = NULL };
    return ret;
}

static inline void __saa_arena_add_page(const saa_arena *restrict arena)
{
    saa_arena_page *latest_page = NULL;
    for (latest_page = arena->pages; latest_page->next != NULL; latest_page = latest_page->next) {}
    latest_page->next = __saa_allocate_arena_page(arena->page_size);
}

static inline saa_arena saa_arena_create(const size_t page_size)
{
    return (saa_arena){ .page_size = page_size, .pages = __saa_allocate_arena_page(page_size) };
}

static inline void *saa_arena_push(const saa_arena *restrict arena, size_t lenght)
{
    void *ret_ptr = NULL;
    saa_arena_page *latest_page = NULL;
    if (lenght > arena->page_size) return NULL;
    for (latest_page = arena->pages; latest_page->next != NULL; latest_page = latest_page->next) {}
    if (latest_page->capacity + lenght > arena->page_size) {
        latest_page->next = __saa_allocate_arena_page(arena->page_size);
        latest_page = latest_page->next;
    }
    ret_ptr = (void *)(latest_page->data + latest_page->capacity);
    latest_page->capacity += lenght;
    return ret_ptr;
}

static inline bool saa_arena_last_page_able_fit_lenght(const saa_arena *restrict arena, size_t lenght)
{
    saa_arena_page *latest_page = NULL;
    for (latest_page = arena->pages; latest_page->next != NULL; latest_page = latest_page->next) {}
    if (latest_page->capacity + lenght > arena->page_size) {
        return false;
    }
    return true;
}

static inline void *saa_arena_push_arbitrary(const saa_arena *restrict arena, const void *restrict value, size_t lenght)
{
    void *ptr = saa_arena_push(arena, lenght);
    memcpy(ptr, value, lenght);
    return ptr;
}

static inline double *saa_arena_push_value_double(const saa_arena *restrict arena, double value)
{
    return (double *)saa_arena_push_arbitrary(arena, (void *)&value, sizeof(value));
}

static inline float *saa_arena_push_value_float(const saa_arena *restrict arena, float value)
{
    return (float *)saa_arena_push_arbitrary(arena, (void *)&value, sizeof(value));
}

static inline int *saa_arena_push_value_int(const saa_arena *restrict arena, int value)
{
    return (int *)saa_arena_push_arbitrary(arena, (void *)&value, sizeof(value));
}

static inline bool *saa_arena_push_value_bool(const saa_arena *restrict arena, int value)
{
    return (bool *)saa_arena_push_arbitrary(arena, (void *)&value, sizeof(value));
}

static inline char *saa_arena_push_value_string(const saa_arena *restrict arena, const char *restrict value)
{
    return (char *)saa_arena_push_arbitrary(arena, (void *)value, strlen(value) + 1);
}

static inline size_t __saa_sum_up_string_lenght(const char **strings)
{
    int i = 0;
    size_t summary_size = 0;
    for (i = 0; strings[i + 1] != NULL; i++) {
        summary_size += strlen(strings[i]);
    }
    summary_size += strlen(strings[i]) + 1;
    return summary_size;
}

static inline char *__saa_arena_push_value_strings(const saa_arena *restrict arena, const char **value)
{
    int index = 0;
    size_t summary_size = 0;
    char *ret = NULL;
    char *tmp = NULL;
    if (value == NULL || value[0] == NULL) {
        return NULL;
    } else if (value[1] == NULL) {
        return saa_arena_push_value_string(arena, value[0]);
    }
    summary_size = __saa_sum_up_string_lenght(value);
    if ((ret = saa_arena_push(arena, summary_size)) == NULL) {
        return NULL;
    }
    tmp = ret;
    for (index = 0; value[index + 1] != NULL; index++) {
        memcpy(tmp, value[index], strlen(value[index]));
        tmp += strlen(value[index]);
    }
    memcpy(tmp, value[index], strlen(value[index]) + 1);
    return ret;
}

static inline void saa_arena_destroy(const saa_arena *arena)
{
    saa_arena_page *page = arena->pages;
    while (page != NULL) {
        saa_arena_page *tmp = page->next;
        free(page->data);
        free(page);
        page = tmp;
    }
}

#ifdef __cplusplus
}// extern "C"
#endif

#endif// SAA_IMPL
#endif// SAA_H
