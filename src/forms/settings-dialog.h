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

signals:
	void changed(obs_data_t *change);
private Q_SLOTS:
	void ToggleShowHide();
public slots:
	void on_check_enabled_state_changed(int state);
	void on_bid_enabled_state_changed(int state);
	void on_device_select(QString curitem);
	void select_output_device(QString item);
	void handle_midi_message(MidiMessage mess);
	void obs_actions_select(QString action);
	void set_edit_mode();
	void save_edit();
	void reset_to_defaults();
	void on_scene_change(QString newscene);
	void on_source_change(QString newsource);
	void add_new_mapping();
	void add_row_from_hook(MidiHook *hook);
	void tab_changed(int i);
	void clear_table();
	void load_table();
	void delete_mapping();
	void edit_mapping();
	void set_cell_colors(QColor color, QTableWidgetItem *item);

private:
	Ui::PluginWindow *ui;
	bool hidedebugitems = true;
	bool loadingdevices = false;
	bool starting = true;
	bool listening = false;
	void configure_table();
	void set_title_window();
	void connect_ui_signals();
	void setCheck(bool check);
	void load_devices();
	void show_pair(Pairs pair);
	void hide_pair(Pairs pair);
	void hide_all_pairs();
	void add_midi_device(const QString &Name);
	void set_headers();
	
	void set_configure_title(const QString &title);
	void connect_midi_message_handler();
	bool map_exists();
	int map_location(MidiMessage message);
	bool verify_mapping();

private:
	bool switching = false;
	void ShowOnly(QList<ActionsClass::Actions> shows);
	void ShowEntry(ActionsClass::Actions Entry);
	void HideEntry(ActionsClass::Actions Entry);
	void ShowAllActions();
	void HideEntries(QList<ActionsClass::Actions> entrys);
	void ShowEntries(QList<ActionsClass::Actions> entrys);
	QString untranslate(QString translation);
	QListView *listview;
};
