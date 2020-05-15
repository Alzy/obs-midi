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
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 REQUIRED COMPONENTS Core Widgets)

include_directories(
        SYSTEM
	${Qt5Core_INCLUDES}
	${Qt5Widgets_INCLUDES}
        "${CMAKE_SOURCE_DIR}/UI")

set(obs-midi_SOURCES
        utils.cpp
	config.cpp
	midi-agent.cpp
	device-manager.cpp
	obs-controller.cpp
        forms/settings-dialog.cpp
	forms/configwindow.cpp
        obs-midi.cpp
)

list(APPEND obs-midi_HEADERS
        utils.h
	config.h
	midi-agent.h
	device-manager.h
	obs-controller.h
        forms/settings-dialog.h
	forms/configwindow.h
        obs-midi.h
)

add_library(obs-midi MODULE
        ${obs-midi_SOURCES}
        ${obs-midi_HEADERS})

target_link_libraries(obs-midi
        ${obs-midi_DEPS}
        obs-frontend-api
        Qt5::Core
        Qt5::Widgets
        libobs)

install_obs_plugin_with_data(obs-midi data)
