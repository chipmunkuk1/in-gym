# Introduction #

This is the introduction to in\_gym.


# Details #

**in\_gym** was created because at the time, there were no other plugins to play gym files in winamp.  This plugin is the result of me taking an interest in getting a player working in winamp after creating my other plugin (in\_npsg, with played zx spectrum ay log files).

# Version History #
|Version|Date|Comment|
|:------|:---|:------|
|v0.1.6.4|27-Aug-2004|Tweaked MJazz some more, sounds better I think...|
|v0.1.6.3|26-Aug-2004|Tweaked MJazz to actually do somthing... doh! :)|
|v0.1.6.2|26-Aug-2004|Woot, MJazz support has been added (I hope it's right, if it's not, please let me know! =)|
|v0.1.6.1|16-Aug-2004|Changed gymx title option slightly, when checked, will show the gymx title instead of the filename, when un-checked, will show the filename instead. If the gymx header is not present, it will just show the filename.|
|v0.1.6.0|14-Aug-2004|Added option to turn off gymx titles|
|v0.1.5.9|27-Dec-2003|Added option to disable VGM support so that you can, if you want, have vgm files handled by another plugin...  This setting requires a restart of winamp when changed to become active...|
|v0.1.5.8|26-Dec-2003|Added some more settings to play with, you can now change the replay rate to slow down or speed up the tunes, 50/60/70Hz or 0.83/1.00/1.17x.|
|v0.1.5.7|02-Oct-2003|Added SMS FM playback! Woohar! (Took blimmin ages, cos MAMES YM2413 code is b0rked! I had to take code from another vgm player to get it to work!) Also, all sample rates are now working again, for GYM and VGM musics.|
|v0.1.5.6|  |Added preliminary VGM decoding.  Plays most SMS/Megadrive/Game Gear songs ok.  You need to un-compress your .vgz files into .vgm files for this plugin to play them...  GZip support will eventually be added when I get my head around the gz code.|
|  |  |**Renember** : set the plugin for 44100hz sample rate, or the vgm files will playback totally wrong!|
|v0.1.5.5|  |Added settings dialog so the sample rate can be changed. Change is made when the next song starts, or if current song is stopped and restarted.|
|v0.1.5.4|  |Changed the default rendering rate to 96000, should make for some higher quality output.|
|v0.1.5.3|  |Fixed seeking, added code to pull the title of the GYM file (if it has a GYMX header that is) To do: Add code so that GYMX headers can be viewed / edited / stored. (uh.... eep!  total C newbie here! :) Apart from the missing gymx editing, and compression, this plugin is as complete as it's going to get me thinks! ;)|
|v0.1.5.2|  |Implemented vBlank counting thanks to a prod by Mangas 2000.  Now the  position bar, and song length actually reflect the length of the song and not the length of the file!|
|v0.1.5.1|  |Fixed a silly bug, (with version 0.1.5.0, bring up the playlist and add a few gym files, see what happens?, now load a few more, eventually, WinAMP will crash.... ;)  - ooops!|
|v0.1.5.0|  |Added in PCM decoding, and GYMX header support (does nothing yet...)|
|v0.1.0.2|  |Not updated version number yet, but i now have a gui!! :) thankz Nitro!!! :)|
|v0.1.0.2|  |Fixed another memory leak, sonf should not stop now if on loop mode.|
|v0.1.0.1|  |Fixed stupid memory leak, should fix some of those crashes, also fixed the ym corruption bug (i.e. instruments from the last ym file, playing in the next one, missing channels, etc)|
|v0.1.0.0|  |It's done!!!  A GYM plugin for winamp!!!! :) hoo F**ing ray!!!! :)**|