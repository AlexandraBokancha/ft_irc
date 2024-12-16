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

-> You must be able to authenticate, set a nickname, a username, join a channel,
send and receive private messages using your reference client.

-> All the messages sent from one client to a channel have to be forwarded to
every other client that joined the channel.

-> You must have **operators** and **regular users**

------------------------------------------------------

Commands that are specific to channel
operators:

KICK - Eject a client from the channel

INVITE - Invite a client to a channel

TOPIC - Change or view the channel topic

MODE - Change the channel’s mode:

· i: Set/remove Invite-only channel

· t: Set/remove the restrictions of the TOPIC command to channel operators

· k: Set/remove the channel key (password)

· o: Give/take channel operator privilege

· l: Set/remove the user limit to channel