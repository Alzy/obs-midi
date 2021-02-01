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
	int on_check_enabled_state_changed(bool state);
	void on_device_select(QString curitem);
	int on_bid_enabled_state_changed(bool state);

signals:
	void changed(obs_data_t *change);
private Q_SLOTS:
	void ToggleShowHide();

public slots:
	void selectOutput(QString item);
	void domessage(MidiMessage mess);

	void obs_actions_select(QString action);
	void check_advanced_switch(bool state);
	void ResetToDefaults();

	void on_source_change(QString source);
	void on_scene_change(QString source);
	void get_sources(QString scene);
	void obs_actions_filter_select(int);
	void get_filters(QString Source);
	void get_transitions();
	void get_scenes();
	void add_new_mapping();
	void add_row_from_hook(MidiHook *hook);
	void tab_changed(int i);
	void delete_mapping();
	void edit_mapping(int row, int col);
	void set_cell_colors(QColor color, QTableWidgetItem *item);

private:
	Ui::PluginWindow *ui;

	bool hidedebugitems = true;
	bool loadingdevices = false;
	QMetaObject::Connection desconnect;
	bool starting = true;
	QString DeviceFilter;
	void ShowPair(pairs pair);
	void HidePair(pairs pair);
	void HideAllPairs();
	void add_midi_device(QString Name);
	void set_headers();
	QStringList SceneList;
	bool listening = false;
	void get_scene_names();

	bool map_exists();

private:
	QGridLayout *layout;
	QStringList *items;
	QString sceneName;
	bool switching = false;
	void ShowOnly(QList<ActionsClass::Actions> shows);
	void ShowEntry(ActionsClass::Actions Entry);
	void HideEntry(ActionsClass::Actions Entry);
	void ShowAllActions();
	void HideAdvancedActions();
	void HideEntries(QList<ActionsClass::Actions> entrys);
	void ShowEntries(QList<ActionsClass::Actions> entrys);
	QString untranslate(QString translation);
	QListView *listview;
	QList<ActionsClass::Actions> FrontendActions = {
		ActionsClass::Actions::Start_Streaming,
		ActionsClass::Actions::Stop_Streaming,
		ActionsClass::Actions::Toggle_Start_Stop_Streaming,
		ActionsClass::Actions::Start_Recording,
		ActionsClass::Actions::Stop_Recording,
		ActionsClass::Actions::Pause_Recording,
		ActionsClass::Actions::Unpause_Recording,
		ActionsClass::Actions::Start_Replay_Buffer,
		ActionsClass::Actions::Stop_Replay_Buffer,
		ActionsClass::Actions::Enable_Preview,
		ActionsClass::Actions::Disable_Preview,
		ActionsClass::Actions::Studio_Mode,
		ActionsClass::Actions::Do_Transition,
		ActionsClass::Actions::Reset_Stats};

	QList<ActionsClass::Actions> filterActions = {
		ActionsClass::Actions::Enable_Source_Filter,
		ActionsClass::Actions::Disable_Source_Filter,
		ActionsClass::Actions::Toggle_Source_Filter};

	QList<ActionsClass::Actions> sceneActions = {
		ActionsClass::Actions::Reset_Scene_Item,
		ActionsClass::Actions::Set_Current_Scene,
		ActionsClass::Actions::Set_Scene_Transition_Override,
		ActionsClass::Actions::Set_Current_Transition};

	QList<ActionsClass::Actions> sourceActions = {
		ActionsClass::Actions::Toggle_Mute,
		ActionsClass::Actions::Take_Source_Screenshot};

	QList<ActionsClass::Actions> mediaActions = {
		ActionsClass::Actions::Play_Pause_Media,
		ActionsClass::Actions::Restart_Media, ActionsClass::Actions::Stop_Media,
		ActionsClass::Actions::Next_Media, ActionsClass::Actions::Previous_Media};

	QList<ActionsClass::Actions> AdvancedSourceActions = {
		ActionsClass::Actions::Set_Mute,
		ActionsClass::Actions::Set_Source_Name,
		ActionsClass::Actions::Set_Sync_Offset,
		ActionsClass::Actions::Set_Source_Settings,
		ActionsClass::Actions::Set_Source_Filter_Visibility,
		ActionsClass::Actions::Set_Audio_Monitor_Type};
	QList<ActionsClass::Actions> AdvancedMediaActions = {
		ActionsClass::Actions::Scrub_Media};

	QList<ActionsClass::Actions> AdvancedFilterActions = {
		ActionsClass::Actions::Set_Gain_Filter};
	QList<ActionsClass::Actions> AdvancedSceneActions = {
		ActionsClass::Actions::Set_Scene_Item_Render,
		ActionsClass::Actions::Set_Scene_Item_Position,
		ActionsClass::Actions::Set_Scene_Item_Transform,
		ActionsClass::Actions::Set_Scene_Item_Crop};
};
