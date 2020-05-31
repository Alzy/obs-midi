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

using namespace std;

////////////////////
// BUTTON ACTIONS //
////////////////////

/**
 * Sets the currently active scene
 */
void OBSController::SetCurrentScene(const char *sceneName)
{
	OBSSourceAutoRelease source = obs_get_source_by_name(sceneName);

	if (source) {
		obs_frontend_set_current_scene(source);
	} else {
		throw("requested scene does not exist");
	}
}

/**
 * Sets the scene in preview. Must be in Studio mode or will throw error
 */
void OBSController::SetPreviewScene(const char *sceneName)
{
	if (!obs_frontend_preview_program_mode_active()) {
		throw("studio mode not enabled");
	}
	OBSScene scene = Utils::GetSceneFromNameOrCurrent(sceneName);
	if (!scene) {
		throw("specified scene doesn't exist");
	}

	obs_frontend_set_current_preview_scene(obs_scene_get_source(scene));
}

/**
 * Change the active scene collection.
 */
void OBSController::SetCurrentSceneCollection(QString sceneCollection)
{
	if (sceneCollection.isEmpty()) {
		throw("Scene Collection name is empty");
	}

	// TODO : Check if specified profile exists and if changing is allowed
	obs_frontend_set_current_scene_collection(sceneCollection.toUtf8());
}

/**
* Reset a scene item.
*/
void OBSController::ResetSceneItem(const char *sceneName, const char *itemName)
{
	OBSScene scene = Utils::GetSceneFromNameOrCurrent(sceneName);
	if (!scene) {
		throw("requested scene doesn't exist");
	}

	obs_data_t *params = obs_data_create();
	obs_data_set_string(params, "scene-name", sceneName);
	OBSDataItemAutoRelease itemField = obs_data_item_byname(params, "item");

	OBSSceneItemAutoRelease sceneItem =
		Utils::GetSceneItemFromRequestField(scene, itemField);
	if (!sceneItem) {
		throw("specified scene item doesn't exist");
	}

	OBSSource sceneItemSource = obs_sceneitem_get_source(sceneItem);

	OBSDataAutoRelease settings = obs_source_get_settings(sceneItemSource);
	obs_source_update(sceneItemSource, settings);
}

/**
 * Transitions the currently previewed scene to the main output.
 */
void OBSController::TransitionToProgram()
{
	obs_frontend_preview_program_trigger_transition();
}

/**
 * Transitions the currently previewed scene to the main output using specified transition.
 * transitionDuration is optional. (milliseconds)
 */
void OBSController::TransitionToProgram(QString transitionName,
					int transitionDuration)
{
	if (!obs_frontend_preview_program_mode_active()) {
		throw("studio mode not enabled");
	}

	if (transitionName.isEmpty()) {
		throw("transition name can not be empty");
	}
	bool success = Utils::SetTransitionByName(transitionName);
	if (!success) {
		throw("specified transition doesn't exist");
	}
	obs_frontend_set_transition_duration(transitionDuration);

	obs_frontend_preview_program_trigger_transition();
}

/**
 * Set the active transition.
 */
void OBSController::SetCurrentTransition(QString name)
{
	bool success = Utils::SetTransitionByName(name);
	if (!success) {
		throw("requested transition does not exist");
	}
}

/**
 * Set the duration of the currently active transition
 */
void OBSController::SetTransitionDuration(int duration)
{
	obs_frontend_set_transition_duration(duration);
}

void OBSController::SetSourceVisibility() {} //DOESNT EXIST

void OBSController::ToggleSourceVisibility() {} //DOESNT EXIST

/**
* Inverts the mute status of a specified source.
*/
void OBSController::ToggleMute(QString sourceName)
{
	if (sourceName.isEmpty()) {
		throw("sourceName is empty");
	}

	OBSSourceAutoRelease source =
		obs_get_source_by_name(sourceName.toUtf8());
	if (!source) {
		throw("sourceName not found");
	}

	obs_source_set_muted(source, !obs_source_muted(source));
}

/**
 * Sets the mute status of a specified source.
 */
void OBSController::SetMute(QString sourceName, bool mute)
{
	if (sourceName.isEmpty()) {
		throw("sourceName is empty");
	}

	OBSSourceAutoRelease source =
		obs_get_source_by_name(sourceName.toUtf8());
	if (!source) {
		throw("specified source doesn't exist");
	}

	obs_source_set_muted(source, mute);
}

