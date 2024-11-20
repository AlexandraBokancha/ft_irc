# IRC RULES [RFC 1459](form https://www.rfc-editor.org/rfc/rfc1459)


# PASS

Command : PASS
Parameters : <password>

Should be sent before the NICK/USER combination and before any SERVER command.

Numeric replies:

ERR_NEEDMOREPARAMS
ERR_ALREADYREGISTRED
ERR_PASSWDMISMATCH

# NICK

Command : NICK
Parameters: <nickname> [ <hopcount> ]

The <hopcount> must be ignored if supplied by a client.

Numeric Replies:

ERR_NONICKNAMEGIVEN

ERR_NICKNAMEINUSE 433
    "<nick> :Nickname is already in use"
    - Returned when a NICK message is processed that results
    in an attempt to change to a currently existing
    nickname.

ERR_ERRONEUSNICKNAME 432
    "<nick> :Erroneus nickname"
    - Returned after receiving a NICK message which contains characters which do not fall in the defined set. See section x.x.x for details on valid nicknames.

    section X:
    1. nickname must be unique
    2. having a lenght of 9 max

    Each client is distinguished from other clients by a unique
nickname having a **maximum length of nine (9) characters**. See the
protocol grammar rules for what may and may not be used in a
nickname. 

Nicknames cannot contain spaces, commas, or special symbols like @, #, and :.
These symbols are reserved for commands or channels.
    
<!-- 
ERR_NICKCOLLISION 444
    "<nick> :Nickname collision KILL"
    - Returned by a server to a client when it detects a
    nickname collision (registered of a NICK that
    already exists by another server). -->