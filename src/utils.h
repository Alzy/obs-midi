/*
obs-websocket
Copyright (C) 2016-2019	Stéphane Lepin <stephane.lepin@gmail.com>
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
#include <QtCore/QString>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QLayout>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QSystemTrayIcon>
#include <QtWidgets/QMessageBox>

#include <cstdio>
#include <iostream>
#include <vector>
#include <obs.hpp>
#include <obs-module.h>
#include <util/config-file.h>
#pragma once
#if __has_include(<obs-frontend-api.h>)
#include "rtmidi17/rtmidi17.hpp"
#else
#include "rtmidi17/rtmidi17.hpp"
#endif

#include "obs-midi.h"

typedef void (*PauseRecordingFunction)(bool);
typedef bool (*RecordingPausedFunction)();

enum class Pairs { Scene, Source, Item, Transition, Audio, Media, Filter, String, Integer, Boolean };
enum class Speed {Slow, Medium, Fast};
class ActionsClass : public QObject {
	Q_OBJECT
public:
	enum class Actions {
		Disable_Preview,
		Disable_Source_Filter,
		Do_Transition,
		Enable_Preview,
		Enable_Source_Filter,
		Move_T_Bar,
		Next_Media,
		Pause_Recording,
		Play_Pause_Media,
		Previous_Media,
		Reload_Browser_Source,
		Reset_Scene_Item,
		Reset_Stats,
		Restart_Media,
		Resume_Recording,
		Save_Replay_Buffer,
		Scrub_Media,
		Set_Audio_Monitor_Type,
		Set_Browser_Source_URL,
		Set_Current_Profile,
		Set_Current_Scene,
		Set_Current_Scene_Collection,
		Set_Current_Transition,
		Set_Gain_Filter,
		Set_Media_Time,
		Set_Mute,
		Set_Opacity,
		Set_Preview_Scene,
		Set_Scene_Item_Crop,
		Set_Scene_Item_Position,
		Set_Scene_Item_Render,
		Set_Scene_Item_Transform,
		Set_Scene_Transition_Override,
		Set_Source_Filter_Visibility,
		Set_Source_Name,
		Set_Source_Position,
		Set_Source_Rotation,
		Set_Source_Scale,
		Set_Source_Settings,
		Set_Sync_Offset,
		Set_Text_GDIPlus_Text,
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
		Take_Screenshot,
		Take_Source_Screenshot,
		Toggle_Mute,
		Toggle_Source_Filter,
		Toggle_Source_Visibility,
		Toggle_Start_Stop_Recording,
		Toggle_Start_Stop_Replay_Buffer,
		Toggle_Start_Stop_Streaming,
		Unpause_Recording
	};
	Q_ENUM(Actions)
	enum class obs_event_type {
		SourceDestroyed,
		SceneChanged,
		SceneListChanged,
		SceneCollectionChanged,
		SceneCollectionListChanged,
		TransitionChange,
		TransitionListChanged,
		ProfileChanged,
		ProfileListChanged,
		StreamStarting,
		StreamStarted,
		StreamStopping,
		StreamStopped,
		RecordingStarting,
		RecordingStarted,
		RecordingStopping,
		RecordingStopped,
		RecordingPaused,
		RecordingResumed,
		ReplayStarting,
		ReplayStarted,
		ReplayStopping,
		ReplayStopped,
		StudioModeSwitched,
		PreviewSceneChanged,
		Exiting,
		FrontendEventHandler,
		TransitionBegin,
		TransitionEnd,
		TransitionVideoEnd,
		SourceCreated,
		SourceDestroy,
		SourceVolumeChanged,
		SourceMuteStateChanged,
		SourceAudioSyncOffsetChanged,
		SourceAudioMixersChanged,
		SourceRenamed,
		SourceFilterAdded,
		SourceFilterRemoved,
		SourceFilterVisibilityChanged,
		SourceFilterOrderChanged,
		SceneReordered,
		SceneItemAdd,
		SceneItemDeleted,
		SceneItemVisibilityChanged,
		SceneItemLockChanged,
		SceneItemTransform,
		SceneItemSelected,
		SceneItemDeselected,
		SwitchScenes
	};
	Q_ENUM(obs_event_type)
	static QString action_to_string(const Actions &enumval);
	static Actions string_to_action(const QString &string);
	static QString event_to_string(const obs_event_type &enumval);
	static obs_event_type string_to_event(const QString &string);
};
typedef void (*PauseRecordingFunction)(bool);
typedef bool (*RecordingPausedFunction)();
namespace Utils {
class OBSActionsWidget;
float mapper(int x);
int mapper2(double x);
int t_bar_mapper(int x);
bool is_number(const QString &s);
bool isJSon(const QString &val);
QString get_midi_message_type(const rtmidi::message &message);
QStringList GetMediaSourceNames();
QStringList GetAudioSourceNames();
QString nsToTimestamp(uint64_t ns);
obs_data_array_t *StringListToArray(char **strings, const char *key);
QString GetSceneItems(obs_source_t *source);
QStringList GetSceneItemsBySource(obs_source_t *source);
obs_data_t *GetSceneItemData(obs_sceneitem_t *item);
QString GetSourceArray();
QString GetSceneArray(const QString &name = "");
// These functions support nested lookup into groups
obs_sceneitem_t *GetSceneItemFromName(obs_scene_t *scene, const QString &name);
obs_sceneitem_t *GetSceneItemFromId(obs_scene_t *scene, int64_t id);
obs_sceneitem_t *GetSceneItemFromItem(obs_scene_t *scene, obs_data_t *item);
obs_sceneitem_t *GetSceneItemFromRequestField(obs_scene_t *scene, obs_data_item_t *dataItem);
obs_scene_t *GetSceneFromNameOrCurrent(const QString &sceneName);
obs_data_t *GetSceneItemPropertiesData(obs_sceneitem_t *item);
obs_data_t *GetSourceFilterInfo(obs_source_t *filter, bool includeSettings);
obs_data_array_t *GetSourceFiltersList(obs_source_t *source, bool includeSettings);
bool IsValidAlignment(uint32_t alignment);
obs_data_array_t *GetScenes();
QString GetSceneData(obs_source_t *source);
// TODO contribute a proper frontend API method for this to OBS and remove this hack
int GetTransitionDuration(obs_source_t *transition);
obs_source_t *GetTransitionFromName(const QString &transitionName);
bool SetTransitionByName(const QString &transitionName);
obs_data_t *GetTransitionData(obs_source_t *transition);
QString OBSVersionString();
const char *GetRecordingFolder();
bool SetRecordingFolder(const char *path);
QString ParseDataToQueryString(obs_data_t *data);
obs_hotkey_t *FindHotkeyByName(const QString &name);
bool ReplayBufferEnabled();
void StartReplayBuffer();
bool IsRPHotkeySet();
const char *GetFilenameFormatting();
bool SetFilenameFormatting(const char *filenameFormatting);
bool inrange(int low, int high, int x);
QStringList GetTransitionsList();
QStringList GetSceneItemsList(const QString &scene);
bool inrange(int low, int high, int x);
QString mtype_to_string(rtmidi::message_type);
int get_midi_note_or_control(const rtmidi::message &mess);
int get_midi_value(const rtmidi::message &mess);
QSpinBox *GetTransitionDurationControl();
QStringList TranslateActions();
QStringList get_scene_names();
QStringList get_source_names(const QString &scene);
QStringList get_filter_names(const QString &Source);
QStringList get_transition_names();
QString untranslate(const QString &tstring);
QStringList get_browser_sources();
const QList<ActionsClass::Actions> AllActions_raw = {ActionsClass::Actions::Disable_Preview,
						     ActionsClass::Actions::Disable_Source_Filter,
						     ActionsClass::Actions::Do_Transition,
						     ActionsClass::Actions::Enable_Preview,
						     ActionsClass::Actions::Enable_Source_Filter,
						     ActionsClass::Actions::Move_T_Bar,
						     ActionsClass::Actions::Next_Media,
						     ActionsClass::Actions::Pause_Recording,
						     ActionsClass::Actions::Play_Pause_Media,
						     ActionsClass::Actions::Previous_Media,
						     ActionsClass::Actions::Reload_Browser_Source,
						     ActionsClass::Actions::Reset_Scene_Item,
						     ActionsClass::Actions::Restart_Media,
						     ActionsClass::Actions::Resume_Recording,
						     ActionsClass::Actions::Save_Replay_Buffer,
						     ActionsClass::Actions::Set_Current_Scene,
						     ActionsClass::Actions::Set_Current_Transition,
						     ActionsClass::Actions::Set_Preview_Scene,
						     ActionsClass::Actions::Set_Scene_Transition_Override,
						     ActionsClass::Actions::Set_Volume,
						     ActionsClass::Actions::Start_Recording,
						     ActionsClass::Actions::Start_Replay_Buffer,
						     ActionsClass::Actions::Start_Streaming,
						     ActionsClass::Actions::Stop_Media,
						     ActionsClass::Actions::Stop_Recording,
						     ActionsClass::Actions::Stop_Replay_Buffer,
						     ActionsClass::Actions::Stop_Streaming,
						     ActionsClass::Actions::Studio_Mode,
						     ActionsClass::Actions::Take_Screenshot,
						     ActionsClass::Actions::Take_Source_Screenshot,
						     ActionsClass::Actions::Toggle_Mute,
						     ActionsClass::Actions::Toggle_Source_Filter,
						     ActionsClass::Actions::Toggle_Source_Visibility,
						     ActionsClass::Actions::Toggle_Start_Stop_Recording,
						     ActionsClass::Actions::Toggle_Start_Stop_Replay_Buffer,
						     ActionsClass::Actions::Toggle_Start_Stop_Streaming,
						     ActionsClass::Actions::Unpause_Recording};
const QList<ActionsClass::Actions> not_ready_actions{
	ActionsClass::Actions::Set_Current_Scene_Collection,
	ActionsClass::Actions::Reset_Stats,
	ActionsClass::Actions::Set_Current_Profile,
	ActionsClass::Actions::Set_Source_Scale,
	ActionsClass::Actions::Set_Source_Rotation,
	ActionsClass::Actions::Set_Source_Position,
	ActionsClass::Actions::Set_Source_Filter_Visibility,
	ActionsClass::Actions::Set_Source_Name,
	ActionsClass::Actions::Set_Transition_Duration,
	ActionsClass::Actions::Set_Gain_Filter,
	ActionsClass::Actions::Set_Media_Time,
	ActionsClass::Actions::Set_Source_Settings,
	ActionsClass::Actions::Set_Sync_Offset,
	ActionsClass::Actions::Set_Mute,
	ActionsClass::Actions::Scrub_Media,
	ActionsClass::Actions::Set_Audio_Monitor_Type,
	ActionsClass::Actions::Set_Scene_Item_Crop,
	ActionsClass::Actions::Set_Scene_Item_Position,
	ActionsClass::Actions::Set_Scene_Item_Render,
	ActionsClass::Actions::Set_Scene_Item_Transform,
	ActionsClass::Actions::Set_Text_GDIPlus_Text,
	ActionsClass::Actions::Set_Opacity,
	ActionsClass::Actions::Set_Browser_Source_URL,
};
void alert_popup(const QString &message);
QString translate_action(ActionsClass::Actions action);
};
/*Midi Message Structure*/
typedef struct MidiMessage {
public:
	MidiMessage() = default;
	void set_message(const rtmidi::message &message)
	{
		this->channel = message.get_channel();
		this->message_type = Utils::get_midi_message_type(message);
		this->NORC = Utils::get_midi_note_or_control(message);
		this->value = Utils::get_midi_value(message);
	}
	QString device_name;
	QString message_type = "none";
	int channel = 0;
	int NORC = 0;
	int value = 0;
	inline bool isNote() { return (message_type == "Note On" || message_type == "Note Off") ? true : false; };
	MidiMessage get() { return (MidiMessage) * this; }
} MidiMessage;
Q_DECLARE_METATYPE(MidiMessage);

