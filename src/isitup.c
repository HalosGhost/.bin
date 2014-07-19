/*****************************\
* Check if a website is up    *
* Author: Sam Stuewe (C) 2014 *
* License: GPLv3              *
\*****************************/

// Includes //
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <getopt.h>

// Variables //
#define BUFFER_SIZE 120

char uri [BUFFER_SIZE] = {'\0'};

// Usage //
void _usage (int status) {
    fputs("Usage: isitup [-h] [-q] [-v] [-u URI]\n\n"
          "Options:\n"
          "  -h, --help\tprint this help and exit\n"
          "  -q, --quiet\tprint nothing\n"
          "  -u, --uri\tcheck status of URI\n"
          "  -v, --verbose\tprint very verbosely\n\n"
          "URI should be given as the domain name and TLD only\n\n"
          "exit codes:\tURI appears {0: up, 1: down, 2: invalid}\n",
		  (status == 0 ? stdout : stderr));
    exit(status);
}

size_t write_function (const char * buffer, size_t size, size_t nmemb, char * userp) {
    char * string = userp;
    size_t length = size * nmemb;
    strncat(string, buffer, length);
    return length;
}

// Main Function //
int main (int argc, char ** argv) {
    static int flag_quiet;
    static int flag_verbose;

    if ( argc <= 1 ) { 
		_usage(1);
	} else {
        int c = 0;
        
        while ( c != -1 ) {
            static struct option options [] = {
                /* Flags */
                { "help",     no_argument,         0, 'h' },
                { "quiet",    no_argument,         0, 'q' },
                { "verbose",  no_argument,         0, 'v' },
                /* Switches */
                { "uri",      required_argument,   0, 'u' },
                { 0,          0,                   0, 0   },
            };

            int opt_index = 0;

            c = getopt_long(argc, argv, "hqvu:", options, &opt_index);

            switch ( c ) {
                case 'h':
					_usage(0);
                    break;

                case 'q':
                    flag_quiet = 1;
                    flag_verbose = 0;
                    break;
                
                case 'u':
                    snprintf(uri, sizeof(uri), "http://isitup.org/%s.txt", optarg);
                    break;

                case 'v':
                    flag_verbose = 1;
                    flag_quiet = 0;
                    break;
            }
        }
    }

    if ( !*uri ) _usage(1);

    curl_global_init(CURL_GLOBAL_ALL);
    CURL * handle = curl_easy_init();
    int status = 0;

    if ( handle ) {
        char response [BUFFER_SIZE] = {'\0'};

        curl_easy_setopt(handle, CURLOPT_URL, uri);
        curl_easy_setopt(handle, CURLOPT_FOLLOWLOCATION, 1);
        curl_easy_setopt(handle, CURLOPT_USERAGENT, "curl/7.35.0");
        curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, write_function);
        curl_easy_setopt(handle, CURLOPT_WRITEDATA, response);
        curl_easy_setopt(handle, CURLOPT_VERBOSE, flag_verbose);

        if ( curl_easy_perform(handle) != CURLE_OK ) {
            curl_easy_cleanup(handle);
            curl_global_cleanup();
            fputs("Could not reach isitup.org\n", stderr);
            exit(1);
        } else {
            int port, http_response;
            double response_time;
            char ip_addr [43];
            sscanf(response, "%*[^,], %d, %d, %[^,], %d, %lg", &port, &status, ip_addr, &http_response, &response_time);

            if ( !flag_quiet ) {
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
    curl_global_cleanup();

    return (status-1);
}
// vim: set tabstop=4 shiftwidth=4 expandtab:
