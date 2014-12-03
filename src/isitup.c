/*****************************\
* Check if a website is up    *
* Author: Sam Stuewe (C) 2014 *
* License: GPLv3              *
\*****************************/

// Includes //
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <curl/curl.h>
#include <argp.h>

// Forward Declarations //
#define BUFFER_SIZE 120

struct args {
    char uri [BUFFER_SIZE], response [BUFFER_SIZE];
    bool quiet: 4, verbose: 4;
};

const char * argp_program_version = "isitup 1.1.1";
const char * argp_program_bug_address = "<halosghost@archlinux.info>";
static char * doc = "isitup -- a simple tool to check if a URI is up\v"
                    "URI should be given as the domain name and TLD only\n"
                    "exit codes: URI appears {0: up, 1: down, 2: invalid}";

static error_t
parse_opt (int32_t key, char * arg, struct argp_state * state);

static size_t
write_function (const char * buffer, size_t size, size_t nmemb, char * userp);

// Main Function //
int32_t
main (int32_t argc, char * argv []) {

    struct argp_option os [] = {
        { 0,         0,   0,     0, "Options:",           -1 },
        { "quiet",   'q', 0,     0, "Silence all output", 0  },
        { "verbose", 'v', 0,     0, "Output everyting",   0  },
        { "uri",     'u', "URI", 0, "Check URI",          0  },
        // When upstream adds support for ipv6, add relevant options
        //{ "ipv4",    '4', 0,     0, "Resolve only ipv4",  0  },
        //{ "ipv6",    '6', 0,     0, "Resolve only ipv6",  0  },
        { 0,         0,   0,     0, 0,                    0  }
    };

    struct argp argp = { os, parse_opt, "", doc, NULL, NULL, 0 };
    struct args args = { {0}, {0}, false, false };

    argp_parse(&argp, argc, argv, 0, 0, &args);

    if ( !*args.uri ) {
        fputs("No URI given\n", stderr);
        exit(2);
    }

    CURL * handle = curl_easy_init();
    uint8_t status = 0;

    if ( handle ) {
        curl_easy_setopt(handle, CURLOPT_URL, args.uri);
        curl_easy_setopt(handle, CURLOPT_FOLLOWLOCATION, 1);
        curl_easy_setopt(handle, CURLOPT_USERAGENT, "curl/7.35.0");
        curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, write_function);
        curl_easy_setopt(handle, CURLOPT_WRITEDATA, args.response);
        curl_easy_setopt(handle, CURLOPT_VERBOSE, args.verbose);

        if ( curl_easy_perform(handle) != CURLE_OK ) {
            curl_easy_cleanup(handle);
            fputs("Could not reach isitup.org\n", stderr);
            exit(1);
        } else {
            int32_t port, http_response;
            double response_time;
            char ip_addr [43];
            sscanf(args.response, "%*[^,], %d, %hhu, %[^,], %d, %lg", 
                   &port, &status, ip_addr, &http_response, &response_time);

            if ( !args.quiet ) {
                printf("%s:%d ", ip_addr, port);
                if ( status > 1 ) { 
                    printf("appears down\n"); 
                } else { 
                    printf("(%d after %lgs)\n", http_response, response_time); 
                }
            }
        }
    }
    
    curl_easy_cleanup(handle);
    return (status-1);
}

// Function Definitions //
static error_t
parse_opt (int32_t key, char * arg, struct argp_state * state) {

    struct args * args = state->input;
    switch ( key ) {
        case 'q':
            args->quiet   = true;
            args->verbose = false;
            break;

        case 'v':
            args->quiet   = false;
            args->verbose = true;
            break;

        case 'u':
            snprintf(args->uri, sizeof(args->uri), 
                     "http://isitup.org/%s.txt", arg);
            break;

        default:
            return ARGP_ERR_UNKNOWN;
    } return 0;
}

static size_t 
write_function (const char * buffer, size_t size, size_t nmemb, char * userp) {

    char * string = userp;
    size_t length = size * nmemb;
    strncat(string, buffer, length);
    return length;
}

// vim: set tabstop=4 shiftwidth=4 expandtab:
