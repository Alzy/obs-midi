project(obs-midi)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}")

include_directories(SYSTEM "${CMAKE_SOURCE_DIR}/UI/obs-frontend-api")

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

set(obs-midi_SOURCES
        obs-midi.cpp)

list(APPEND obs-midi_HEADERS
        obs-midi.h)

add_library(obs-midi MODULE
        ${obs-midi_SOURCES}
        ${obs-midi_HEADERS})

target_link_libraries(obs-midi
        ${obs-midi_DEPS}
        libobs
        obs-frontend-api
        rtmidi)

install_obs_plugin_with_data(obs-midi data)
