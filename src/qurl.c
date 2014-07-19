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

// Variables //
#define BUFFER_SIZE 120

char url [BUFFER_SIZE] = {'\0'};

// Usage //
void _usage (void) {
    fputs("Usage: qurl [-h] [-q] [-v] [-u URL]\n\n"
          "Options:\n"
          "  -h, --help\tprint this help and exit\n"
          "  -q, --quiet\tprint less\n"
          "  -u, --url\tcheck status of URL\n"
          "  -v, --verbose\tprint very verbosely\n\n"
          "URL must include the protocol (e.g., http://)\n", stderr);
    exit(0);
}

size_t write_function (const char * buffer, size_t size, size_t nmemb, char * userp) {
    char * string = userp;
    size_t length = size * nmemb;
    strncat(string, buffer, length);
    return length;
}

// Main Function //
int main (int argc, char ** argv) {
    static int flag_help;
    static int flag_verbose;
    static int flag_quiet;

    if ( argc <= 1 ) {
		flag_help = 1;
	} else {   
		int c = 0;
        
        while ( c != -1 ) {
            static struct option options [] = {
                /* Flags */
                { "help",     no_argument,         0, 'h' },
                { "quiet",    no_argument,         0, 'q' },
                { "verbose",  no_argument,         0, 'v' },
                /* Switches */
                { "url",      required_argument,   0, 'u' },
                { 0,          0,                   0, 0   },
            };

            int opt_index = 0;

            c = getopt_long(argc, argv, "hqvu:", options, &opt_index);

            if ( c == -1 ) break;

            switch ( c ) {
                case 'h':
                    flag_help = 1;
                    break;

                case 'q':
                    flag_verbose = 0;
                    flag_quiet = 1;
                    break;
                
                case 'u':
                    snprintf(url, sizeof(url), "http://qurl.org/api/url?url=%s", optarg);
                    break;

                case 'v':
                    flag_quiet = 0;
                    flag_verbose = 1;
                    break;;
            }
        }
    }

    if ( flag_help || !*url ) { _usage(); };

    curl_global_init(CURL_GLOBAL_ALL);
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
            curl_global_cleanup();
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
					printf("Link %s: ", ( lnk_existed[0] == 't' ? "existed" : "did not exist" ));
				} printf("%s\n", strpd_url);
            }
            else {
                fprintf(stderr,"Not a valid URL\n");
                status = 1;
            }
        }
    }
    
    curl_easy_cleanup(handle);
    curl_global_cleanup();

    return status;
}
// vim: set tabstop=4 shiftwidth=4 expandtab:
