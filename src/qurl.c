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
    char response [BUFFER_SIZE];
    unsigned char url_count;
    bool verbosity;
};

const char * argp_program_version = "qurl 1.2.0";
const char * argp_program_bug_address = "<halosghost@archlinux.info>";
static char * doc = "qurl -- a simple program to shorten URLs using qurl.org\v"
                    "URL should include the protocol";

static size_t
write_function (const char *, size_t, size_t, char *);

static CURLcode
shorten_url (const char *, char *, bool);

static error_t
parse_opt (signed, char *, struct argp_state *);

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
    struct args args = { {0}, 0, false };

    argp_parse(&argp, argc, argv, 0, 0, &args);

    if ( argc <= 1 || !args.url_count ) {
        fputs("You must pass at least one URL to shorten\n", stderr);
        return EXIT_FAILURE;
    } return EXIT_SUCCESS;
}

// Function Definitions //
static size_t
write_function (const char * buffer, size_t size, size_t nmemb, char * userp) {

    size_t length = size * nmemb;
    strncat(userp, buffer, length);
    return length;
}

static CURLcode
shorten_url (const char * url, char * response, bool verbosity) {

    CURL * h = curl_easy_init();
    CURLcode res;

    if ( !h ) {
        fputs("Failed to get CURL handle", stderr);
        return CURLE_FAILED_INIT;
    }

    curl_easy_setopt(h, CURLOPT_URL, url);
    curl_easy_setopt(h, CURLOPT_FOLLOWLOCATION, 1);
    curl_easy_setopt(h, CURLOPT_USERAGENT, "curl/7.35.0");
    curl_easy_setopt(h, CURLOPT_WRITEFUNCTION, write_function);
    curl_easy_setopt(h, CURLOPT_WRITEDATA, response);
    curl_easy_setopt(h, CURLOPT_VERBOSE, verbosity);

    if ( (res = curl_easy_perform(h)) != CURLE_OK ) {
        curl_easy_cleanup(h);
        fprintf(stderr, "Error connecting to qurl.org -- %s\n",
                curl_easy_strerror(res));
        return res;
    } else {
        if ( response[3] == 'x' ) { // {"exists" ...
            char lnk_url [26] = {'\0'}; // largest possible is 23 at the moment
            char exists [6] = {'\0'};

            sscanf(response, "%*[^:]:%[^,],%*[^:]:\"%[^\"]", exists, lnk_url);

            char * lnk_ptr = lnk_url;
            char strpd_url [26];
            char * strpd_ptr = strpd_url;

            while ( *lnk_ptr ) {
                if ( *lnk_ptr != '\\' ) {
                    *strpd_ptr = *lnk_ptr;
                    strpd_ptr ++;
                } lnk_ptr ++;
            } *strpd_ptr = '\0';

            printf("Link %s: %s\n",
                   (exists[0] == 't' ? "existed" : "did not exist"), strpd_url);
        } else {
            fprintf(stderr,"Not a valid URL\n");
            return CURLE_URL_MALFORMAT;
        }
    } curl_easy_cleanup(h); return CURLE_OK;
}

static error_t
parse_opt (signed key, char * arg, struct argp_state * state) {

    struct args * args = state->input;
    switch ( key ) {
        case 'v':
            args->verbosity = true;
            break;

        case 'u':
            args->url_count ++;
            unsigned long len = strlen(arg) + 29;
            char * url = malloc(len);
            snprintf(url, len, "http://qurl.org/api/url?url=%s", arg);
            shorten_url(url, args->response, args->verbosity);
            if ( url ) { free(url); }
            break;

        default:
            return ARGP_ERR_UNKNOWN;
    } return 0;
}

// vim: set ts=4 sw=4 et:
