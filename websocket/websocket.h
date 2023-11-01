#ifndef WEBSOCKET_H
#define WEBSOCKET_H

#include <cstring>

#include <ixwebsocket/IXNetSystem.h>
#include <ixwebsocket/IXWebSocket.h>
#include <ixwebsocket/IXUserAgent.h>

#include <lualib.h>

namespace websocket
{
    class websocket
    {
    private:
        ix::WebSocket socket = ix::WebSocket();
    public:
        lua_State* L = nullptr;
        int onmessage_ref = 0;
        int onclose_ref = 0;

        ~websocket()
        {
            stop();
        }

        bool setup(const std::string& url)
        {
            socket.setUrl(url);
            socket.setOnMessageCallback([=](const ix::WebSocketMessagePtr& message)
            {
                switch(message->type)
                {
                    case ix::WebSocketMessageType::Message:
                    {
                        if(onmessage_ref)
                        {
                            lua_rawgeti(L, LUA_REGISTRYINDEX, onmessage_ref);
                            lua_pushlstring(L, message->str.c_str(), message->str.size());
                            lua_pcall(L, 1, 0, 0);
                        }

                        break;
                    }
                    case ix::WebSocketMessageType::Close:
                    {
                        if(onclose_ref)
                        {
                            lua_rawgeti(L, LUA_REGISTRYINDEX, onclose_ref);
                            lua_pcall(L, 0, 0, 0);
                        }

                        break;
                    }
                    default:
                        break;
                }
            });

            if(!socket.connect(30).success) // Failed to connect
            {
                return false; 
            }

            socket.start();

            return true;
        }

        void stop()
        {
            socket.stop();
        }

        void send_message(const std::string& message)
        {
            socket.sendText(message.c_str());
        }

        bool is_connected() const
        {
            return socket.getReadyState() == ix::ReadyState::Open;
        }
    };

    void configure();
    void open_library(lua_State* L);
}

#endif // WEBSOCKET_H