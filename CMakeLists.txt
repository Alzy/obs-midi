cmake_minimum_required(VERSION 3.10)
# Change obs-plugintemplate to your plugin's name in a machine-readable format
# (e.g.: obs-myawesomeplugin) and set
project(obs-midi VERSION 0.2.4)

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
add_subdirectory(RtMidi17)

set(obs-midi_DEPS RtMidi17)

include_directories(RtMidi17)




# OBS MIDI
set(CMAKE_PREFIX_PATH "${QTDIR}")
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_definitions(-DASIO_STANDALONE)

if (WIN32 OR APPLE)
	include(external/FindLibObs.cmake)
endif()

find_package(LibObs REQUIRED)
find_package(Qt5 REQUIRED COMPONENTS Core Widgets)

include_directories(
"${LIBOBS_INCLUDE_DIR}/../UI/obs-frontend-api"

        SYSTEM
	${Qt5Core_INCLUDES}
	${Qt5Widgets_INCLUDES}
        "${CMAKE_SOURCE_DIR}/UI")

set(obs-midi_SOURCES
  src/utils.cpp
	src/config.cpp
	src/midi-agent.cpp
	src/device-manager.cpp
	src/obs-controller.cpp
  src/forms/settings-dialog.cpp
	src/forms/configwindow.cpp
  src/obs-midi.cpp
	src/events.cpp
	src/rpc/RpcEvent.cpp

)

list(APPEND obs-midi_HEADERS
    src/utils.h
		src/config.h
	  src/midi-agent.h
	  src/device-manager.h
	  src/obs-controller.h
    src/forms/settings-dialog.h
	  src/forms/configwindow.h
    src/obs-midi.h
	  src/events.h
	  src/rpc/RpcEvent.h
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
        # --- Linux-specific build settings and tasks ---
        if(UNIX AND NOT APPLE)
        	include(GNUInstallDirs)

        	set_target_properties(obs-midi PROPERTIES PREFIX "")
        	target_link_libraries(obs-midi obs-frontend-api)

        	file(GLOB locale_files data/locale/*.ini)

        	if(${USE_UBUNTU_FIX})
        		install(TARGETS obs-midi
        			LIBRARY DESTINATION "/usr/lib/obs-plugins")
        	endif()
        	install(TARGETS obs-midi
        		LIBRARY DESTINATION "${CMAKE_INSTALL_FULL_LIBDIR}/obs-plugins")

        	install(FILES ${locale_files}
        		DESTINATION "${CMAKE_INSTALL_FULL_DATAROOTDIR}/obs/obs-plugins/obs-midi/locale")
        endif()
        # --- End of section ---

        # -- OS X specific build settings and tasks --
        if(APPLE)
        	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++ -fvisibility=default")

        	set(CMAKE_SKIP_RPATH TRUE)
        	set_target_properties(obs-midi PROPERTIES PREFIX "")
        	target_link_libraries(obs-midi "${OBS_FRONTEND_LIB}")
        endif()
        # -- End of section --

#install_obs_plugin_with_data(obs-midi data)
