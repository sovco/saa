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
static inline void *saa_arena_push(saa_arena *arena, size_t lenght);
static inline double *saa_arena_push_value_double(saa_arena *arena, double value);
static inline float *saa_arena_push_value_float(saa_arena *arena, float value);
static inline int *saa_arena_push_value_int(saa_arena *arena, int value);
static inline bool *saa_arena_push_value_bool(saa_arena *arena, int value);
static inline char *saa_arena_push_value_string(saa_arena *arena, const char *value);
static inline void *saa_arena_push_arbitrary(saa_arena *arena, void *value, size_t lenght);
static inline void saa_arena_destroy(saa_arena *arena);

#define saa_arena_push_value(arena, type) _Generic((type),     \
                    float: saa_arena_push_value_float,         \
                    double: saa_arena_push_value_double,       \
                    int: saa_arena_push_value_int,             \
                    char *: saa_arena_push_value_string        \
              )(arena, type)

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

static inline saa_arena saa_arena_create(const size_t page_size)
{
    return (saa_arena){ .page_size = page_size, .pages = __saa_allocate_arena_page(page_size) };
}

static inline void *saa_arena_push(saa_arena *arena, size_t lenght)
{
    void *ret_ptr = NULL;
    saa_arena_page *latest_page = NULL;
    for (latest_page = arena->pages; latest_page->next != NULL; latest_page = latest_page->next) {}
    if (latest_page->capacity + lenght > arena->page_size) {
        latest_page->next = __saa_allocate_arena_page(arena->page_size);
        latest_page = latest_page->next;
    }
    ret_ptr = (void *)(latest_page->data + latest_page->capacity);
    latest_page->capacity += lenght;
    return ret_ptr;
}

static inline void *saa_arena_push_arbitrary(saa_arena *arena, void *value, size_t lenght)
{
    void *ptr = saa_arena_push(arena, lenght);
    memcpy(ptr, value, lenght);
    return ptr;
}

static inline double *saa_arena_push_value_double(saa_arena *arena, double value)
{
    return (double *)saa_arena_push_arbitrary(arena, (void *)&value, sizeof(value));
}

static inline float *saa_arena_push_value_float(saa_arena *arena, float value)
{
    return (float *)saa_arena_push_arbitrary(arena, (void *)&value, sizeof(value));
}

static inline int *saa_arena_push_value_int(saa_arena *arena, int value)
{
    return (int *)saa_arena_push_arbitrary(arena, (void *)&value, sizeof(value));
}

static inline bool *saa_arena_push_value_bool(saa_arena *arena, int value)
{
    return (bool *)saa_arena_push_arbitrary(arena, (void *)&value, sizeof(value));
}

static inline char *saa_arena_push_value_string(saa_arena *arena, const char *value)
{
    return (char *)saa_arena_push_arbitrary(arena, (void *)value, strlen(value) + 1);
}

static inline void saa_arena_destroy(saa_arena *arena)
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
