# Still needed
  - Review user/Channel MODE. To spot bug fixes an mode intyeraction:
  - Do the WHO command family function (WHOIS ...)
  - Fix read incomplete messages (keep reading until CRLF is not reached)
  - Add broadcastToChannel for QUIT, PRIVMSG/NOTICE sent to channel
 
 - MODE
      -> user mode +/-o is not functional
      -> channel mode +/-i, +/-t, +/-l, +/-k are not working correctly
  - JOIN : if i make JOIN 0 it creates #0 channel, but it's an invalide channel name (it must begin with a '&' or '#')

# NOT obligatory
  - MOTD when user is registred