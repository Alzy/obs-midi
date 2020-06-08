# OBS-MIDI

Use MIDI devices to trigger events in OBS and visa versa.

Currently a WIP

# Installing


1. Grab the zip from the [OBS-Midi Releases Page](https://github.com/Alzy/obs-midi/releases)

2. Copy the files in the zip to  your obs plugins directory. 

  * By Default this is C:\Program Files\obs-studio\obs-plugins\64bit

3. Launch OBS

  * The Configuration is under Tools-> OBS Midi Settings

  * Select your controller, hit configure, Toggle a button and fader, remap it to an action and hit save!


Currently There is only a windows build, But as this uses the RTMIDI library, and is cross-platform compatible, we may release a binary for mac or windows. 

# Build instructions 
in your obs-studio/plugins folder
1. ```git clone --recursive https://github.com/Alzy/obs-midi.git```
2. Append to CMakeLists.txt  
  * ```add_subdirectory(obs-midi)```
 Go back to your obs Build directory
 ## windows
 1. run ```cmake-gui.exe```
 2. click ```configure```
 3. click ```generate```
 4. click ```open project```
 5. in visual studio, right click on the obs-studio solution and click build
 ## Linux
 1. Rerun cmake using your normal options
 2. rerun ```make -j4```
 3. rerun ```sudo make install``` / ```sudo checkinstall```  depending on which you normally use.
 ### [More info on building OBS from source](https://github.com/obsproject/obs-studio/wiki/install-instructions)
 
 # Special thanks to the following projects 
 * [RTMidi17](https://github.com/jcelerier/RtMidi17) An amazing modernized RTMIDI library 
 * [OBSWebsocket](https://github.com/Palakis/obs-websocket/) from which much code was borrowed for interfacing with obs 
