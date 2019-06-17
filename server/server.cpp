/** @file server.cpp
 *  @brief This file contains http streaming server implementation
 *
 *  This contains the http server,
 *  Now it contains only http server requests implementation
 *
 *  @author Suren G.
 */

#include <memory>
#include <cstdint>
#include <iostream>
#include <evhttp.h>

#define USERNAME "TEST"
#define PASSWORD "PASS"
#define DUMMY_TOKEN "7yn9160zQZuVjtdiGnUH4VGanAFgNyNn" // Dummy token, now hard coded, but should be generated each time connection estableshed or session expired.

/** @brief Calls when there is generic http request
 *  @param req http request
 *  @return Void.
 */
void gen_req_cb(evhttp_request *req, void *);

/** @brief Calls when there is login request
 *  @param req http request
 *  @return Void.
 */
void login_req_cb(evhttp_request *req, void *);

/** @brief Calls when there is frame request
 *  @param req http request
 *  @return Void.
 */
void frames_req_cb(evhttp_request *req, void *);

/** @brief Prints buffer as row data
 *  @param str Title of printed data
 *  @param buffer Data which should be printed
 *  @param len Length of the buffer
 *  @param type of data
 *  @return Void.
 */
void print_bytes(char *str, void *buffer, size_t len, char *type);

char *generate_token(char *str);
bool verify_token(evhttp_request *req);

char *srv_address = "127.0.0.1"; // Server's address
uint16_t srv_port = 5555; // Server's port

int main()
{
    if (!event_init())
    {
        fprintf(stderr, "Failed to init libevent.");
        return -1;
    }
    std::unique_ptr<evhttp, decltype(&evhttp_free)> server(evhttp_start(srv_address, srv_port), &evhttp_free);
    if (!server)
    {
        fprintf(stderr, "Failed to init http server.");
        return -1;
    }
  
    evhttp_set_gencb(server.get(), &gen_req_cb, nullptr);
    evhttp_set_cb(server.get(), "/login", &login_req_cb, NULL);
    evhttp_set_cb(server.get(), "/frames", &frames_req_cb, NULL);
    if (event_dispatch() == -1)
    {
        std::cerr << "Failed to run messahe loop." << std::endl;
        return -1;
    }
    return 0;
}

void gen_req_cb(evhttp_request *req, void *)
{
    auto *out_buf = evhttp_request_get_output_buffer(req);
    if (!out_buf)
        return;
    evbuffer_add_printf(out_buf, "<html><body><center><h1>Hello World!</h1></center></body></html>");
    evhttp_send_reply(req, HTTP_OK, "", out_buf);
};

void login_req_cb(evhttp_request *req, void *)
{
    struct evbuffer *input_buf = evhttp_request_get_input_buffer(req);
    if (!input_buf)
    {
        evhttp_send_reply(req, 401, "", NULL);
        return;
    }
    
    struct evbuffer *out_buf = evhttp_request_get_output_buffer(req); 
    size_t input_len = evbuffer_get_length(evhttp_request_get_input_buffer(req)); 
    char data[input_len + 1];
    data[input_len] = 0;

    // copy POST body into your char array
    evbuffer_copyout(input_buf, data, input_len);
    
    char *username = strtok(data, ":");
    char *password = strtok(NULL, "\0"); // Split string from last find delimiter, strtok remembers last string

    if (strcmp(username, USERNAME) == 0 && strcmp(password, PASSWORD) == 0)
    {
        evbuffer_add_printf(out_buf, DUMMY_TOKEN);
        evhttp_send_reply(req, HTTP_OK, "", NULL);
    } else 
    {
        evbuffer_add_printf(out_buf, "<html><body><center><h1>Failed!</h1></center></body></html>");
        evhttp_send_reply(req, 401, "", out_buf);
    }
};

void frames_req_cb(evhttp_request *req, void *)
{
    struct evbuffer *input_buf = evhttp_request_get_input_buffer(req);
    if (!input_buf)
    {
        evhttp_send_reply(req, 401, "", NULL);
        return;
    }
    if (!verify_token(req))
    {
        evhttp_send_reply(req, 401, "", NULL);
        return;
    }
    auto *out_buf = evhttp_request_get_output_buffer(req);
    if (!out_buf)
        return;
    evbuffer_add_printf(out_buf, "<html><body><center><h1>Login!</h1></center></body></html>");
    evhttp_send_reply(req, HTTP_OK, "", out_buf);
};

bool verify_token(evhttp_request *req) 
{
    struct evbuffer *out_buf = evhttp_request_get_output_buffer(req); 
    size_t input_len = evbuffer_get_length(evhttp_request_get_input_buffer(req)); 
    char data[input_len + 1];
    data[input_len] = 0;
    if(strcmp(data, DUMMY_TOKEN) != 0)
        return false;
    return true;
}

void print_bytes(char *str, void *buffer, size_t len, char *type)
{
    printf("%s: ", str);
    if(strcmp(type, "str") == 0)
    {
        char *char_data = (char*)buffer;
        for(int i = 0; i < len; ++i)
        {
            printf("%s", char_data + i);
        }
    }
    if(strcmp(type, "hex") == 0)
    {
        for(int i = 0; i < len; ++i)
        {
            int *int_data = (int*)buffer;
            printf("%02x ", int_data + i);
        }
    }
    printf("\n");
}
