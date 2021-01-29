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

#pragma once
#if __has_include(<obs-frontend-api.h>)
#include "rtmidi17/rtmidi17.hpp"
#else
#include "RtMidi17/rtmidi17.hpp"
#endif
#include <stdio.h>
#include <iostream>
#include <vector>

#include <QtCore/QString>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QLayout>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QSystemTrayIcon>

#include <obs.hpp>
#include <obs-module.h>
#include <util/config-file.h>
#include "obs-midi.h"

typedef void (*PauseRecordingFunction)(bool);
typedef bool (*RecordingPausedFunction)();
typedef struct MidiMessage {
	QString device_name;
	QString message_type;
	int channel;
	int NORC;
	int value;
} MidiMessage;
Q_DECLARE_METATYPE(MidiMessage);
enum pairs { Scene, Source, Item, Transition, Audio, Media, Filter };
enum class Actions{
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
		 Set_Current_Transition,
		 Set_Gain_Filter,
		 Set_Media_Time,
		 Set_Mute,
		 Set_Scene_Item_Crop,
		 Set_Scene_Item_Position,
		 Set_Scene_Item_Render,
		 Set_Scene_Item_Transform,
		 Set_Scene_Transition_Override,
		 Set_Source_Filter_Visibility,
		 Set_Source_Name,
		 Set_Source_Settings,
		 Set_Sync_Offset,
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
		 Do_Transition,
		 Unpause_Recording
};
typedef void (*PauseRecordingFunction)(bool);
typedef bool (*RecordingPausedFunction)();
namespace Utils {
class OBSActionsWidget;
float mapper(int x);
int mapper2(double x);
bool is_number(const QString &s);
bool isJSon(QString val);

QString getMidiMessageType(int in);
QStringList GetMediaSourceNames();
QStringList GetAudioSourceNames();

QString nsToTimestamp(uint64_t ns);
obs_data_array_t *StringListToArray(char **strings, const char *key);
obs_data_array_t *GetSceneItems(obs_source_t *source);
obs_data_t *GetSceneItemData(obs_sceneitem_t *item);
OBSDataArrayAutoRelease GetSourceArray();
OBSDataArrayAutoRelease GetSceneArray(QString name = NULL);
// These functions support nested lookup into groups
obs_sceneitem_t *GetSceneItemFromName(obs_scene_t *scene, QString name);
obs_sceneitem_t *GetSceneItemFromId(obs_scene_t *scene, int64_t id);
obs_sceneitem_t *GetSceneItemFromItem(obs_scene_t *scene, obs_data_t *item);
obs_sceneitem_t *GetSceneItemFromRequestField(obs_scene_t *scene,
					      obs_data_item_t *dataItem);

obs_scene_t *GetSceneFromNameOrCurrent(QString sceneName);
obs_data_t *GetSceneItemPropertiesData(obs_sceneitem_t *item);

obs_data_t *GetSourceFilterInfo(obs_source_t *filter, bool includeSettings);
obs_data_array_t *GetSourceFiltersList(obs_source_t *source,
				       bool includeSettings);

bool IsValidAlignment(const uint32_t alignment);

obs_data_array_t *GetScenes();
obs_data_t *GetSceneData(obs_source_t *source);

// TODO contribute a proper frontend API method for this to OBS and remove this hack
int GetTransitionDuration(obs_source_t *transition);
obs_source_t *GetTransitionFromName(QString transitionName);
bool SetTransitionByName(QString transitionName);
obs_data_t *GetTransitionData(obs_source_t *transition);

QString OBSVersionString();

const char *GetRecordingFolder();
bool SetRecordingFolder(const char *path);

QString ParseDataToQueryString(obs_data_t *data);
obs_hotkey_t *FindHotkeyByName(QString name);

bool ReplayBufferEnabled();
void StartReplayBuffer();
bool IsRPHotkeySet();
const char *GetFilenameFormatting();
bool SetFilenameFormatting(const char *filenameFormatting);
bool inrange(int low, int high, int x);
QStringList GetTransitionsList();
QStringList GetSceneItemsList(QString scene);
bool inrange(int low, int high, int x);
QString mtype_to_string(rtmidi::message_type);

int get_midi_note_or_control(rtmidi::message mess);
int get_midi_value(rtmidi::message mess);
QSpinBox *GetTransitionDurationControl();
QString action_to_string(Actions action);
Actions string_to_action(QString string);
};
