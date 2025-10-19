# saa
Simple Arena Allocator

# Basic Usage

```c
#define SAA_IMPL
#include <saa/saa.h> 

saa_arena arena = saa_arena_create(100); // page size 100 bytes
double *pushed_a = saa_arena_push_value_double(&arena, 77.7);
char *pushed_b = saa_arena_push_value_string(&arena, "pushing this to arena");
saa_arena_destroy(&arena);
```

# Building Tests

```bash
mkdir -p build/deps/nob
curl -Lo test/deps/nob/nob.h https://raw.githubusercontent.com/tsoding/nob.h/refs/heads/main/nob.h
gcc -o project-build test/project-build.c && ./project-build
```
