#include "actions.h"
#include <iostream>

#include "utils.h"
#include "midi-agent.h"
QString ActionsClass::action_to_string(const ActionsClass::Actions &enumval)
{
	return QVariant::fromValue(enumval).toString();
}

ActionsClass::Actions ActionsClass::string_to_action(const QString &action)
{
	return QVariant(action).value<ActionsClass::Actions>();
}
QString
ActionsClass::event_to_string(const ActionsClass::obs_event_type &enumval)
{
	return QVariant::fromValue(enumval).toString();
}

ActionsClass::obs_event_type
ActionsClass::string_to_event(const QString &action)
{
	return QVariant(action).value<ActionsClass::obs_event_type>();
}

/**
 * Sets the currently active scene
 */
void ActionsClass::SetCurrentScene(QString sceneName)
{
	OBSSourceAutoRelease source =
		obs_get_source_by_name(sceneName.toStdString().c_str());

	if (source) {
		obs_frontend_set_current_scene(source);
	} else {
		throw("requested scene does not exist");
	}
}

/**
 * Sets the scene in preview. Must be in Studio mode or will throw error
 */
void ActionsClass::SetPreviewScene(QString sceneName)
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
void ActionsClass::SetCurrentSceneCollection(QString sceneCollection)
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
void ActionsClass::ResetSceneItem(QString sceneName, QString itemName)
{
	OBSScene scene = Utils::GetSceneFromNameOrCurrent(sceneName);
	if (!scene) {
		throw("requested scene doesn't exist");
	}

	obs_data_t *params = obs_data_create();
	obs_data_set_string(params, "scene-name",
			    sceneName.toStdString().c_str());
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
void ActionsClass::TransitionToProgram()
{
	obs_frontend_preview_program_trigger_transition();
}

/**
 * Transitions the currently previewed scene to the main output using specified transition.
 * transitionDuration is optional. (milliseconds)
 */
void ActionsClass::TransitionToProgram(QString transitionName,
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
void ActionsClass::SetCurrentTransition(QString name)
{
	bool success = Utils::SetTransitionByName(name);
	if (!success) {
		throw("requested transition does not exist");
	}
}

/**
 * Set the duration of the currently active transition
 */
void ActionsClass::SetTransitionDuration(int duration)
{
	obs_frontend_set_transition_duration(duration);
}

void ActionsClass::SetSourceVisibility() {} //DOESNT EXIST

void ActionsClass::ToggleSourceVisibility() {} //DOESNT EXIST

/**
* Inverts the mute status of a specified source.
*/
void ActionsClass::ToggleMute(QString sourceName)
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
void ActionsClass::SetMute(QString sourceName, bool mute)
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
void ActionsClass::StartStopStreaming()
{
	if (obs_frontend_streaming_active())
		StopStreaming();
	else
		StartStreaming();
}

/**
 * Start streaming.
 */
void ActionsClass::StartStreaming()
{
	if (obs_frontend_streaming_active() == false) {
		obs_frontend_streaming_start();
	}
}

/**
 * Stop streaming.
 */
void ActionsClass::StopStreaming()
{
	if (obs_frontend_streaming_active() == true) {
		obs_frontend_streaming_stop();
	}
}

/**
 * Toggle recording on or off.
 */
void ActionsClass::StartStopRecording()
{
	(obs_frontend_recording_active() ? obs_frontend_recording_stop()
					 : obs_frontend_recording_start());
}

/**
 * Start recording.
 */
void ActionsClass::StartRecording()
{
	if (!obs_frontend_recording_active()) {
		obs_frontend_recording_start();
	}
}

/**
 * Stop recording.
 */
void ActionsClass::StopRecording()
{
	if (obs_frontend_recording_active()) {
		obs_frontend_recording_stop();
	}
}

/**
* Pause the current recording.
*/
void ActionsClass::PauseRecording()
{
	if (obs_frontend_recording_active()) {
		obs_frontend_recording_pause(true);
	}
}

/**
* Resume/unpause the current recording (if paused).
*/
void ActionsClass::ResumeRecording()
{
	if (obs_frontend_recording_active()) {
		obs_frontend_recording_pause(false);
	}
}

/**
* Toggle the Replay Buffer on/off.
*/
void ActionsClass::StartStopReplayBuffer()
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
void ActionsClass::StartReplayBuffer()
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
void ActionsClass::StopReplayBuffer()
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
void ActionsClass::SaveReplayBuffer()
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

void ActionsClass::SetCurrentProfile(QString profileName)
{
	if (profileName.isEmpty()) {
		throw("profile name is empty");
	}

	// TODO : check if profile exists
	obs_frontend_set_current_profile(profileName.toUtf8());
}

void ActionsClass::SetTextGDIPlusText(QString text) {}

void ActionsClass::SetBrowserSourceURL(QString url) {}

void ActionsClass::ReloadBrowserSource() {}

void ActionsClass::TakeSourceScreenshot(QString source)
{
	obs_frontend_take_source_screenshot(
		obs_get_source_by_name(source.toStdString().c_str()));
}

void ActionsClass::EnableSourceFilter() {}

void ActionsClass::DisableSourceFilter() {}

void ActionsClass::ToggleSourceFilter() {}

////////////////
// CC ACTIONS //
////////////////

void ActionsClass::SetVolume(QString source, float volume)
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
void ActionsClass::SetSyncOffset(QString sourceName, int64_t sourceSyncOffset)
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

void ActionsClass::SetSourcePosition() {}

void ActionsClass::SetSourceRotation() {}

void ActionsClass::SetSourceScale() {}

void ActionsClass::SetGainFilter() {}

void ActionsClass::SetOpacity() {}

void ActionsClass::do_obs_action(MidiHook *hook, int MidiVal,
				 ActionsClass::Actions action)
{
	switch (action) {
	case ActionsClass::Actions::Set_Current_Scene:
		ActionsClass::SetCurrentScene(hook->scene);
		break;
	case ActionsClass::Actions::Reset_Scene_Item:
		ActionsClass::ResetSceneItem(hook->scene, hook->item);
		break;
	case ActionsClass::Actions::Toggle_Mute:
		ActionsClass::ToggleMute(hook->audio_source);
		break;
	case ActionsClass::Actions::Do_Transition:
		if (hook->transition.isEmpty()) {
			ActionsClass::TransitionToProgram();
		} else if (hook->duration != -1) {
			ActionsClass::TransitionToProgram(hook->transition,
							  hook->duration);
		} else {
			ActionsClass::TransitionToProgram(hook->transition);
		}
		break;
	case ActionsClass::Actions::Set_Current_Transition:
		ActionsClass::SetCurrentTransition(hook->transition);
		break;
	case ActionsClass::Actions::Set_Mute:
		ActionsClass::SetMute(hook->audio_source, hook->bool_override);
		break;
	case ActionsClass::Actions::Toggle_Start_Stop_Streaming:
		ActionsClass::StartStopStreaming();
		break;
	case ActionsClass::Actions::Set_Preview_Scene:
		ActionsClass::SetPreviewScene(hook->scene);
		break;
	case ActionsClass::Actions::Set_Current_Scene_Collection:
		ActionsClass::SetCurrentSceneCollection(hook->scene_collection);
		break;
	case ActionsClass::Actions::Set_Transition_Duration:
		if (hook->duration != -1) {
			ActionsClass::SetTransitionDuration(hook->duration);
		} else {
			ActionsClass::SetTransitionDuration(MidiVal);
		}
		break;
	case ActionsClass::Actions::Start_Streaming:
		ActionsClass::StartStreaming();
		break;
	case ActionsClass::Actions::Stop_Streaming:
		ActionsClass::StopStreaming();
		break;
	case ActionsClass::Actions::Start_Recording:
		ActionsClass::StartRecording();
		break;
	case ActionsClass::Actions::Stop_Recording:
		ActionsClass::StopRecording();
		break;
	case ActionsClass::Actions::Start_Replay_Buffer:
		ActionsClass::StartReplayBuffer();
		break;
	case ActionsClass::Actions::Stop_Replay_Buffer:
		ActionsClass::StopReplayBuffer();
		break;
	case ActionsClass::Actions::Set_Volume:
		ActionsClass::SetVolume(hook->audio_source,
					pow(Utils::mapper(MidiVal), 3.0));
		break;
	case ActionsClass::Actions::Take_Source_Screenshot:
		ActionsClass::TakeSourceScreenshot(hook->source);
		break;
	case ActionsClass::Actions::Pause_Recording:
		ActionsClass::PauseRecording();
		break;
	case ActionsClass::Actions::Enable_Source_Filter:
		ActionsClass::EnableSourceFilter();
		break;
	case ActionsClass::Actions::Disable_Source_Filter:
		ActionsClass::DisableSourceFilter();
		break;
	case ActionsClass::Actions::Toggle_Start_Stop_Recording:
		ActionsClass::StartStopRecording();
		break;
	case ActionsClass::Actions::Toggle_Start_Stop_Replay_Buffer:
		ActionsClass::StartStopReplayBuffer();
		break;
	case ActionsClass::Actions::Resume_Recording:
		ActionsClass::ResumeRecording();
		break;
	case ActionsClass::Actions::Save_Replay_Buffer:
		ActionsClass::SaveReplayBuffer();
		break;
	case ActionsClass::Actions::Set_Current_Profile:
		ActionsClass::SetCurrentProfile(hook->profile);
		break;
	case ActionsClass::Actions::Toggle_Source_Filter:
		ActionsClass::ToggleSourceFilter();
		break;
	case ActionsClass::Actions::Set_Text_GDIPlus_Text:
		ActionsClass::SetTextGDIPlusText(hook->string_override);
		break;
	case ActionsClass::Actions::Set_Browser_Source_URL:
		ActionsClass::SetBrowserSourceURL(hook->string_override);
		break;
	case ActionsClass::Actions::Reload_Browser_Source:
		ActionsClass::ReloadBrowserSource();
		break;
	case ActionsClass::Actions::Set_Sync_Offset:
		ActionsClass::SetSyncOffset(hook->media_source,
					    (int64_t)MidiVal);
		break;
	case ActionsClass::Actions::Set_Source_Rotation:
		ActionsClass::SetSourceRotation();
		break;
	case ActionsClass::Actions::Set_Source_Position:
		ActionsClass::SetSourcePosition();
		break;
	case ActionsClass::Actions::Set_Gain_Filter:
		ActionsClass::SetGainFilter();
		break;
	case ActionsClass::Actions::Set_Opacity:
		ActionsClass::SetOpacity();
		break;
	case ActionsClass::Actions::Set_Source_Scale:
		ActionsClass::SetSourceScale();
		break;
	};
}