/**
 * Toggle streaming on or off.
 */
void OBSController::StartStopStreaming()
{
	if (obs_frontend_streaming_active())
		StopStreaming();
	else
		StartStreaming();
}

/**
 * Start streaming.
 */
void OBSController::StartStreaming()
{
	if (obs_frontend_streaming_active() == false) {
		obs_frontend_streaming_start();
	}
}

/**
 * Stop streaming.
 */
void OBSController::StopStreaming()
{
	if (obs_frontend_streaming_active() == true) {
		obs_frontend_streaming_stop();
	}
}

/**
 * Toggle recording on or off.
 */
void OBSController::StartStopRecording()
{
	(obs_frontend_recording_active() ? obs_frontend_recording_stop()
					 : obs_frontend_recording_start());
}

/**
 * Start recording.
 */
void OBSController::StartRecording()
{
	if (!obs_frontend_recording_active()) {
		obs_frontend_recording_start();
	}
}

/**
 * Stop recording.
 */
void OBSController::StopRecording()
{
	if (obs_frontend_recording_active()) {
		obs_frontend_recording_stop();
	}
}

/**
* Pause the current recording.
*/
void OBSController::PauseRecording()
{
	if (obs_frontend_recording_active()) {
		obs_frontend_recording_pause(true);
	}
}

/**
* Resume/unpause the current recording (if paused).
*/
void OBSController::ResumeRecording()
{
	if (obs_frontend_recording_active()) {
		obs_frontend_recording_pause(false);
	}
}

/**
* Toggle the Replay Buffer on/off.
*/
void OBSController::StartStopReplayBuffer()
{
	if (obs_frontend_replay_buffer_active()) {
		obs_frontend_replay_buffer_stop();
	} else {
		Utils::StartReplayBuffer();
	}
}

/**
* Start recording into the Replay Buffer.
* Will throw an error if "Save Replay Buffer" hotkey is not set in OBS' settings.
* Setting this hotkey is mandatory, even when triggering saves only
* through obs-midi.
*/
void OBSController::StartReplayBuffer()
{
	if (!Utils::ReplayBufferEnabled()) {
		throw("replay buffer disabled in settings");
	}

	if (obs_frontend_replay_buffer_active() == false) {
		Utils::StartReplayBuffer();
	}
}

/**
* Stop recording into the Replay Buffer.
*/
void OBSController::StopReplayBuffer()
{
	if (obs_frontend_replay_buffer_active() == true) {
		obs_frontend_replay_buffer_stop();
	}
}

/**
* Flush and save the contents of the Replay Buffer to disk. This is
* basically the same as triggering the "Save Replay Buffer" hotkey.
* Will return an `error` if the Replay Buffer is not active.
*/
void OBSController::SaveReplayBuffer()
{
	if (!obs_frontend_replay_buffer_active()) {
		throw("replay buffer not active");
	}

	OBSOutputAutoRelease replayOutput =
		obs_frontend_get_replay_buffer_output();

	calldata_t cd = {0};
	proc_handler_t *ph = obs_output_get_proc_handler(replayOutput);
	proc_handler_call(ph, "save", &cd);
	calldata_free(&cd);
}

void OBSController::SetCurrentProfile(QString profileName)
{
	if (profileName.isEmpty()) {
		throw("profile name is empty");
	}

	// TODO : check if profile exists
	obs_frontend_set_current_profile(profileName.toUtf8());
}

void OBSController::SetTextGDIPlusText() {}

void OBSController::SetBrowserSourceURL() {}

void OBSController::ReloadBrowserSource() {}

void OBSController::TakeSourceScreenshot(QString source) {}

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

/**
 * Set the audio sync offset of a specified source.
 */
void OBSController::SetSyncOffset(QString sourceName, int64_t sourceSyncOffset)
{
	if (sourceName.isEmpty()) {
		throw("source name is empty");
	}

	OBSSourceAutoRelease source =
		obs_get_source_by_name(sourceName.toUtf8());
	if (!source) {
		throw("specified source doesn't exist");
	}

	obs_source_set_sync_offset(source, sourceSyncOffset);
}

void OBSController::SetSourcePosition() {}

void OBSController::SetSourceRotation() {}

void OBSController::SetSourceScale() {}

void OBSController::SetGainFilter() {}

void OBSController::SetOpacity() {}
