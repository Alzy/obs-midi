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

#include <iostream>

#include "utils.h"
#include "midi-agent.h"
#include "obs-controller.h"



////////////////////
// BUTTON ACTIONS //
////////////////////

void OBSController::SetCurrentScene() {}

void OBSController::SetPreviewScene() {}

void OBSController::TransitionToProgram() {}

void OBSController::SetCurrentTransition() {}

void OBSController::SetSourceVisibility() {}

void OBSController::ToggleSourceVisibility() {}

void OBSController::ToggleMute() {}

void OBSController::SetMute() {}

void OBSController::StartStopStreaming() {}

void OBSController::StartStreaming() {}

void OBSController::StopStreaming() {}

void OBSController::StartStopRecording() {}

void OBSController::StartRecording() {}

void OBSController::StopRecording() {}

void OBSController::StartStopReplayBuffer() {}

void OBSController::StartReplayBuffer() {}

void OBSController::StopReplayBuffer() {}

void OBSController::SaveReplayBuffer() {}

void OBSController::PauseRecording() {}

void OBSController::ResumeRecording() {}

void OBSController::SetTransitionDuration() {}

void OBSController::SetCurrentProfile() {}

void OBSController::SetCurrentSceneCollection() {}

void OBSController::ResetSceneItem() {}

void OBSController::SetTextGDIPlusText() {}

void OBSController::SetBrowserSourceURL() {}

void OBSController::ReloadBrowserSource() {}

void OBSController::TakeSourceScreenshot() {}

void OBSController::EnableSourceFilter() {}

void OBSController::DisableSourceFilter() {}

void OBSController::ToggleSourceFilter() {}



////////////////
// CC ACTIONS //
////////////////

void OBSController::SetVolume(QString source, float volume)
{
	OBSSourceAutoRelease obsSource =
		obs_get_source_by_name(source.toUtf8());
	if (!obsSource) {
		return; // source does not exist
	}

	obs_source_set_volume(obsSource, volume);
}

void OBSController::SetSyncOffset() {}

void OBSController::SetSourcePosition() {}

void OBSController::SetSourceRotation() {}

void OBSController::SetSourceScale() {}

void OBSController::SetGainFilter() {}

void OBSController::SetOpacity() {}
