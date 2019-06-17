/** @file client.cpp
 *  @brief This file contains client part
 *
 *  Now after executation it connects to 
 *  The server using predefined credentials 
 *  And receives the token
 *  @author Suren G.
 */

#include <iostream>
#include <curl/curl.h>

struct mem_struct {
    char *memory;
    size_t size;
};
 
static size_t write_cb(void *contents, size_t size, size_t nmemb, void *userp);
 
int main(void)
{
    CURL *curl;
    CURLcode res;
    
    /* In windows, this will init the winsock stuff */ 
    curl_global_init(CURL_GLOBAL_ALL);
    
    /* get a curl handle */ 
    curl = curl_easy_init();
    if(curl) {
        std::string read_buffer;
        /* First set the URL that is about to receive our POST. This URL can
           just as well be a https:// URL if that is what should receive the
           data. */ 
        curl_easy_setopt(curl, CURLOPT_URL, "127.0.0.1:5555/login");
        /* Now specify the POST data */ 
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, "TEST:PASS");
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &write_cb);

        /* Perform the request, res will get the return code */ 
        res = curl_easy_perform(curl);
        /* Check for errors */ 
        if(res != CURLE_OK)
          fprintf(stderr, "curl_easy_perform() failed: %s\n",
                  curl_easy_strerror(res));
         
        /* always cleanup */ 
        curl_easy_cleanup(curl);
    }
    curl_global_cleanup();
    return 0;
}




static size_t write_cb(void *contents, size_t size, size_t nmemb, void *userp)
{
    size_t realsize = size * nmemb;
    struct mem_struct *mem = (struct mem_struct *)userp;
    
    char *ptr = (char*)realloc(mem->memory, mem->size + realsize + 1);
    if(ptr == NULL) {
        /* out of memory! */ 
        printf("not enough memory (realloc returned NULL)\n");
        return 0;
    }
    
    mem->memory = ptr;
    memcpy(&(mem->memory[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;
    printf("Tokenen: %s \n", mem->memory);
    return realsize;
}
