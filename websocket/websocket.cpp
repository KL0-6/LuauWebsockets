#include "websocket.h"

std::unordered_map<void*, std::shared_ptr<websocket::websocket>> socket_map{};

// This is required for Windows, useless for non windows platforms
void websocket::configure()
{
    ix::initNetSystem();
}

/*
    (<userdata>)socket_connect(string url)
        -> Connects to a given websocket URL and returns a userdata. Errors if it fails to connect.
*/
int socket_connect(lua_State* L)
{
    luaL_checkstring(L, 1);

    const char* url = lua_tostring(L, 1);

    std::shared_ptr<websocket::websocket> socket = std::make_shared<websocket::websocket>();

    if(!socket->setup(url)) 
    {
        luaL_error(L, "Failed to connect to URL!");
    }

    socket->L = L;

    // This creates a userdata and assosicates it websocket_methods
    void* userdata = lua_newuserdatatagged(L, sizeof(websocket::websocket), 0);
    lua_getfield(L, LUA_REGISTRYINDEX, "websocket_methods");
    lua_setmetatable(L, -2);

    socket_map.insert({ userdata, socket });

    return 1;
}

/*
    (<null>)socket_send(<userdata> socket, <string> message)
        -> Sends a message to socket if it's connected. 
*/
int socket_send(lua_State* L)
{
    luaL_checktype(L, 1, LUA_TUSERDATA);
    luaL_checktype(L, 2, LUA_TSTRING);

    void* userdata = lua_touserdata(L, lua_upvalueindex(1)); // There should be a upvalue containing the userdata

    if(socket_map.find(userdata) == socket_map.end()) // Make sure the userdata exists in the socket map
    {
        luaL_error(L, "could not find socket");
    }

    std::shared_ptr<websocket::websocket> socket = socket_map[userdata];

    if(socket->is_connected()) // Make sure we are connected to the URL
    {
        socket->send_message(lua_tostring(L, 2)); // Send the message
    }
    else
    {
        socket_map.erase(userdata); // Remove the userdata from the map since it is not connected
    }

    return 0;
}

/*
    (<null>)socket_close(<userdata> socket)
        -> Disconnects from the socket.
*/
int socket_close(lua_State* L)
{
    luaL_checktype(L, 1, LUA_TUSERDATA);

    void* userdata = lua_touserdata(L, lua_upvalueindex(1)); // There should be a upvalue containing the userdata

    if(socket_map.find(userdata) == socket_map.end()) // Make sure the userdata exists in the socket map
    {
        luaL_error(L, "could not find socket");
    }

    std::shared_ptr<websocket::websocket> socket = socket_map[userdata];

    socket->stop();

    socket_map.erase(userdata); // Remove the userdata from the map since it is now closed

    return 0;
}

/*
    Metamethods for websocket_methods
*/

int websocket_index(lua_State* L)
{
     void* userdata = lua_touserdata(L, 1);
    const char* key = lua_tostring(L, 2);

    if(socket_map.find(userdata) == socket_map.end())
    {
        luaL_error(L, "could not find socket");
    }

    std::shared_ptr<websocket::websocket> socket = socket_map[userdata];

    if(!strcmp(key, "Send"))
    {
        lua_pushvalue(L, 1);        
        lua_pushcclosure(L, socket_send, 0, 1);

        return 1;
    }
    else if(!strcmp(key, "Close"))
    {
        lua_pushvalue(L, 1);        
        lua_pushcclosure(L, socket_close, 0, 1);

        return 1;
    }

    return 0;
}

int websocket_newindex(lua_State* L)
{
    luaL_checktype(L, 1, LUA_TUSERDATA);
    luaL_checktype(L, 2, LUA_TSTRING);

    void* userdata = lua_touserdata(L, 1);
    const char* key = lua_tostring(L, 2);

    if(socket_map.find(userdata) == socket_map.end())
    {
        luaL_error(L, "could not find socket");
    }

    std::shared_ptr<websocket::websocket> socket = socket_map[userdata];

    if(!strcmp(key, "OnMessage"))
    {
        luaL_checktype(L, 3, LUA_TFUNCTION);

        socket->onmessage_ref = lua_ref(L, -1);
    }
    else if(!strcmp(key, "OnClose"))
    {
        luaL_checktype(L, 3, LUA_TFUNCTION);

        socket->onclose_ref = lua_ref(L, -1);
    }

    return 0;
}

void websocket::open_library(lua_State* L)
{
    luaL_newmetatable(L, "websocket_methods");
    {
        lua_pushcclosure(L, websocket_index, "websocket_index", 0);
        lua_setfield(L, -2, "__index");

        lua_pushcclosure(L, websocket_newindex, "websocket_newindex", 0);
        lua_setfield(L, -2, "__newindex");
    };

    lua_createtable(L, 0, 0);
    {
        lua_pushcclosure(L, socket_connect, "connect", 0);
        lua_setfield(L, -2, "connect");
    };
    lua_setglobal(L, "Socket");
}