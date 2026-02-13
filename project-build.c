#define NOB_IMPLEMENTATION
#include "build/deps/nob/nob.h"

#define SCLIP_IMPL
#include "project-build.h"

int main(int argc, char **argv)
{
    const int res = sclip_parse(argc, (const char **)argv);
    if (res == SCLIP_PARSE_VERS_OR_HELP) {
        return 0;
    }
    NOB_GO_REBUILD_URSELF(argc, argv);
    Nob_Cmd cmd = { 0 };

    const char *build_type = sclip_opt_debug_get_value() ? "-ggdb" : "-O3";

    nob_cmd_append(&cmd, "mkdir", "-p", "build/deps/stf");
    if (!nob_cmd_run(&cmd)) return 1;
    if (!file_exists("build/deps/stf/stf.h")) {
        nob_cmd_append(&cmd, "curl", "-Lo", "build/deps/stf/stf.h", "https://raw.githubusercontent.com/sovco/stf/refs/heads/master/include/stf/stf.h");
        if (!nob_cmd_run(&cmd)) return 1;
    }
    nob_cmd_append(&cmd, "mkdir", "-p", "build/deps/smb");
    if (!nob_cmd_run(&cmd)) return 1;
    if (!file_exists("build/deps/smb/smb.h")) {
        nob_cmd_append(&cmd, "curl", "-Lo", "build/deps/smb/smb.h", "https://raw.githubusercontent.com/sovco/smb/refs/heads/master/include/smb/smb.h");
        if (!nob_cmd_run(&cmd)) return 1;
    }
    nob_cmd_append(&cmd, "cc", "-Wall", "-Wextra", "-std=c11", build_type, "-D_POSIX_C_SOURCE=200112L", "-o", "build/saa-test", "-lpthread", "-lm", "-Iinclude", "-Ibuild/deps", "test/saa-test.c");
    if (!nob_cmd_run(&cmd)) return 1;
    if (sclip_opt_run_tests_get_value()) {
        nob_cmd_append(&cmd, "./build/saa-test");
        if (!nob_cmd_run(&cmd)) return 1;
    }
    if (sclip_opt_run_valgrind_get_value()) {
        nob_cmd_append(&cmd, "valgrind", "--leak-check=full", "--show-leak-kinds=all", "--track-origins=yes", "./build/saa-test");
        if (!nob_cmd_run(&cmd)) return 1;
    }
    return 0;
}