/*
 * Midi Hook Class
 */
class MidiHook : public QObject {
	Q_OBJECT
public:
	int channel = -1;     // midi channel
	QString message_type; // Message Type
	int norc = -1;        // Note or Control
	QString action;
	QString scene;
	QString source;
	QString filter;
	QString transition;
	QString item;
	QString audio_source;
	QString media_source;
	int duration = -1;
	QString scene_collection;
	QString profile;
	QString string_override;
	std::optional<bool> bool_override;
	std::optional<int> int_override;
	int value = -1;
	MidiMessage *get_message_from_hook() const
	{
		MidiMessage *message = new MidiMessage();
		message->channel = this->channel;
		message->message_type = this->message_type;
		message->NORC = this->norc;
		return std::move(message);
	}
	MidiHook(){};
	MidiHook(const QString &jsonString)
	{
		obs_data_t *data = obs_data_create_from_json(jsonString.toStdString().c_str());
		channel = obs_data_get_int(data, "channel");
		message_type = obs_data_get_string(data, "message_type");
		norc = obs_data_get_int(data, "norc");
		action = obs_data_get_string(data, "action");
		scene = obs_data_get_string(data, "scene");
		source = obs_data_get_string(data, "source");
		filter = obs_data_get_string(data, "filter");
		transition = obs_data_get_string(data, "transition");
		item = obs_data_get_string(data, "item");
		audio_source = obs_data_get_string(data, "audio_source");
		media_source = obs_data_get_string(data, "media_source");
		duration = obs_data_get_int(data, "duration");
		scene_collection = obs_data_get_string(data, "scene_collection");
		profile = obs_data_get_string(data, "profile");
		string_override = obs_data_get_string(data, "string_override");
		bool_override.emplace(obs_data_get_bool(data, "bool_override"));
		int_override.emplace(obs_data_get_int(data, "int_override"));
	}

	QString GetData()
	{
		blog(LOG_DEBUG, "MH::GetData");
		obs_data_t *data = obs_data_create();
		obs_data_set_int(data, "channel", channel);
		obs_data_set_string(data, "message_type", message_type.toStdString().c_str());
		obs_data_set_int(data, "norc", norc);
		obs_data_set_string(data, "action", action.toStdString().c_str());
		if (!scene.isEmpty()) {
			obs_data_set_string(data, "scene", scene.toStdString().c_str());
		}
		if (!source.isEmpty()) {
			obs_data_set_string(data, "source", source.toStdString().c_str());
		}
		if (!filter.isEmpty()) {
			obs_data_set_string(data, "filter", filter.toStdString().c_str());
		}
		if (!transition.isEmpty()) {
			obs_data_set_string(data, "transition", transition.toStdString().c_str());
		}
		if (!item.isEmpty()) {
			obs_data_set_string(data, "item", item.toStdString().c_str());
		}
		if (!audio_source.isEmpty()) {
			obs_data_set_string(data, "audio_source", audio_source.toStdString().c_str());
		}
		if (!media_source.isEmpty()) {
			obs_data_set_string(data, "media_source", media_source.toStdString().c_str());
		}
		if (duration != -1) {
			obs_data_set_int(data, "duration", duration);
		}
		if (!scene_collection.isEmpty()) {
			obs_data_set_string(data, "scene_collection", scene_collection.toStdString().c_str());
		}
		if (!profile.isEmpty()) {
			obs_data_set_string(data, "profile", profile.toStdString().c_str());
		}
		if (!string_override.isEmpty()) {
			obs_data_set_string(data, "string_override", string_override.toStdString().c_str());
		}
		if (bool_override) {
			obs_data_set_bool(data, "bool_override", *bool_override);
		}
		if (int_override) {
			obs_data_set_int(data, "int_override", *int_override);
		}
		QString hookdata(obs_data_get_json(data));
		blog(LOG_DEBUG, "Midi Hook JSON = %s", hookdata.toStdString().c_str());
		obs_data_release(data);
		blog(LOG_DEBUG, "Midi Hook JSON post release = %s", hookdata.toStdString().c_str());

		return hookdata;
	}
};
