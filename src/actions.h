#pragma once
#include <QtCore/QString>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QLayout>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QSystemTrayIcon>
#include <obs.hpp>
#include <obs-module.h>
#include <iostream>

#include "utils.h"
#include "midi-agent.h"
#include "obs-midi.h"

class ActionsClass : public QObject {
	Q_OBJECT
public:
	enum class Actions {
		Disable_Preview,
		Disable_Source_Filter,
		Enable_Preview,
		Enable_Source_Filter,
		Next_Media,
		Pause_Recording,
		Play_Pause_Media,
		Previous_Media,
		Reset_Scene_Item,
		Reset_Stats,
		Restart_Media,
		Scrub_Media,
		Set_Audio_Monitor_Type,
		Set_Current_Scene,
		Set_Current_Scene_Collection,
		Set_Current_Transition,
		Set_Gain_Filter,
		Set_Media_Time,
		Set_Mute,
		Set_Preview_Scene,
		Set_Scene_Item_Crop,
		Set_Scene_Item_Position,
		Set_Scene_Item_Render,
		Set_Scene_Item_Transform,
		Set_Scene_Transition_Override,
		Set_Source_Filter_Visibility,
		Set_Source_Name,
		Set_Source_Settings,
		Set_Sync_Offset,
		Set_Transition_Duration,
		Set_Volume,
		Start_Recording,
		Start_Replay_Buffer,
		Start_Streaming,
		Stop_Media,
		Stop_Recording,
		Stop_Replay_Buffer,
		Stop_Streaming,
		Studio_Mode,
		Take_Source_Screenshot,
		Toggle_Mute,
		Toggle_Source_Filter,
		Toggle_Start_Stop_Streaming,
		Toggle_Start_Stop_Recording,
		Toggle_Start_Stop_Replay_Buffer,
		Do_Transition,
		Unpause_Recording,
		Resume_Recording,
		Save_Replay_Buffer,
		Set_Current_Profile,
		Set_Text_GDIPlus_Text,
		Set_Browser_Source_URL,
		Reload_Browser_Source,
		Set_Source_Scale,
		Set_Source_Rotation,
		Set_Source_Position,
		Set_Opacity
	};
	Q_ENUM(Actions)
	enum class obs_event_type {
		SourceVolumeChanged,
		SwitchScenes,
		TransitionBegin,
		TransitionEnd,
		SourceMuteStateChanged,
		SourceRenamed,
		Exiting,
		SourceDestroyed,
	};
	QList<Actions> AllActions_raw = {Actions::Disable_Preview,
					 Actions::Disable_Source_Filter,
					 Actions::Enable_Preview,
					 Actions::Enable_Source_Filter,
					 Actions::Next_Media,
					 Actions::Pause_Recording,
					 Actions::Play_Pause_Media,
					 Actions::Previous_Media,
					 Actions::Reset_Scene_Item,
					 Actions::Reset_Stats,
					 Actions::Restart_Media,
					 Actions::Set_Audio_Monitor_Type,
					 Actions::Set_Current_Scene,
					 Actions::Set_Current_Transition,
					 Actions::Set_Gain_Filter,
					 Actions::Set_Media_Time,
					 Actions::Set_Mute,
					 Actions::Set_Scene_Item_Crop,
					 Actions::Set_Scene_Item_Position,
					 Actions::Set_Scene_Item_Render,
					 Actions::Set_Scene_Item_Transform,
					 Actions::Set_Scene_Transition_Override,
					 Actions::Set_Source_Filter_Visibility,
					 Actions::Set_Source_Name,
					 Actions::Set_Source_Settings,
					 Actions::Set_Sync_Offset,
					 Actions::Set_Volume,
					 Actions::Start_Recording,
					 Actions::Start_Replay_Buffer,
					 Actions::Start_Streaming,
					 Actions::Stop_Media,
					 Actions::Stop_Recording,
					 Actions::Stop_Replay_Buffer,
					 Actions::Stop_Streaming,
					 Actions::Studio_Mode,
					 Actions::Take_Source_Screenshot,
					 Actions::Toggle_Mute,
					 Actions::Toggle_Source_Filter,
					 Actions::Toggle_Start_Stop_Streaming,
					 Actions::Do_Transition,
					 Actions::Unpause_Recording};
	Q_ENUM(obs_event_type)

	static QString action_to_string(const Actions &enumval);
	static Actions string_to_action(const QString &string);

	static QString event_to_string(const obs_event_type &enumval);
	static obs_event_type string_to_event(const QString &string);

	void SetCurrentScene(QString sceneName);
	void SetPreviewScene(QString sceneName);
	void SetCurrentSceneCollection(QString sceneCollection);
	void ResetSceneItem(QString sceneName, QString itemName);
	void TransitionToProgram();
	void TransitionToProgram(QString transitionName,
				 int transitionDuration = 300);
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
	void SetTextGDIPlusText(QString text);
	void SetBrowserSourceURL(QString url);
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

	void do_obs_action(MidiHook *hook, int MidiVal,
			   ActionsClass::Actions action);
};
