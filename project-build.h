#ifndef SCLIP_H
#define SCLIP_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <string.h>
#include <limits.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>

typedef enum {
    SCLIP_STRING,
    SCLIP_LONG,
    SCLIP_DOUBLE,
    SCLIP_BOOL,
    SCLIP_STDIN,
} sclip_option_type;

typedef enum {
    SCLIP_PARSE_VERS_OR_HELP = -3,
    SCLIP_PARSE_NO_ARGS = -2,
    SCLIP_PARSE_MANDATORY_ARG_NOT_PROVIDED = -1,
    SCLIP_PARSE_ALL_OK = 0,
} sclip_parse_ret;

typedef union {
    long numeric;
    double real;
    const char *string;
} sclip_value;

typedef struct
{
    const void *const data;
    const size_t lenght;
} sclip_stdin_content;

typedef struct
{
    const char *long_opt;
    const char *short_opt;
    const sclip_option_type type;
    sclip_value value;
    const bool optional;
} sclip_option;

#define SCLIP_HELP_STR \
"Usage:\n"\
"project-build [options]\n"\
"    -d, --debug                <bool>      Enable debug \n"\
"    -T, --run-tests            <bool>      Runs tests after build. \n"\
"    -V, --run-tests-w-valgrind <bool>      Runs tests after build. \n"\
"    -h, --help                 <bool>      Shows help menu \n"\
"    -v, --version              <bool>      Shows version string \n"\
""

#define SCLIP_VERSION_STR "project-build 0.0.1\n"

typedef enum {
    SCLIP_OPTION_DEBUG_ID,
    SCLIP_OPTION_RUN_TESTS_ID,
    SCLIP_OPTION_RUN_VALGRIND_ID,
    SCLIP_OPTION_HELP_ID,
    SCLIP_OPTION_VERSION_ID
} sclip_option_id;

static sclip_option SCLIP_OPTIONS[] = {
    [SCLIP_OPTION_DEBUG_ID] = { .long_opt = "--debug", .short_opt = "-d", .type = SCLIP_BOOL, .optional = true, .value = { .numeric = LONG_MIN } },
    [SCLIP_OPTION_RUN_TESTS_ID] = { .long_opt = "--run-tests", .short_opt = "-T", .type = SCLIP_BOOL, .optional = true, .value = { .numeric = LONG_MIN } },
    [SCLIP_OPTION_RUN_VALGRIND_ID] = { .long_opt = "--run-tests-w-valgrind", .short_opt = "-V", .type = SCLIP_BOOL, .optional = true, .value = { .numeric = LONG_MIN } },
    [SCLIP_OPTION_HELP_ID] = { .long_opt = "--help", .short_opt = "-h", .type = SCLIP_BOOL, .optional = true, .value = { .string = SCLIP_HELP_STR } },
    [SCLIP_OPTION_VERSION_ID] = { .long_opt = "--version", .short_opt = "-v", .type = SCLIP_BOOL, .optional = true, .value = { .string = SCLIP_VERSION_STR } }
};

#define sclip_parse(argc, argv) \
    __sclip_parse(argc, argv, &SCLIP_OPTIONS[0])
static inline sclip_parse_ret __sclip_parse(int argc, const char **argv, sclip_option *restrict options);
static inline void sclip_print_help();
static inline bool sclip_opt_matches(const char *arg, sclip_option *restrict option);
static inline sclip_value sclip_opt_parse_long(const char *arg);
static inline sclip_value sclip_opt_parse_double(const char *arg);
static inline double sclip_opt_get_value_double(const sclip_option *restrict options, const sclip_option_id id);
static inline long sclip_opt_get_value_long(const sclip_option *restrict options, const sclip_option_id id);
static inline bool sclip_opt_get_value_bool(const sclip_option *restrict options, const sclip_option_id id);
static inline const char *sclip_opt_get_value_string(const sclip_option *restrict options, const sclip_option_id id);
static inline bool sclip_opt_is_provided(const sclip_option *restrict options, const sclip_option_id id);
static inline bool sclip_is_stdin_available();
static inline sclip_stdin_content sclip_get_stdin_contents();
static inline void sclip_free_stdin_content(sclip_stdin_content *restrict const content);

#ifdef __cplusplus
}// extern "C"
#endif

#define sclip_opt_debug_is_provided() \
    sclip_opt_is_provided(&SCLIP_OPTIONS[0], SCLIP_OPTION_DEBUG_ID)
#define sclip_opt_run_tests_is_provided() \
    sclip_opt_is_provided(&SCLIP_OPTIONS[0], SCLIP_OPTION_RUN_TESTS_ID)
#define sclip_opt_run_valgrind_is_provided() \
    sclip_opt_is_provided(&SCLIP_OPTIONS[0], SCLIP_OPTION_RUN_VALGRIND_ID)

#define sclip_opt_debug_get_value() \
    sclip_opt_get_value_bool(&SCLIP_OPTIONS[0], SCLIP_OPTION_DEBUG_ID)
#define sclip_opt_run_tests_get_value() \
    sclip_opt_get_value_bool(&SCLIP_OPTIONS[0], SCLIP_OPTION_RUN_TESTS_ID)
