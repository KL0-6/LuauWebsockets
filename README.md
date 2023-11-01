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

# Documentation: 

## Socket.connect
Connects to url. 

### Example
```lua
local socket = Socket.connect("ws://localhost:8008");
```

## Socket.send
Sends a text message to the socket.

### Parameters
 * `socket` - The socket userdata returned when you connect to a socket.
 * `message` - The message you want to send

## Socket.OnMessage
`ℹ️ Callback`
Optional callback invoked when a new message is received.

### Parameters
 * `message` - The message received

### Example
```lua
socket.OnMessage = function(message)
    print("New message from socket!", message);
end
```

## Socket.OnClose
`ℹ️ Callback`
Optional callback invoked when the connection is closed.

### Example
```lua
socket.OnClose = function()
    print("Socket Closed!");
end
```
