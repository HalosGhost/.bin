/*****************************\
* Create shortcut URLs        *
* Author: Sam Stuewe (C) 2014 *
* License: GPLv3              *
\*****************************/

// Includes //
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <getopt.h>

// Forward Declarations //
#define BUFFER_SIZE 120
static char url [BUFFER_SIZE] = {'\0'};

static void
_usage (int status) __attribute__((noreturn));

static size_t
write_function (const char * buffer, size_t size, size_t nmemb, char * userp);

// Usage //
// Main Function //
int 
main (int argc, char * argv []) {

    static int flag_verbose;
    static int flag_quiet;

    if ( argc <= 1 ) {
        _usage(1);
    } else {   
        static struct option os [] = {
            /* Flags */
            { "help",     no_argument,         0, 'h' },
            { "quiet",    no_argument,         0, 'q' },
            { "verbose",  no_argument,         0, 'v' },
            /* Switches */
            { "url",      required_argument,   0, 'u' },
            { 0,          0,                   0, 0   },
        };

        for ( int c = 0, i = 0; c != -1; 
              c = getopt_long(argc, argv, "hqvu:", os, &i) ) {
            switch ( c ) {
                case 'h':
                    _usage(0);

                case 'q':
                    flag_verbose = 0;
                    flag_quiet = 1;
                    break;
                
                case 'u':
                    snprintf(url, sizeof(url), 
                             "http://qurl.org/api/url?url=%s", optarg);
                    break;

                case 'v':
                    flag_quiet = 0;
                    flag_verbose = 1;
                    break;
            }
        }
    }

    if ( !*url ) { _usage(1); };

    CURL * handle = curl_easy_init();
    int status = 0;

    if ( handle ) {
        char response [BUFFER_SIZE] = {'\0'};

        curl_easy_setopt(handle, CURLOPT_URL, url);
        curl_easy_setopt(handle, CURLOPT_FOLLOWLOCATION, 1);
        curl_easy_setopt(handle, CURLOPT_USERAGENT, "curl/7.35.0");
        curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, write_function);
        curl_easy_setopt(handle, CURLOPT_WRITEDATA, response);
        curl_easy_setopt(handle, CURLOPT_VERBOSE, flag_verbose);

        if ( curl_easy_perform(handle) != CURLE_OK ) {
            curl_easy_cleanup(handle);
            fputs("Could not reach qurl.org\n", stderr);
            exit(1);
        } else {
            if ( response[3] == 'x' ) { // {"exists" ...
                char lnk_url [26] = {'\0'}; // largest possible is 23 at the moment
                char lnk_existed [6] = {'\0'};

                sscanf(response, "%*[^:]:%[^,],%*[^:]:\"%[^\"]", lnk_existed, lnk_url);

                char * lnk_ptr = lnk_url;
                char strpd_url [26];
                char * strpd_ptr = strpd_url;

                while ( *lnk_ptr ) {
                    if ( *lnk_ptr != '\\' ) {
                        *strpd_ptr = *lnk_ptr;
                        strpd_ptr ++;
                    } 

                    lnk_ptr ++;
                }

                *strpd_ptr = '\0';

                if ( !flag_quiet ) {
                    printf("Link %s: ", (lnk_existed[0] == 't' ? "existed" : "did not exist"));
                } printf("%s\n", strpd_url);
            }
            else {
                fprintf(stderr,"Not a valid URL\n");
                status = 1;
            }
        }
    }
    
    curl_easy_cleanup(handle);

    return status;
}

// Function Definitions //
static void 
_usage (int status) {

    fputs("Usage: qurl [-h] [-q] [-v] [-u URL]\n\n"
          "Options:\n"
          "  -h, --help\tprint this help and exit\n"
          "  -q, --quiet\tprint less\n"
          "  -u, --url\tcheck status of URL\n"
          "  -v, --verbose\tprint very verbosely\n\n"
          "URL must include the protocol (e.g., http://)\n"
          ,(status ? stderr : stdout));
    exit(status);
}

static size_t 
write_function (const char * buffer, size_t size, size_t nmemb, char * userp) {

    char * string = userp;
    size_t length = size * nmemb;
    strncat(string, buffer, length);
    return length;
}

// vim: set ts=4 sw=4 et:
