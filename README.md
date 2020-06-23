# OBS-MIDI


Use MIDI devices to trigger events in OBS and visa versa.



# Status -- WIP
## Windows
[![Build Status](https://dev.azure.com/cpyarger0055/OBS-Midi/_apis/build/status/OBS-Midi?branchName=master&jobName=Build_Windows)](https://dev.azure.com/cpyarger0055/OBS-Midi/_build/latest?definitionId=5&branchName=master)
## macOS
[![Build Status](https://dev.azure.com/cpyarger0055/OBS-Midi/_apis/build/status/OBS-Midi?branchName=master&jobName=Build_macOS)](https://dev.azure.com/cpyarger0055/OBS-Midi/_build/latest?definitionId=5&branchName=master)
## Linux
[![Build Status](https://dev.azure.com/cpyarger0055/OBS-Midi/_apis/build/status/OBS-Midi?branchName=master&jobName=Build_Linux)](https://dev.azure.com/cpyarger0055/OBS-Midi/_build/latest?definitionId=5&branchName=master)
# Installing


1. Grab the zip from the [OBS-Midi Releases Page](https://github.com/Alzy/obs-midi/releases/tag/v0.1.0)

2. Copy the files in the zip to  your obs plugins directory.

  * By Default this is C:\Program Files\obs-studio\obs-plugins\64bit

3. Launch OBS

  * The Configuration is under Tools-> OBS Midi Settings

  * Select your controller, hit configure, Toggle a button and fader, remap it to an action and hit save!



Currently runs on Windows and Linux, and is untested on MAC


# Build instructions
In your obs-studio/plugins folder
1. ```git clone --recursive https://github.com/Alzy/obs-midi.git```
2. Append to CMakeLists.txt  
  * ```add_subdirectory(obs-midi)```
 Go back to your OBS Build directory
 ## Windows
 1. run ```cmake-gui.exe```
 2. click ```configure```
 3. click ```generate```
 4. click ```open project```
 5. In visual studio, right click on the obs-studio solution and click build
 ## Linux
 1. Rerun cmake using your normal options
 2. rerun ```make -j4```
 3. rerun ```sudo make install``` / ```sudo checkinstall```  depending on which you normally use.
 ### [More info on building OBS from source](https://github.com/obsproject/obs-studio/wiki/install-instructions)

 # Special thanks to the following projects
 * [RTMidi17](https://github.com/jcelerier/RtMidi17) An amazing modernized RTMIDI library
 * [OBSWebsocket](https://github.com/Palakis/obs-websocket/) from which much code was borrowed for interfacing with OBS
 * [Midi2OBS](https://github.com/lebaston100/MIDItoOBS) Where I got my start playing with this
