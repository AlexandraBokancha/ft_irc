# IRC RULES [RFC 1459](form https://www.rfc-editor.org/rfc/rfc1459)

## CLIENT
There are two types of clients, normal user and Operators (kind of like admin)

### NAMING
- Client have a unique nickname of maximum 9 characters

## SERVER RULES
- Server must have the following informnation about client :
    - Real name of the host the client is running on
    - Client username on that host
    - The server to which the client is connected

## CHANNEL
A channel is a group of one or more client chatting within eachother

### CHANNEL RULES
- The channel is created implicitly when the first client JOIN it. And it ceases to exist when the last client leaves it

- If the channel doesn't exist, the channel is created and the creating user becomes a **channel operator**

- Channel naming :
    - The Channel name must begin with a '&' or '#' character
    - It may be of a length of 200 character (190 character in my testing)
    - Cannot contain any spaces (' '), <Ctrl+G> or coma ','

## MESSAGES
- Each messages id composed as follow
    ```< prefix(optionnal) | command | command paramaters (up to 15) >```
    - prefix, comnmand and parmaters are separated by one or more ASCCI space ' '
    - Prefix is indicated by ':' at the beginning of message
    - If prefix then there must not be space character between ':' and prefix
- Message pseudo NBF representation :
    - <message>  ::= [':' <prefix> <SPACE> ] <command> <params> <crlf>
    - <prefix>   ::= <servername> | <nick> [ '!' <user> ] [ '@' <host> ]
    - <command>  ::= <letter> { <letter> } | <number> <number> <number>
    - <SPACE>    ::= ' ' { ' ' }
    - <params>   ::= <SPACE> [ ':' <trailing> | <middle> <params> ]
    - <middle>   ::= <Any *non-empty* sequence of octets not including SPACE or NUL or CR or LF, the first of which may not be ':'>
    - <trailing> ::= <Any, possibly *empty*, sequence of octets not including NUL or CR or LF>
    - <crlf>     ::= CR LF

## GENERAL RULES
- Coma are used to separate argument
- { } | are considered to be the lower equivqlent of []/ (Usefull for username comparison)
- IRC messages are always ended with CR-LF (Carriage return - Line feed)


## AUTHENTIFICATION

Upon connecting to an IRC server, a client is sent the MOTD (if
present) as well as the current user/server count (as per the LUSER
command). The server is also required to give an unambiguous message
to the client which states its name and version as well as any other
introductory messages which may be deemed appropriate.

After dealing with this, the server must then send out the **new user’s nickname**
and other information as supplied by itself (USER command)
and as the server could discover (from DNS/authentication servers).
The server must send this information out with **NICK first followed by **USER**.


Assuming you’ve chosen a nick that is not already taken, the IRC server will send back a RPL_WELCOME reply (which is assigned code 001). This reply has the following components:

:bar.example.com: The prefix. 

001: The numeric code for RPL_WELCOME.

amy: The first parameter which, in reply messages, must always be the nick of the user this reply is intended for.

:Welcome to the Internet Relay Network borja!borja@polaris.cs.uchicago.edu:
 The second parameter.

001    RPL_WELCOME
       "Welcome to the Internet Relay Network
        <nick>!<user>@<host>"

test:

:lindbohm.freenode.net 001 sorandom29 :Welcome to the freenode Internet Relay Chat Network sorandom29
