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

const char * argp_program_version = "pomf 1.2.1";
const char * argp_program_bug_address = "<halosghost@archlinux.info>";
static char * doc = "pomf -- a simple tool to upload files to pomf.se";

/**
 * TODO list:
 * - stdin support
 */

struct args {
    bool verbosity;
};

static size_t
write_function (char *, size_t, size_t, char *);

static unsigned char
check_file_size (const char *);

static CURLcode
upload_file (const char *, bool);

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
    struct args as = { false };
    argp_parse(&argp, argc, argv, 0, 0 , &as);

    return 0;
}

// Function Definitions //
static size_t
write_function (char * buffer, size_t size, size_t nmemb, char * userp) {

    size_t length = size * nmemb;
    strncat(userp, buffer, length);
    return length;
}

static unsigned char
check_file_size (const char * path) {

    struct stat b;
    int fd = open(path, O_RDONLY);
    if ( fd == -1 || fstat(fd, &b) || !S_ISREG(b.st_mode) ) {
        fprintf(stderr, "Error opening file at %s\n", path);
        close(fd); return 1;
    } else if ( b.st_size > FILE_MAX ) {
        fprintf(stderr, "File at %s too large\n", path);
        close(fd); return 2;
    } close(fd); return 0;
}

static CURLcode
upload_file (const char * path, bool verbosity) {

    CURL * handle = curl_easy_init();
    CURLcode res;

    if ( !handle ) {
        fputs("Failed to get CURL handle", stderr);
        return CURLE_FAILED_INIT;
    }

    struct curl_httppost * post = NULL;
    struct curl_httppost * postend = NULL;

    curl_formadd(&post, &postend,
                 CURLFORM_COPYNAME, "files[]",
                 CURLFORM_FILE, path,
                 CURLFORM_END);

    char written_result [BUFFER_SIZE] = { '\0' };

    curl_easy_setopt(handle, CURLOPT_VERBOSE, verbosity);
    curl_easy_setopt(handle, CURLOPT_URL, "https://pomf.se/upload.php");
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

    if ( res != CURLE_OK ) {
        fprintf(stderr, "Upload failed -- %s\n", curl_easy_strerror(res));
    } else {
        char url [BUFFER_SIZE];
        sscanf(written_result, "%*[^[][%*[^,],%*[^,],%*[^:]:\"%[^\"]", url);
        printf("https://a.pomf.se/%s\n", url);
    } return res;
}

static error_t
parse_opt (signed key, char * arg, struct argp_state * state) {

    struct args * a = state->input;
    switch ( key ) {
        case 'f':
            if ( check_file_size(arg) ) {
                break;
            } else {
                upload_file(arg, a->verbosity);
            } break;

        case 'v':
            a->verbosity = true;
            break;

        default:
            return ARGP_ERR_UNKNOWN;
    } return 0;
}
// vim: set ts=4 sw=4 et:
