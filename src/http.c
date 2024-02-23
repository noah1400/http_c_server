#include <http.h>

void http_free_request(http_request *req)
{
    if (req->_query_string)
    {
        free(req->_query_string);
    }
    if (req->headers)
    {
        hashmap_free(req->headers);
    }
    if (req->body)
    {
        free(req->body);
    }

    if (req->params)
        hashmap_free(req->params);
    free(req);
}

// Assuming http_request is defined as previously described

char *http_request_to_string(http_request *request)
{
    // Calculate the total length needed for the string representation
    size_t totalLength = strlen(request->method) + strlen(request->uri) + strlen(request->version) + 4; // Space for spaces and \r\n
    if (request->_query_string)
    {
        totalLength += strlen(request->_query_string) + 1; // +1 for '?'
    }

    // Allocate memory for the string representation
    char *requestString = (char *)malloc(totalLength + 1); // +1 for null terminator
    if (!requestString)
    {
        return NULL; // Memory allocation failed
    }

    // Start constructing the string
    char *ptr = requestString;
    ptr += sprintf(ptr, "<%s %s%s%s %s>",
                   request->method,
                   request->uri,
                   request->_query_string ? "?" : "",
                   request->_query_string ? request->_query_string : "",
                   request->version);

    return requestString;
}

http_response *http_create_response(int status_code, char *status_message, char *body)
{
    http_response *response = (http_response *)malloc(sizeof(http_response));
    if (!response)
        return NULL; // Failed to allocate memory for response

    strcpy(response->version, "HTTP/1.1");
    response->status_code = status_code;
    strncpy(response->status_message, status_message, sizeof(response->status_message) - 1);
    response->status_message[sizeof(response->status_message) - 1] = '\0'; // Ensure null-termination

    response->body = body ? strdup(body) : NULL;
    response->body_length = body ? strlen(body) : 0;

    response->headers = hashmap_new();

    // Example usage: Add a default Content-Type header
    // This assumes http_response_add_header is properly implemented
    http_response_add_header(response, "Content-Type", "text/plain");
    if (body)
    {
        char contentLength[32];
        sprintf(contentLength, "%zu", response->body_length);
        http_response_add_header(response, "Content-Length", contentLength);
    }

    return response;
}

void http_free_response(http_response *response)
{
    if (response->body)
        free(response->body);
    // Free headers
    hashmap_free(response->headers);
    free(response);
}

void append_header(char *key, char *value, void *responseString)
{
    // cast responseString to char*
    responseString = (char *)responseString;
    strcat(responseString, key);
    strcat(responseString, ": ");
    strcat(responseString, value);
    strcat(responseString, "\r\n");
}

void calculateLength(char *key, char *value, void *count)
{
    int *c = (int *)count;
    *c += strlen(key) + 2 + strlen(value) + 2; // key: value\r\n
}

char *http_response_to_string(http_response *response)
{
    // Estimate initial size
    size_t totalLength = strlen(response->version) + 4 + strlen(response->status_message) + 2 + response->body_length + 2;

    // Calculate headers length

    int count = 0;
    hashmap_iterate(response->headers, calculateLength, &count);
    totalLength += count;

    char *responseString = (char *)malloc(totalLength + 1); // +1 for null terminator
    if (!responseString)
        return NULL;

    sprintf(responseString, "%s %d %s\r\n", response->version, response->status_code, response->status_message);

    hashmap_iterate(response->headers, append_header, responseString);

    strcat(responseString, "\r\n"); // Headers and body separator

    // Append body if exists
    if (response->body && response->body_length > 0)
    {
        strcat(responseString, response->body);
    }

    return responseString;
}

int http_response_add_header(http_response *response, char *key, char *value)
{
    if (!response || !key || !value)
        return -1; // Basic validation
    if (!response->headers)
    {
        response->headers = hashmap_new();
        if (!response->headers)
            return -1; // Failed to create headers map
    }
    return hashmap_put(response->headers, key, value);
}

int http_request_add_header(http_request *req, char *key, char *value)
{
    if (!req || !key || !value)
        return -1; // Basic validation
    if (!req->headers)
    {
        req->headers = hashmap_new();
        if (!req->headers)
            return -1; // Failed to create headers map
    }
    return hashmap_put(req->headers, key, value);
}

char *http_request_get_header_value(http_request *req, char *key)
{
    if (!req || !key)
        return NULL; // Basic validation
    return hashmap_get(req->headers, key);
}

char *http_request_get_param(http_request *req, char *key)
{
    if (!req || !key)
        return NULL; // Basic validation
    return hashmap_get(req->params, key);
}