#define sclip_opt_run_valgrind_get_value() \
    sclip_opt_get_value_bool(&SCLIP_OPTIONS[0], SCLIP_OPTION_RUN_VALGRIND_ID)

#ifdef SCLIP_IMPL

#ifdef __cplusplus
extern "C" {
#endif

#include <assert.h>

static inline double sclip_opt_get_value_double(const sclip_option *restrict options, const sclip_option_id id)
{
    return options[id].value.real;
}

static inline long sclip_opt_get_value_long(const sclip_option *restrict options, const sclip_option_id id)
{
    return options[id].value.numeric;
}

static inline bool sclip_opt_get_value_bool(const sclip_option *restrict options, const sclip_option_id id)
{
    return options[id].value.numeric == 1;
}

static inline const char *sclip_opt_get_value_string(const sclip_option *restrict options, const sclip_option_id id)
{
    return options[id].value.string;
}

static inline bool sclip_opt_is_provided(const sclip_option *restrict options, const sclip_option_id id)
{
    return options[id].value.numeric != LONG_MIN;
}

static inline bool sclip_opt_matches(const char *arg, sclip_option *restrict option)
{
    assert(arg != NULL);
    if (arg[0] != '-') return false;
    if (option->short_opt != NULL && strcmp(arg, option->short_opt) == 0)
        return true;
    else if (option->long_opt != NULL && strcmp(arg, option->long_opt) == 0)
        return true;
    return false;
}

static inline sclip_value sclip_opt_parse_long(const char *arg)
{
    assert(arg != NULL);
    static const int base = 10;
    char *end_ptr = NULL;
    const long ret = strtol(arg, &end_ptr, base);
    if (end_ptr == arg) return (sclip_value){ .numeric = LONG_MIN };
    return (sclip_value){ .numeric = ret };
}

static inline sclip_value sclip_opt_parse_double(const char *arg)
{
    assert(arg != NULL);
    char *end_ptr = NULL;
    const double ret = strtod(arg, &end_ptr);
    if (end_ptr == arg) return (sclip_value){ .numeric = LONG_MIN };
    return (sclip_value){ .real = ret };
}

static inline sclip_parse_ret  __sclip_parse(int argc, const char **argv, sclip_option *restrict options)
{
    if(argc == 1) {
        return SCLIP_PARSE_NO_ARGS;
    }
    for (register int j = SCLIP_OPTION_VERSION_ID; j >= 0; j--) {
        for (register int i = 1; i < argc; i++) {
            if (!sclip_opt_matches(argv[i], &options[j])) continue;
            switch (options[j].type) {
            case SCLIP_STRING: {
                options[j].value = (sclip_value){ .string = argv[i + 1] };
            } break;
            case SCLIP_LONG: {
                options[j].value = sclip_opt_parse_long(argv[i + 1]);
            } break;
            case SCLIP_DOUBLE: {
                options[j].value = sclip_opt_parse_double(argv[i + 1]);
            } break;
            case SCLIP_BOOL: {
                if (j == SCLIP_OPTION_VERSION_ID || j == SCLIP_OPTION_HELP_ID) {
                   fputs(options[j].value.string, stdout);
                   return SCLIP_PARSE_VERS_OR_HELP;
                }
                options[j].value = (sclip_value){ .numeric = 1 };
            } break;
            default: {
                options[j].value = (sclip_value){ .numeric = LONG_MIN };
            } break;
            }
        }
        if (!options[j].optional && options[j].value.numeric == LONG_MIN) {
            fprintf(stderr, "Mandatory option/value %s, %s was not provided\nRefer to --help, -h\n", options[j].long_opt, options[j].short_opt);
            return SCLIP_PARSE_MANDATORY_ARG_NOT_PROVIDED;
        }
    }
    return SCLIP_PARSE_ALL_OK;
}

static inline void sclip_print_help()
{
    fputs(SCLIP_HELP_STR, stdout);
}

static inline bool sclip_is_stdin_available()
{
    return !isatty(STDIN_FILENO);
}

static inline sclip_stdin_content sclip_get_stdin_contents()
{
    static const size_t default_size = 4096;
    char *data = NULL;
    char buffer[default_size];
    size_t maximum_size = default_size;
    size_t total_bytes_read = 0;
    size_t bytes_read = 0;

    if ((data = malloc(default_size)) == NULL) {
        return (sclip_stdin_content){ .data = NULL, .lenght = 0 };
    }

    while ((bytes_read = fread(buffer, 1, default_size, stdin)) > 0) {
        if ((total_bytes_read + bytes_read) > maximum_size) {
            maximum_size = 2 * maximum_size;
            if ((data = realloc(data, maximum_size)) == NULL) {
                return (sclip_stdin_content){ .data = NULL, .lenght = 0 };
            }
        }
        memcpy(data + total_bytes_read, buffer, bytes_read);
        total_bytes_read += bytes_read;
    }
    return (sclip_stdin_content){ .data = data, .lenght = total_bytes_read };
}

static inline void sclip_free_stdin_content(sclip_stdin_content *restrict const content)
{
    free((void *)content->data);
}

#ifdef __cplusplus
}// extern "C"
#endif

#endif
#endif
