---
layout: page
title: Downloads
permalink: /Downloads/
---
![CI Multiplatform Build and release](https://github.com/cpyarger/obs-midi/workflows/CI%20Multiplatform%20Build%20and%20release/badge.svg?branch=master)
[![Open Source Helpers](https://www.codetriage.com/cpyarger/obs-midi/badges/users.svg)](https://www.codetriage.com/cpyarger/obs-midi)
[![Crowdin](https://badges.crowdin.net/obs-midi/localized.svg)](https://crowdin.com/project/obs-midi)
[![CodeFactor](https://www.codefactor.io/repository/github/cpyarger/obs-midi/badge/master)](https://www.codefactor.io/repository/github/cpyarger/obs-midi/overview/master)

# Installing
1. Grab the installer from the [OBS-Midi Releases Page](https://github.com/cpyarger/obs-midi/releases/latest)
2. Download and run the installer for your OS
3. Launch OBS
  * The Configuration is under Tools-> OBS Midi Settings
  * Select your controller, hit configure, Toggle a button and fader, remap it to an action and hit save!

# Installing Extra Steps - MAC
- If you are unable to see obs-midi in the tools you will need to install jack
- https://jackaudio.org/downloads/

# Ubuntu specifics - SNAP Release
Due to specific ALSA permissions not being set on the ubuntu snap craft package, OBS-MIDI will not work on the snap version of obs-studio
