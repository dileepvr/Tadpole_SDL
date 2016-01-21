Tadpole is a persistent server casual game meant for projection on a big screen in a live environment. Players connect to it via TCP to spawn players. There is a live live leader board and hi-score board.

It is written in SDL and C++.

This is work in progress.
The license is GPLv3. See LICENSE.txt for full text of license.


The files ipaddr.c and ipaddr.h are slightly modified versions of
files from a lightweight webserver project called
File-and-Froget. They were written by Tyler Montbriand. The code is
hosted on: http://burningsmell.org/faf/. He didn't release it under a
license, so I hope this attribution is enough!



Welcome to Tadpole_SDL.
A game by Dileep V. Reddy.
Network controller written in collaboration with Wes Erickson.
The project and license details can be found at:
https://github.com/dileepvr/Tadpole_SDL
https://github.com/inflamedspirit/Tadpole_Controller
To join the game using our Javascript controller, simply visit localhost:<JAVAPORT>.
A help file with this information can be accessed in localhost:<HELPPORT>.
To write your own custom controller, follow the protocol below:
Tadpole server is listening for connections on port <PORT>.
To spawn a controllable tadpole,
1. Open a TCP connection to localhost:<PORT>
2. If a spot is not available, server sends 'X' and disconnects.
3. If spot is available, server sends 'N'.
4. Respond with desired player name.
5. Server sends \RGBXXYYZZ\, where \XXYYZZ\ are the RGB color values in hex (as a string). The Tadpole should be visible on screen.
6. To control Tadpole, send the following characters:
'1' = Up-key-pressed
'2' = Up-key-released
'3' = Right-key-pressed
'4' = Right-key-released
'5' = Down-key-pressed
'6' = Down-key-released
'7' = Left-key-pressed
'8' = Left-key-released
7. To kill Tadpole, send 'K'. Server replies with 'D'.
8. Everytime Tadpole hitpoints change, server sends \HXX\, where \XX\ encodes current hitpoint count.
9. Server sends 'D' and closes connection when Tadpole dies.
10. Server sends 'C' and closes connection when shutting down.

The game can be played as a single player game. To spawn locally (keyboard) controlled tadpole, just it enter. The name is set inside main.cpp. Arrow keys control movement, and pressing 'k' key will force kill.
