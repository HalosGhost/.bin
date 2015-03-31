/********************************************************\
* A small utility to query games installed through Steam *
* Sam Stuewe (C) 2014 Licensed under the terms of        *
* the GNU Public License version 2                       *
\********************************************************/

#include <stdbool.h> // bool
#include <stdint.h>  // int32_t, uint32_t
#include <argp.h>    // struct argp_state, struct argp, argp_parse()
#include <glob.h>    // glob_t, glob(), globfree(), gl_pathc, gl_pathv
#include <stdlib.h>  // getenv(), EXIT_FAILURE, EXIT_SUCCESS
#include <string.h>  // strlen(), strncpy(), strncat(), strerror()
#include <errno.h>   // errno
#include <unistd.h>  // chdir()
#include <stdio.h>   // fprintf(), fopen(), fscanf(), printf(), fclose()

const char * argp_program_version = "steamlib 1.0";
const char * argp_program_bug_address = "<halosghost@archlinux.info>";
static char * doc = "steamlib -- a tool to interact with a steam library";

struct args {
    char * path, * title, * appid;
    uint64_t launch: 32, print: 32; // bools
};

struct game {
    char name [80]; // longest game name I could find is 78ish chars
    uint32_t id;
};

bool
identify_acfs (glob_t *, const char *);

static error_t
parse_opt (int32_t, char *, struct argp_state *);

/**
 * TODO:
 * Add {,reverse} lookup functionality
 * Add launch functionality
 * Align and (correctly) sort output
 */

int32_t
main (int32_t argc, char * argv []) {

    const struct argp_option os [] = {
        { 0,         0,   0,       0, "Options:",                        -1 },
        { "launch",  'l', 0,       0, "Run the looked-up game",           0 },
        { "lookup",  'r', "TITLE", 0, "Return appID of game with TITLE",  0 },
        { "reverse", 'R', "APPID", 0, "Return title of game with APPID",  0 },
        { "path",    'p', "PATH",  0, "Look for .acf files in PATH",      0 },
        { "print",   'P', 0,       0, "Print whole library",              0 },
        { 0,         0,   0,       0, 0,                                  0 }
    };

    struct argp argp = { os, parse_opt, "", doc, 0, 0, 0 };
    struct args as   = { 0, 0, 0, false, !(argc - 1) };
    argp_parse(&argp, argc, argv, 0, 0, &as);

    glob_t glb;
    if ( !identify_acfs(&glb, as.path) ) {
        return EXIT_FAILURE;
    }

    struct game library [glb.gl_pathc];

    FILE * f;
    for ( size_t i = 0; i < glb.gl_pathc; i ++ ) {
        f = fopen(glb.gl_pathv[i], "r");
        fscanf(f, "%*[^d]d\"%*[^\"]\"%u%*[^m]me\"%*[^\"]\"%[^\"]",
               &library[i].id, library[i].name);
        fclose(f);
    }

    if ( as.print ) {
        for ( size_t i = 0; i < glb.gl_pathc; i ++ ) {
            printf("%u: %s\n", library[i].id, library[i].name);
        }
    }

    globfree(&glb);
    if ( as.path ) {
        free(as.path);
    } return EXIT_SUCCESS;
}

bool
identify_acfs (glob_t * glb, const char * path) {

    int32_t r, errsv;

    if ( path ) {
        const char * p = path;
        errno = 0;
        r = chdir(p);
        errsv = errno;
    } else {
        char * p = getenv("HOME");
        if ( !p ) {
            fputs("Could not locate home folder!\n", stderr);
            return false;
        }

        size_t n = strlen(p) + 31;
        char steamdir [n + 1];
        strncpy(steamdir, p, n);
        strncat(steamdir, "/.local/share/Steam/steamapps/",
                n - strlen(steamdir));
        errno = 0;
        r = chdir(steamdir);
        errsv = errno;
    }

    if ( r ) {
        fprintf(stderr, "Error: %s\n", strerror(errsv));
        return false;
    }

    r = glob("./*.acf", 0, NULL, glb);
    if ( r ) {
        fputs("Globbing failed!\n", stderr);
        return false;
    } return true;
}

static error_t
parse_opt (int32_t key, char * arg, struct argp_state * state) {

    struct args * a = state->input;
    switch ( key ) {
        case 'l': case 'r': case 'R':
            fprintf(stderr, "%c: Not Yet Implemented\n", key);
            break;

        case 'p': {
            size_t n = strlen(arg) + 1;
            if ( a->path ) { free(a->path); }
            a->path = malloc(n);
            strncpy(a->path, arg, n);
            break;
        }

        case 'P':
            a->print = true;
            break;

        default:
            return ARGP_ERR_UNKNOWN;
    } return 0;
}
// vim: set ts=4 sw=4 et:

