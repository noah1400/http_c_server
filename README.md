# http_c_server
 Simple http server with simple routing

## Current Limitations and Known Issues
**HTTP Version Support**: This software currently supports only HTTP/1.1. Compatibility with other versions of the HTTP protocol, such as HTTP/2 or HTTP/1.0, is not guaranteed.

**Request Validation**: Please be aware that HTTP/TCP requests that do not adhere to the expected format may lead to unpredictable behavior. The error handling mechanisms in place may not sufficiently identify or mitigate issues arising from improperly formatted requests.

**Error Checking**: The error detection capabilities are in development, and as such, the system might not accurately recognize all instances of malformed requests. I am trying to actively work to enhance the robustness of the request parsing and validation processes.

**Usage Advisory**: Given the above limitations, I advise users to ensure that input data is well-formed and compliant with HTTP/1.1 specifications to avoid potential issues. I appreciate feedback and bug reports from you to help improve the software.

## Build
```bash
gcc main.c http.c server.c responses.c routing.c -Iincludes -o bin/server -lpthread -Wall -Wextra -pedantic
```

## Run

```bash
sudo ./bin/server
```

Stopping server with `CTRL+C` will automatically free all remaining resources.

## Usage

```c
#include <server.h>
#include <http.h>
#include <routing.h>

// GET /
http_response *handle_index(http_request *req) {
    (void)req; // Unused parameter (req) warning
    return http_response_text(200, "Index <GET>");
}

// GET /hello
http_response *handle_hello(http_request *req) {
    (void)req; // Unused parameter (req) warning
    return http_response_text(200, "Hello, World! <GET>");
}

// POST /hello
http_response *handle_hello_post(http_request *req) {
    (void)req; // Unused parameter (req) warning
    return http_response_text(200, "Hello, World! <POST>");
}

int main() {
    router_t *router = router_create();
    router_get(router, "/", handle_index);
    router_get(router, "/hello", handle_hello);
    router_post(router, "/hello", handle_hello_post);

    init_server(80, router);
    start_server();
    return 0;
}
```