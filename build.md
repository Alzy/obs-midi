THESE INSTRUCTIONS ARE OUT OF DATE


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
