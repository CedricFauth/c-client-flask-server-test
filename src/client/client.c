#include <curl/curl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char domain[] = "https://api.fritz.box/";

int main(int argc, char *argv[]){

    if(argc != 2) return 1;
    unsigned url_len = strlen(domain) + strlen(argv[1]);
    char *url;

    if(!(url = malloc(url_len + 1))) return 1;
    strcpy(url,domain);
    strcat(url,argv[1]);
    url[url_len] = '\0';

    printf("\n\rrequest: \t%s\n\rresponse: \t", url);
    
    curl_global_init(CURL_GLOBAL_ALL);
    
    CURL *curl = curl_easy_init();
    if(curl) {
        
        CURLcode res;
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_CAINFO, "/home/progfix/myApiCA.pem");

        res = curl_easy_perform(curl);
        if(res != CURLE_OK)
            fprintf(stderr, "error: %s\n", curl_easy_strerror(res));
        
        curl_easy_cleanup(curl);
    }
    curl_global_cleanup();
    free(url);

    return 0;
}
