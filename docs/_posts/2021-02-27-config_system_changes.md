---
layout: post
title:  "Configuration System Changes"
date:   2021-02-27 10:10:06 -0500
categories: Release update
---

# BREAKING CHANGES
PLEASE NOTE THERE ARE BREAKING CHANGES BETWEEN THE OLD VERSION AND THIS
you WILL need to recreate your mappings, This should not be an ongoing issue.

### New Features
- Added feedback for the following actions (Credit: @dtg01100):
  - Start Streaming
  - Stop Streaming
  - Toggle Start / Stop Streaming
  - Start Recording
  - Stop Recording
  - Toggle Start / Stop Recording

### Config System Updates
- The config file subsystem has had a major rewrite
- No longer are we using the profile config ini file
- All config files are now json files
- Config file location is now under the {obs config dir}/plugin_config/obs-midi directory
- Config file names now use the following standard obs-midi_{Profile Name}_{Scene Collection Name}.json
- Config files now use obs_data_save_json_safe function
- Saves the data to a file as JSON text, and if overwriting an old file, backs up that old file to help prevent potential file corruption.

### Other BugFixes
- OBS Midi now has limited support for switching scene collections and profiles.
- The Majority of memory leaks have been cleaned up.
- Crashes on Toggle Mute have been resolved

New videos will be out soon.

### Issues, Ideas, Bugfixes
- Any issues or bugs can be reported at the [Issue Tracker](https://github.com/cpyarger/obs-midi/issues)
- Any feature requests please post in the ideas section of the [OBS-MIDI Discussions](https://github.com/cpyarger/obs-midi/discussions/categories/ideas)
