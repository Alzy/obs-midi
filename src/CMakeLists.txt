
##################
# THE GOOD STUFF #
##################

# RTMIDI
add_subdirectory(RtMidi17)

set(obs-midi_DEPS RtMidi17)

include_directories(RtMidi17)

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
	events.cpp
	rpc/RpcEvent.cpp

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
	events.h
	rpc/RpcEvent.h

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
