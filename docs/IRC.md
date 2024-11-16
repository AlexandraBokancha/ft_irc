# IRC RULES [RFC 1459](form https://www.rfc-editor.org/rfc/rfc1459)

## CLIENT
There are two types of clients, normal user and Operators (kind of like admin)
### NAMING
- Client have a nickname of maximum 9 characters

## SERVER RULES
- Server must have the following informnation about client :
    - Real name of the host the client is running on
    - Client username on that host
    - The server to which the client is connected

## CHANNEL
A channel is a group of one or more client chatting within eachother
### CHANNEL RULES
- The channel is created implicitly when the first client JOIN it.
- Channel naming :
    - The Channel name must begin with a '&' or '#' character
    - It may be of a length of 200 character (190 character im my testing)
    - Cannot contain <Ctrl+G> or coma ','

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
