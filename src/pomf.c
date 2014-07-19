/*****************************\
* Upload files to pomf.se     *
* Author: Sam Stuewe (C) 2014 *
* License: GPLv3              *
\*****************************/

// Libraries //
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <curl/curl.h>

// TODO: Add getopt support for progress bars/verbosity
// TODO: Add json parsing and curl WRITE_FUNCTION for correctly 
//       returning the URL

// Forward Declarations //
#define BUFFER_SIZE 256
void
usage (int status) {
    fputs("Usage: pomf <file>\n",
          (status == 0 ? stdout : stderr));
    exit(status);
}

struct json_write_result {
    char * data;
    int position;
};

// Function drawn from Petri Lehtinen's GitHub Jansson example
static 
size_t 
write_data_buffer (void * ptr, size_t size, size_t nmemb, void * stream) {

    struct json_write_result * result = (struct json_write_result * )stream;
    if ( result->position + size * nmemb >= BUFFER_SIZE - 1 ) {
        fprintf(stderr, "Write Buffer is too small\n");
        return 0;
    }

    memcpy(result->data + result->position, ptr, size * nmemb);
    result->position += size * nmemb;

    return size * nmemb;
}

// Main Function //
int
main (int argc, char * argv []) {
    if ( argc <= 1 ) { 
        usage(1);
    } else if ( argv[1][0] == '-' ) {
        if ( argv[1][1] == 'h' || argv[1][2] == 'h' ) {
            usage(0);
        }
    }

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
                 CURLFORM_FILE, argv[1],
                 CURLFORM_END);

    char * data = calloc(1, BUFFER_SIZE);
    struct json_write_result written_result;
    written_result.data = data;
    written_result.position = 0;

    curl_easy_setopt(handle, CURLOPT_URL, "http://pomf.se/upload.php");
    curl_easy_setopt(handle, CURLOPT_NOPROGRESS, 1L);  
    curl_easy_setopt(handle, CURLOPT_FAILONERROR, 1L);
    curl_easy_setopt(handle, CURLOPT_HTTPPOST, post);
    curl_easy_setopt(handle, CURLOPT_MAXREDIRS, 50L);
    curl_easy_setopt(handle, CURLOPT_TCP_KEEPALIVE, 1L);
    curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, write_data_buffer);
    curl_easy_setopt(handle, CURLOPT_WRITEDATA, &written_result);

    res = curl_easy_perform(handle);
    curl_easy_cleanup(handle);
    handle = NULL;
    curl_formfree(post);
    post = NULL;

    char url [BUFFER_SIZE];
    sscanf(written_result.data, "%*[^[][%*[^,],%*[^,],%*[^:]:\"%[^\"]", url);
    printf("http://a.pomf.se/%s\n", url);

    if ( written_result.data ) { 
        free(written_result.data); 
    }

    return (int )res;
}

// vim: set ts=4 sw=4 et:
