cmake_minimum_required(VERSION 2.8)
project(rtmidi)

if (WIN32)
    add_definitions(-D__WINDOWS_MM__)
elseif(APPLE)
    add_definitions(-D__MACOSX_CORE__)
elseif(UNIX)
    add_definitions(-D__LINUX_ALSA__ -fPIC -lalsa)
endif()
# ALSA
if(RTMIDI_API_ALSA)
  set(NEED_PTHREAD ON)
  find_package(ALSA)
  if (NOT ALSA_FOUND)
    message(FATAL_ERROR "ALSA API requested but no ALSA dev libraries found")
  endif()
  list(APPEND INCDIRS ${ALSA_INCLUDE_DIR})
  list(APPEND LINKLIBS ${ALSA_LIBRARY})
  list(APPEND PKGCONFIG_REQUIRES "alsa")
  list(APPEND API_DEFS "-D__LINUX_ALSA__")
  list(APPEND API_LIST "alsa")
endif()
set(RTMIDI_SOURCES
        RtMidi.cpp
        rtmidi_c.cpp)

set(RTMIDI_HEADERS
        RtMidi.h
        rtmidi_c.h)

add_library(rtmidi STATIC
        ${RTMIDI_HEADERS}
        ${RTMIDI_SOURCES})
