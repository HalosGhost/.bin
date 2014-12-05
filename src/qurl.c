/*****************************\
* Create shortcut URLs        *
* Author: Sam Stuewe (C) 2014 *
* License: GPLv3              *
\*****************************/

// Includes //
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <curl/curl.h>
#include <argp.h>

// Forward Declarations //
#define BUFFER_SIZE 120

struct args {
    char url [BUFFER_SIZE], response [BUFFER_SIZE];
    bool verbose;
};

const char * argp_program_version = "qurl 1.1.1";
const char * argp_program_bug_address = "<halosghost@archlinux.info>";
static char * doc = "qurl -- a simple program to shorten URLs using qurl.org\v"
                    "URL should include the protocol";

static error_t
parse_opt (int key, char * arg, struct argp_state * state);

static size_t
write_function (const char * buffer, size_t size, size_t nmemb, char * userp);

// Main Function //
signed
main (signed argc, char * argv []) {

    struct argp_option os [] = {
        { 0,         0,   0,     0, "Options:",        -1 },
        { "verbose", 'v', 0,     0, "Print verbosely", 0  },
        { "url",     'u', "URL", 0, "Shorten URL",     0  },
        { 0,         0,   0,     0, 0,                 0  }
    };

    struct argp argp = { os, parse_opt, "", doc, NULL, NULL, 0 };
    struct args args = { {0}, {0}, false };

    argp_parse(&argp, argc, argv, 0, 0, &args);

    if ( !*args.url ) {
        fputs("No URI given\n", stderr);
        exit(2);
    }

    CURL * h = curl_easy_init();
    signed status = 0;

    if ( h ) {
        curl_easy_setopt(h, CURLOPT_URL, args.url);
        curl_easy_setopt(h, CURLOPT_FOLLOWLOCATION, 1);
        curl_easy_setopt(h, CURLOPT_USERAGENT, "curl/7.35.0");
        curl_easy_setopt(h, CURLOPT_WRITEFUNCTION, write_function);
        curl_easy_setopt(h, CURLOPT_WRITEDATA, args.response);
        curl_easy_setopt(h, CURLOPT_VERBOSE, args.verbose);

        if ( curl_easy_perform(h) != CURLE_OK ) {
            curl_easy_cleanup(h);
            fputs("Could not reach qurl.org\n", stderr);
            exit(1);
        } else {
            if ( args.response[3] == 'x' ) { // {"exists" ...
                char lnk_url [26] = {'\0'}; // largest possible is 23 at the moment
                char existed [6] = {'\0'};

                sscanf(args.response, "%*[^:]:%[^,],%*[^:]:\"%[^\"]", existed, lnk_url);

                char * lnk_ptr = lnk_url;
                char strpd_url [26];
                char * strpd_ptr = strpd_url;

                while ( *lnk_ptr ) {
                    if ( *lnk_ptr != '\\' ) {
                        *strpd_ptr = *lnk_ptr;
                        strpd_ptr ++;
                    } lnk_ptr ++;
                }

                *strpd_ptr = '\0';

                printf("Link %s: %s\n", (existed[0] == 't' ? "existed" : "did not exist"),
                       strpd_url);
            } else {
                fprintf(stderr,"Not a valid URL\n");
                status = 1;
            }
        }
    } curl_easy_cleanup(h);
    return status;
}

// Function Definitions //
static error_t
parse_opt (signed key, char * arg, struct argp_state * state) {

    struct args * args = state->input;
    switch ( key ) {
        case 'v':
            args->verbose = true;
            break;

        case 'u':
            snprintf(args->url, sizeof(args->url),
                     "http://qurl.org/api/url?url=%s", arg);
            break;

        default:
            return ARGP_ERR_UNKNOWN;
    } return 0;
}

static size_t
write_function (const char * buffer, size_t size, size_t nmemb, char * userp) {

    size_t length = size * nmemb;
    strncat(userp, buffer, length);
    return length;
}

// vim: set ts=4 sw=4 et:
