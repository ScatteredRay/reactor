// Copyright (c) 2013, Nicholas "Indy" Ray. All rights reserved.
// See the LICENSE file for usage, modification, and distribution terms.

#include "edit_server.h"
#ifdef WITH_EDIT_SERVER

#include "reporting.h"

#include "webby/webby.h"
#include <string.h>
#include <assert.h>

#ifdef _WIN32
#include <winsock2.h>
#endif //_WIN32

struct EditServer
{
    void* memory;
    WebbyServer* server;
};

static int edit_srv_dispatch(struct WebbyConnection *connection)
{
    //connection->request.uri
    const int num_headers = 1;
    WebbyHeader headers[num_headers];
    headers[0].name = "Access-Control-Allow-Origin";
    headers[0].value = "*";

    WebbyBeginResponse(connection, 200, 14, headers, num_headers);
    WebbyWrite(connection, "Hello, world!\n", 14);
    WebbyEndResponse(connection);
    return 0;
}

static void edit_srv_log(const char* text)
{
    log(LOG_INFO, text);
}

EditServer* InitEditServer()
{

    // TODO: This should be somewhere more general.
#ifdef _WIN32
    WORD wsa_version = MAKEWORD(2, 2);
    WSADATA wsa_data;
    if (0 != WSAStartup( wsa_version, &wsa_data ))
    {
        log(LOG_ERROR, "WSAStartup failed");
        return NULL;
    }
#endif //_WIN32

    EditServer* srv = new EditServer();

    WebbyServerConfig config;

    memset(&config, 0, sizeof config);
    config.bind_address = "127.0.0.1";
    config.listening_port = 25115;
    config.flags = WEBBY_SERVER_LOG_DEBUG;
    config.connection_max = 4;
    config.request_buffer_size = 2048;
    config.io_buffer_size = 8192;
    config.dispatch = &edit_srv_dispatch;
    config.log = &edit_srv_log;

    size_t memory_size = WebbyServerMemoryNeeded(&config);

    srv->memory  = malloc(memory_size);

    srv->server = WebbyServerInit(&config, srv->memory, memory_size);

    if(!srv->server)
    {
        log(LOG_ERROR, "Problem initializing edit server.\n");
        assert(srv->server);
    }

    return srv;
}

void DestroyEditServer(EditServer* srv)
{
    WebbyServerShutdown(srv->server);
    free(srv->memory);

#ifdef _WIN32
    WSACleanup();
#endif //_WIN32
}

void UpdateEditServer(EditServer* srv)
{
    WebbyServerUpdate(srv->server);
}

#endif //WITH_EDIT_SERVER
