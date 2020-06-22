/*
obs-midi

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program. If not, see <https://www.gnu.org/licenses/>
*/

#pragma once

#include <iostream>

#include "utils.h"
#include "midi-agent.h"
#include "obs-midi.h"

namespace OBSController {
// BUTTON ACTIONS
void SetCurrentScene(const char *sceneName);
void SetPreviewScene(const char *sceneName);
void SetCurrentSceneCollection(QString sceneCollection);
void ResetSceneItem(const char *sceneName, const char *itemName);
void TransitionToProgram();
void TransitionToProgram(QString transitionName, int transitionDuration = 300);
void SetCurrentTransition(QString name);
void SetTransitionDuration(int duration); // can also be used with cc

void SetSourceVisibility();    // doesn't exist??
void ToggleSourceVisibility(); //doesn't exist?

void ToggleMute(QString sourceName);
void SetMute(QString sourceName, bool mute);

void StartStopStreaming();
void StartStreaming();
void StopStreaming();

void StartStopRecording();
void StartRecording();
void StopRecording();
void PauseRecording();
void ResumeRecording();

void StartStopReplayBuffer();
void StartReplayBuffer();
void StopReplayBuffer();
void SaveReplayBuffer();

void SetCurrentProfile(QString profileName);
void SetTextGDIPlusText();
void SetBrowserSourceURL();
void ReloadBrowserSource();
void TakeSourceScreenshot(QString source);
void EnableSourceFilter();
void DisableSourceFilter();
void ToggleSourceFilter();

// CC ACTIONS
void SetVolume(QString source, float volume);
void SetSyncOffset(QString sourceName, int64_t sourceSyncOffset);
void SetSourcePosition();
void SetSourceRotation();
void SetSourceScale();
void SetGainFilter();
void SetOpacity();
};
