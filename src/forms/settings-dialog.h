/*
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

#include <vector>
#include <QtWidgets/QDialog>
#include "ui_settings-dialog.h"
#include "../midi-agent.h"
#include "../version.h"

class PluginWindow : public QDialog {
	Q_OBJECT

public:
	PluginWindow(QWidget *parent);
	~PluginWindow();
	void setCheck(bool check);
	void SetAvailableDevices();
	int on_check_enabled_stateChanged(bool state);
	void on_item_select(QString curitem);
	int on_bid_enabled_stateChanged(bool state);


	
	
signals:
	void changed(obs_data_t *change);
private Q_SLOTS:
	void ToggleShowHide();

public slots:
	void selectOutput(QString item);
	void domessage(MidiMessage mess);
	
	void obs_actions_select(QString action);
	void check_advanced_switch(bool state);
	void edit_action(obs_data_t *TriggerType);
	void ResetToDefaults();
	void onChange(QString string);
	void on_source_change(QString source);
	void on_scene_change(QString source);
	bool MapCall(QString plugin, obs_data_t *map);
	void get_sources(QString scene);
	void obs_actions_filter_select(int);
	void get_filters(QString Source);
	void get_transitions();
	void get_scenes();
	

private:
	Ui::PluginWindow *ui;

	bool hidedebugitems = true;
	bool loadingdevices = false;
	QMetaObject::Connection desconnect;
	bool starting = true;
	QString DeviceFilter;
	void ShowPair(pairs);
	void HidePair(pairs);
	void HideAllPairs();
	void add_midi_device(QString Name);
	void set_headers();
	QStringList SceneList;
	bool listening = false;
	void get_scene_names();

	
private:
	QGridLayout *layout;
	
	
	QStringList *items;
	QString sceneName;
	bool switching = false;
	bool DoMap(obs_data_t *map);
	QStringList TranslateActions();
	void ShowIntActions();
	void ShowStringActions();
	void ShowBoolActions();
	void ShowOnly(QList<Actions> shows);
	void ShowEntry(Actions Entry);
	void HideEntry(Actions Entry);
	void ShowAllActions();
	void HideAdvancedActions();
	void HideEntries(QList<Actions> entrys);
	void ShowEntries(QList<Actions> entrys);
	QString FirstVisible();
	QString untranslate(QString translation);
	void ShowPair(QString pair);
	void HidePair(QString pair);
	QListView *listview;
	QList<Actions> FrontendActions = {Actions::Start_Streaming,
				   Actions::Stop_Streaming,
				   Actions::Toggle_Start_Stop_Streaming,
				   Actions::Start_Recording,
				   Actions::Stop_Recording,
				   Actions::Pause_Recording,
				   Actions::Unpause_Recording,
				   Actions::Start_Replay_Buffer,
				   Actions::Stop_Replay_Buffer,
				   Actions::Enable_Preview,
				   Actions::Disable_Preview,
				   Actions::Studio_Mode,
				   Actions::Do_Transition,
				   Actions::Reset_Stats};

	QList<Actions> filterActions = {Actions::Enable_Source_Filter,
				     Actions::Disable_Source_Filter,
				     Actions::Toggle_Source_Filter};

	QList<Actions> sceneActions = {Actions::Reset_Scene_Item,
				    Actions::Set_Current_Scene,
				    Actions::Set_Scene_Transition_Override,
				    Actions::Set_Current_Transition};

	QList<Actions> sourceActions = {Actions::Toggle_Mute,
				 Actions::Take_Source_Screenshot};

	QList<Actions> mediaActions = {Actions::Play_Pause_Media,
				    Actions::Restart_Media,
				    Actions::Stop_Media,
				    Actions::Next_Media,
				    Actions::Previous_Media};

	QList<Actions> AllActions = {};

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

	QList<Actions> AdvancedSourceActions = {
		Actions::Set_Mute,
					 Actions::Set_Source_Name,
					 Actions::Set_Sync_Offset,
					 Actions::Set_Source_Settings,
					 Actions::Set_Source_Filter_Visibility,
					 Actions::Set_Audio_Monitor_Type};
	QList<Actions> AdvancedMediaActions = {Actions::Scrub_Media};

	QList < Actions> AdvancedFilterActions = {
		Actions::Set_Gain_Filter};
	QList<Actions> AdvancedSceneActions = {Actions::Set_Scene_Item_Render,
					    Actions::Set_Scene_Item_Position,
					    Actions::Set_Scene_Item_Transform,
					    Actions::Set_Scene_Item_Crop};

	QStringList intActions = {
		"Set Volume",
		"Set Media Time",
	};



};
