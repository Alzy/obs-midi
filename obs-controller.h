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
	void SetCurrentScene(MidiHook* hook);
	void SetPreviewScene(MidiHook* hook);
	void SetCurrentSceneCollection(MidiHook* hook);
	void ResetSceneItem(MidiHook *hook);
	void TriggerTransition();
	
	void TransitionToProgram(MidiHook *hook);
	void SetCurrentTransition(MidiHook *hook);
	void SetTransitionDuration(MidiHook *hook); // can also be used with cc

	void SetSourceVisibility(MidiHook *hook); // doesn't exist??
	void ToggleSourceVisibility(MidiHook *hook); //doesn't exist?

	void ToggleMute(MidiHook *hook);
	void SetMute(MidiHook *hook);

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

	void SetCurrentProfile(MidiHook *hook);
	void SetTextGDIPlusText(MidiHook *hook);
	void SetBrowserSourceURL(MidiHook *hook);
	void ReloadBrowserSource(MidiHook *hook);
	void TakeSourceScreenshot(MidiHook *hook);
	void EnableSourceFilter(MidiHook *hook);
	void DisableSourceFilter(MidiHook *hook);
	void ToggleSourceFilter(MidiHook *hook);

	// CC ACTIONS
	void SetVolume(MidiHook *hook);
	void SetSyncOffset(MidiHook *hook);
	void SetSourcePosition(MidiHook *hook);
	void SetSourceRotation(MidiHook *hook);
	void SetSourceScale(MidiHook *hook);
	void SetGainFilter(MidiHook *hook);
	void SetOpacity(MidiHook *hook);
	};
