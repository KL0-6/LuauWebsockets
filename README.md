# LuauWebsockets

LuaU C API to interact with websockets using [IXWebSocket](https://www.github.com/machinezone/IXWebSocket)!

Example usage:
```lua
local socket = Socket.connect("ws://localhost:8008"); -- Connect to the socket

socket.OnMessage = function(message) -- Invoked when a message is received 
    print("New message from socket!", message);
    socket:Send(message)
end

socket.OnClose = function() -- Invoked when the connection is closed
    print("Socket Closed!");
end

socket:Send("Hello from lua"); -- Send a message
socket:Close(); -- Closes the connection
```
