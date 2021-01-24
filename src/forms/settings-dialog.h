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



private Q_SLOTS:
	void ToggleShowHide();

public slots:
	void selectOutput(QString item);
	void domessage(MidiMessage mess);


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
	QStringList SL_filters = {};
	QStringList encoders = {};
	QStringList outputs = {};
	QStringList services = {};
	QStringList SL_scenes = {};
	QStringList SL_sources = {};

	QStringList GetScenes();
	QStringList *items;
	QString sceneName;
	bool switching = false;
	bool DoMap(obs_data_t *map);
	QStringList FrontendActions = {"Start Streaming",
				       "Stop Streaming",
				       "Toggle Start/Stop Streaming",
				       "Start Recording",
				       "Stop Recording",
				       "Pause Recording",
				       "Unpause Recording",
				       "Start Replay Buffer",
				       "Stop Replay Buffer",
				       "Enable Preview",
				       "Disable Preview",
				       "Studio Mode",
				       "Transition",
				       "Reset Stats"};

	QStringList filterActions = {"Enable Source Filter",
				     "Disable Source Filter",
				     "Toggle Source Filter"};

	QStringList sceneActions = {"Reset Scene Item", "Set Current Scene",
				    "Set Scene Transition Override",
				    "Set Current Transition"};

	QStringList sourceActions = {"Toggle Mute", "Take Source Screenshot"};

	QStringList mediaActions = {"Play/Pause Media", "Restart Media",
				    "Stop Media", "Next Media",
				    "Previous Media"};

	QStringList AllActions = {};

	QStringList AllActions_raw = {
		"control.action.Disable_Preview",
		"control.action.Disable_Source_Filter",
		"control.action.Enable_Preview",
		"control.action.Enable_Source_Filter",
		"control.action.Next_Media",
		"control.action.Pause_Recording",
		"control.action.Play_Pause_Media",
		"control.action.Previous_Media",
		"control.action.Reset_Scene_Item",
		"control.action.Reset_Stats",
		"control.action.Restart_Media",
		"control.action.Set_Audio_Monitor_Type",
		"control.action.Set_Current_Scene",
		"control.action.Set_Current_Transition",
		"control.action.Set_Gain_Filter",
		"control.action.Set_Media_Time",
		"control.action.Set_Mute",
		"control.action.Set_Scene_Item_Crop",
		"control.action.Set_Scene_Item_Position",
		"control.action.Set_Scene_Item_Render",
		"control.action.Set_Scene_Item_Transform",
		"control.action.Set_Scene_Transition_Override",
		"control.action.Set_Source_Filter_Visibility",
		"control.action.Set_Source_Name",
		"control.action.Set_Source_Settings",
		"control.action.Set_Sync_Offset",
		"control.action.Set_Volume",
		"control.action.Start_Recording",
		"control.action.Start_Replay_Buffer",
		"control.action.Start_Streaming",
		"control.action.Stop_Media",
		"control.action.Stop_Recording",
		"control.action.Stop_Replay_Buffer",
		"control.action.Stop_Streaming",
		"control.action.Studio_Mode",
		"control.action.Take_Source_Screenshot",
		"control.action.Toggle_Mute",
		"control.action.Toggle_Source_Filter",
		"control.action.Toggle_Start_Stop_Streaming",
		"control.action.Transition",
		"control.action.Unpause_Recording"};

	QStringList AdvancedSourceActions = {"Set Mute",
					     "Set Source Name",
					     "Set Sync Offset",
					     "Set Source Settings",
					     "Set Source Filter Visibility",
					     "Set Audio Monitor Type"};
	QStringList AdvancedMediaActions = {"Scrub Media"};

	QStringList AdvancedFilterActions = {"Set Gain Filter"};
	QStringList AdvancedSceneActions = {"Set Scene Item Render",
					    "Set Scene Item Position",
					    "Set Scene Item Transform",
					    "Set Scene Item Crop"};

	QStringList intActions = {
		"Set Volume",
		"Set Media Time",
	};
	void TranslateActions();
	void ShowIntActions();
	void ShowStringActions();
	void ShowBoolActions();
	void ShowOnly(QStringList shows);
	void ShowEntry(QString Entry);
	void HideEntry(QString Entry);
	void ShowAllActions();
	void HideAdvancedActions();
	void HideEntries(QStringList entrys);
	void ShowEntries(QStringList entrys);
	QString FirstVisible();
	QString untranslate(QString translation);
	void ShowPair(QString pair);
	void HidePair(QString pair);
	QListView *listview;

public slots:
	bool MapCall(QString plugin, obs_data_t *map);

private slots:
	QStringList GetSources(QString scene);
	void obs_actions_filter_select(int);
	QStringList GetFilters(QString Source);
	QStringList GetTransitions();
	void obs_actions_select(QString action);
	void check_advanced_switch(bool state);
	void edit_action(obs_data_t *TriggerType);
	void ResetToDefaults();
	void onChange();
	void on_source_change(QString source);
	void on_scene_change(QString source);
signals:
	void changed(obs_data_t *change);
};
