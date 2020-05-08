cmake_minimum_required(VERSION 3.10)

# Change obs-plugintemplate to your plugin's name in a machine-readable format
# (e.g.: obs-myawesomeplugin) and set 
project(obs-midi VERSION 1.0.0)

# Replace `Your Name Here` with the name (yours or your organization's) you want
# to see as the author of the plugin (in the plugin's metadata itself and in the installers)
set(PLUGIN_AUTHOR "Alzy")

# Replace `com.example.obs-plugin-template` with a unique Bundle ID for macOS releases
# (used both in the installer and when submitting the installer for notarization)
set(MACOS_BUNDLEID "com.example.obs-midi")

# Replace `me@contoso.com` with the maintainer email address you want to put in Linux packages
set(LINUX_MAINTAINER_EMAIL "alfredo@seriousbusiness.la")

if(WIN32)
	set(MODULE_DESCRIPTION "OBS MIDI module")
endif()



##################
# THE GOOD STUFF #
##################

# RTMIDI
add_subdirectory(rtmidi)

set(obs-midi_DEPS rtmidi)

include_directories(rtmidi)

if(APPLE)
    find_library(COREMIDI CoreMIDI)
    include_directories(${COREMIDI})
    find_library(COREAUDIO CoreAudio)
    include_directories(${COREAUDIO})
    find_library(COREFOUNDATION CoreFoundation)
    include_directories(${COREFOUNDATION})

    set(obs-midi_DEPS
            ${obs-midi_DEPS}
            ${COREMIDI}
            ${COREAUDIO}
            ${COREFOUNDATION})
elseif(WIN32)
    set(obs-midi_DEPS
            ${obs-midi_DEPS}
            winmm.lib)
endif()


# OBS MIDI
set(obs-midi_SOURCES
        obs-midi.cpp)

list(APPEND obs-midi_HEADERS
        obs-midi.h)

add_library(obs-midi MODULE
        ${obs-midi_SOURCES}
        ${obs-midi_HEADERS})

target_link_libraries(obs-midi
      ${obs-midi_DEPS}
      libobs)

install_obs_plugin_with_data(obs-midi data)