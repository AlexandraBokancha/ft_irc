Requirements for the project:

-> You mustn't develop a client

-> You mustn't handle server-to-server communication

----------------------------------------------------
-> The server must be capable of handling multiple clients
at the same time and never hang

-> Forking is not allowed. All I/O operations must be 
non-blocking

-> Only 1 poll() can be used for handling all these operations

-> Communication between client and server has to be done
via TCP/IP (v4 or v6)

-> Using your reference client with your server must be
similiar to using it with any official IRC server

------------------------------------------------------

Features to implement:
