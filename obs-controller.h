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


namespace OBSController
{
	// BUTTON ACTIONS
	void SetCurrentScene();
	void SetPreviewScene();
	void TransitionToProgram();
	void SetCurrentTransition();
	void SetSourceVisibility();
	void ToggleSourceVisibility();
	void ToggleMute();
	void SetMute();
	void StartStopStreaming();
	void StartStreaming();
	void StopStreaming();
	void StartStopRecording();
	void StartRecording();
	void StopRecording();
	void StartStopReplayBuffer();
	void StartReplayBuffer();
	void StopReplayBuffer();
	void SaveReplayBuffer();
	void PauseRecording();
	void ResumeRecording();
	void SetTransitionDuration();
	void SetCurrentProfile();
	void SetCurrentSceneCollection();
	void ResetSceneItem();
	void SetTextGDIPlusText();
	void SetBrowserSourceURL();
	void ReloadBrowserSource();
	void TakeSourceScreenshot();
	void EnableSourceFilter();
	void DisableSourceFilter();
	void ToggleSourceFilter();

	// CC ACTIONS
	void SetVolume(QString source, float volume);
	void SetSyncOffset();
	void SetSourcePosition();
	void SetSourceRotation();
	void SetSourceScale();
	void SetGainFilter();
	void SetOpacity();
};
