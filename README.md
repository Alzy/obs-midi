# OBS-MIDI
Use MIDI devices to trigger events in OBS and visa versa.

OBS MIDI came about because two guys complained about the same thing, within the same hour, and decided to try to take on the task

Those two guys are
* [Alzy](https://github.com/Alzy) and
* [Chris](http://github.com/cpyarger)


# Status -- ![CI Multiplatform Build and release](https://github.com/cpyarger/obs-midi/workflows/CI%20Multiplatform%20Build%20and%20release/badge.svg?branch=master)
[![Open Source Helpers](https://www.codetriage.com/cpyarger/obs-midi/badges/users.svg)](https://www.codetriage.com/cpyarger/obs-midi)
[![Crowdin](https://badges.crowdin.net/obs-midi/localized.svg)](https://crowdin.com/project/obs-midi)
[![CodeFactor](https://www.codefactor.io/repository/github/cpyarger/obs-midi/badge/master)](https://www.codefactor.io/repository/github/cpyarger/obs-midi/overview/master)
Currently runs on Windows and Linux, and MAC

# Installing


1. Grab the installer from the [OBS-Midi Releases Page](https://github.com/cpyarger/obs-midi/releases)

2. Download and run the installer for your OS

3. Launch OBS

  * The Configuration is under Tools-> OBS Midi Settings

  * Select your controller, hit configure, Toggle a button and fader, remap it to an action and hit save!

# Installing Extra Steps - MAC
- If you are unable to see obs-midi in the tools you will need to install jack
- https://jackaudio.org/downloads/

# Installing Extra Steps - Ubuntu SNAP
- If you install OBS Studio via snap you will need to run the following command to give permissions for OBS to access the raw MIDI devices through ALSA

```bash
sudo snap connect obs-studio:alsa
```

Currently runs on Windows and Linux, and is partly tested on MAC

 # Special thanks to the following projects
 * [RTMidi17](https://github.com/jcelerier/RtMidi17) An amazing modernized RTMIDI library
 * [OBSWebsocket](https://github.com/Palakis/obs-websocket/) from which much code was borrowed for interfacing with OBS
 * [Midi2OBS](https://github.com/lebaston100/MIDItoOBS) Where I got my start playing with this

# Build Instructions
[Build instructions can be found here](build.md)

## Special Thanks to our other contributors
* [dtg01100](https://github.com/dtg01100)
* [garbart](https://github.com/garbart)
* [DDRBoxman](https://github.com/DDRBoxman)
* [kernel-coder](https://github.com/kernel-coder)
* [Luxcium](https://github.com/Luxcium)

# Contributing

Anyone who wants to help out the project

- Drop any ideas or suggestions or bugs in the issues
- Leave an honest review on the [obs forum](https://obsproject.com/forum/resources/obs-midi.1023/)
- If you know anyone who would be willing to help with the programming let them know about the project
- If you or anyone you know makes youtube videos, we are always looking for reviews, and tutorial videos �
- Contribute to translations on [crowdin](https://crowdin.com/project/obs-midi)
- If you know C++ we can always use help with cleaning up the code base and especially hunting and fixing memory leaks.
