/*****************************\
* Upload files to pomf.se     *
* Author: Sam Stuewe (C) 2014 *
* License: GPLv3              *
\*****************************/

// Libraries //
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <argp.h>
#include <stdbool.h>
#include <limits.h>
#include <curl/curl.h>
#include <sys/stat.h>
#include <fcntl.h>

// Forward Declarations //
#define BUFFER_SIZE 256
#define FILE_MAX 52428800

const char * argp_program_version = "pomf 1.1.0";
const char * argp_program_bug_address = "<halosghost@archlinux.info>";
static char * doc = "pomf -- a simple tool to upload files to pomf.se";

/**
 * TODO list:
 * - stdin support
 */

struct args {
    char src [4097];
    bool verbosity;
};

static size_t
write_function (char *, size_t, size_t, char *);

static error_t
parse_opt (signed, char *, struct argp_state *);

// Main Function //
signed
main (signed argc, char * argv []) {

    struct argp_option os [] = {
        { 0,         0,   0,      0, "Options:",        -1 },
        { "file",    'f', "FILE", 0, "upload FILE",     0  },
        { "verbose", 'v', 0,      0, "print much more", 0  },
        { 0,         0,   0,      0, 0,                 0  }
    };

    struct argp argp = { os, parse_opt, 0, doc, 0, 0, 0 };
    struct args as = { { '\0' }, false };
    argp_parse(&argp, argc, argv, 0, 0 , &as);

    struct stat b;
    int fd = open(as.src, O_RDONLY);
    if ( fd == -1 || fstat(fd, &b) || !S_ISREG(b.st_mode) ) {
        fprintf(stderr, "Error opening file at %s\n", as.src);
        close(fd);
        exit(1);
    } else if ( b.st_size > FILE_MAX ) {
        fputs("File too large", stderr);
        close(fd);
        exit(1);
    } close(fd);

    CURL * handle = curl_easy_init();
    CURLcode res;

    if ( !handle ) {
        fputs("Failed to get CURL handle", stderr);
        exit(1);
    }

    struct curl_httppost * post = NULL;
    struct curl_httppost * postend = NULL;

    curl_formadd(&post, &postend,
                 CURLFORM_COPYNAME, "files[]",
                 CURLFORM_FILE, as.src,
                 CURLFORM_END);

    char written_result [BUFFER_SIZE] = { '\0' };

    curl_easy_setopt(handle, CURLOPT_VERBOSE, as.verbosity);
    curl_easy_setopt(handle, CURLOPT_URL, "http://pomf.se/upload.php");
    curl_easy_setopt(handle, CURLOPT_NOPROGRESS, 1L);
    curl_easy_setopt(handle, CURLOPT_FAILONERROR, 1L);
    curl_easy_setopt(handle, CURLOPT_HTTPPOST, post);
    curl_easy_setopt(handle, CURLOPT_MAXREDIRS, 50L);
    curl_easy_setopt(handle, CURLOPT_TCP_KEEPALIVE, 1L);
    curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, write_function);
    curl_easy_setopt(handle, CURLOPT_WRITEDATA, &written_result);

    res = curl_easy_perform(handle);
    curl_easy_cleanup(handle);
    handle = NULL;
    curl_formfree(post);
    post = NULL;

    char url [BUFFER_SIZE];
    sscanf(written_result, "%*[^[][%*[^,],%*[^,],%*[^:]:\"%[^\"]", url);
    printf("http://a.pomf.se/%s\n", url);

    return (signed )res;
}

// Function Definitions //
static size_t
write_function (char * buffer, size_t size, size_t nmemb, char * userp) {

    char * string = userp;
    size_t length = size * nmemb;
    strncat(string, buffer, length);
    return length;
}

static error_t
parse_opt (signed key, char * arg, struct argp_state * state) {

    struct args * a = state->input;
    switch ( key ) {
        case 'f':
            strncpy(a->src, arg, 259);
            break;

        case 'v':
            a->verbosity = true;
            break;

        default:
            return ARGP_ERR_UNKNOWN;
    } return 0;
}

// vim: set ts=4 sw=4 et:
