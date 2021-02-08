# OBS-MIDI


Use MIDI devices to trigger events in OBS and visa versa.



# Status -- WIP
![CI Multiplatform Build](https://github.com/cpyarger/obs-midi/workflows/CI%20Multiplatform%20Build/badge.svg)
# Installing


1. Grab the zip from the [OBS-Midi Releases Page](https://github.com/Alzy/obs-midi/releases)

2. Copy the files in the zip to  your obs plugins directory.

  * By Default this is C:\Program Files\obs-studio\obs-plugins\64bit

3. Launch OBS

  * The Configuration is under Tools-> OBS Midi Settings

  * Select your controller, hit configure, Toggle a button and fader, remap it to an action and hit save!



Currently runs on Windows and Linux, and is untested on MAC


 # Special thanks to the following projects
 * [RTMidi17](https://github.com/jcelerier/RtMidi17) An amazing modernized RTMIDI library
 * [OBSWebsocket](https://github.com/Palakis/obs-websocket/) from which much code was borrowed for interfacing with OBS
 * [Midi2OBS](https://github.com/lebaston100/MIDItoOBS) Where I got my start playing with this
