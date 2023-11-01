#include <stdio.h>
#include <iostream>

#include <lualib.h>
#include <Luau/Compiler.h>

#include "websocket/websocket.h"

#include <ixwebsocket/IXWebSocketServer.h>

int main()
{
    websocket::configure();
    
    lua_State* L = luaL_newstate(); // Create our lua_State
    luaL_openlibs(L); // Register the default lua libraries & functions, such as print, tostring, etc.
    websocket::open_library(L);
    
    std::string script = R"(
        local socket = Socket.connect("ws://localhost:8008");

        socket.OnMessage = function(message)
            print("New message from socket!", message);
            socket:Send(message)
        end

        socket.OnClose = function()
            print("Socket Closed!");
        end

        socket:Send("Hello from lua");
        socket:Close();
    )";

    std::string bytecode = Luau::compile(script);
    if(luau_load(L, "@", bytecode.c_str(), bytecode.size(), 0)) // luau_load will return 1 if there is an issue deserializing
    {
        std::printf("Failed to deserializing bytecode %s!\n", lua_tostring(L, -1)); // The error is pushed onto the stack.

        return -1;
    }

    if(lua_pcall(L, 0, 0, 0)) // If the result isn't 0, an error occured while running the script.
    {
        std::printf("Script Error %s!\n", lua_tostring(L, -1)); // Pcall pushes the error onto the stack.

        return -1;
    }

    return 0;
}