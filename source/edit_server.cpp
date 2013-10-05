// Copyright (c) 2013, Nicholas "Indy" Ray. All rights reserved.
// See the LICENSE file for usage, modification, and distribution terms.

#include "edit_server.h"
#ifdef WITH_EDIT_SERVER

#include "reporting.h"
#include "reflect.h"

#include "webby/webby.h"
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include <map>
#include <string>

#ifdef _WIN32
#include <winsock2.h>
#endif //_WIN32

struct EditObject
{
    void* obj;
    Reflect* reflect;
};

struct string_compare
{
    bool operator() (const char* x, const char* y) const
    {
        return stricmp(x, y);
    }
    typedef char* first_argument_type;
    typedef char* second_argument_type;
    typedef bool result_type;
};

struct EditServer
{
    void* memory;
    WebbyServer* server;

    std::map<char*, EditObject*, string_compare> objMap;
};

EditServer* g_edit_server;

static void returnError(struct WebbyConnection* connection)
{
    WebbyBeginResponse(connection, 500, 14, NULL, 0);
    WebbyWrite(connection, "Internal Error", 14);
    WebbyEndResponse(connection);
}

static int edit_srv_dispatch(struct WebbyConnection* connection)
{
    const int num_headers = 3;
    WebbyHeader headers[num_headers];
    headers[0].name = "Access-Control-Allow-Origin";
    headers[0].value = "*";
    headers[1].name = "Access-Control-Allow-Headers";
    headers[1].value = "origin, content-type, accept";
    headers[2].name = "Access-Control-Allow-Methods";
    headers[2].value = "POST, PUT, GET";

    if(stricmp(connection->request.method, "OPTIONS") == 0)
    {
        WebbyBeginResponse(connection, 200, 14, headers, num_headers);
        WebbyWrite(connection, "\n\n", 14);
        WebbyEndResponse(connection);
    }
    else if(stricmp(connection->request.method, "PUT") == 0)
    {
        char* str = new char[strlen(connection->request.uri) + 1];
        strcpy(str, connection->request.uri);

        const char* delim = "/\\";

        char* tok = strtok(str, delim);

        if(stricmp(tok, "object") == 0)
        {
            tok = strtok(NULL, delim);
            EditObject* obj = tok ? g_edit_server->objMap[tok] : NULL;
            if(obj) {
                void* ptr = obj->obj;
                Reflect* reflect = obj->reflect;
                tok = strtok(NULL, delim);
                while(tok) {
                    Reflect* r = reflect->get_property(tok);
                    if(r)
                    {
                        reflect = r;
                        ptr = reflect->get_pointer(ptr);
                    }
                    tok = strtok(NULL, delim);
                }

                assert(ptr && reflect);

                size_t len = connection->request.content_length;
                assert(len < 255); // Just until we have more robust  parsing put together.
                char* buf = new char[len + 1];
                WebbyRead(connection, buf, len);
                buf[len] = '\0';
                assert(reflect->get_type() == Type_Float);
                float f = (float)atof(buf);
                reflect->set_float(ptr, f);
                delete[] buf;
                WebbyBeginResponse(connection, 200, 14, headers, num_headers);
                WebbyWrite(connection, "SUCCESS", 14);
                WebbyEndResponse(connection);
            }
            else
            {
                returnError(connection);
            }
        }
        delete[] str;
    }
    else
    {
        WebbyBeginResponse(connection, 404, 14, NULL, 0);
        WebbyWrite(connection, "Resource not Found", 14);
        WebbyEndResponse(connection);
    }
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

    memset(&config, 0, sizeof(config));
    config.bind_address = "127.0.0.1";
    config.listening_port = 25115;
    config.flags = 0;
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

    g_edit_server = srv;
    return srv;
}

void DestroyEditServer(EditServer* srv)
{

    for(auto it = srv->objMap.begin(); it != srv->objMap.end(); it++)
    {
        delete[] it->first;
        delete it->second;
    }

    WebbyServerShutdown(srv->server);
    free(srv->memory);

    delete srv;

#ifdef _WIN32
    WSACleanup();
#endif //_WIN32
}

void UpdateEditServer(EditServer* srv)
{
    WebbyServerUpdate(srv->server);
}

void EditServerAddObject(EditServer* srv, const char* name, void* ptr, Reflect* reflect)
{
    size_t len = strlen(name);
    char* key = new char[len + 1];
    strcpy(key, name);
    EditObject* obj = new EditObject();
    obj->obj = ptr;
    obj->reflect = reflect;
    srv->objMap[key] = obj;
}

#endif //WITH_EDIT_SERVER
