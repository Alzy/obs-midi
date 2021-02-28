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
#include <QtWidgets/QDialog>

#include <vector>

#include "ui_settings-dialog.h"

#include "../midi-agent.h"
#include "../version.h"

class PluginWindow : public QDialog {
	Q_OBJECT
public:
	explicit PluginWindow(QWidget *parent);
	~PluginWindow() override;
public  slots:
	void ToggleShowHide();
private slots:
	void on_check_enabled_state_changed(int state);
	void on_bid_enabled_state_changed(int state);
	void on_device_select(const QString &curitem);
	void select_output_device(const QString &item);
	void handle_midi_message(const MidiMessage &mess);
	void obs_actions_select(const QString &action);
	void set_edit_mode();
	void save_edit();
	void reset_to_defaults();
	void on_scene_change(const QString &newscene);
	void on_source_change(const QString &newsource);
	void add_new_mapping();
	void add_row_from_hook(MidiHook *hook);
	void tab_changed(int i);
	void clear_table();
	void load_table();
	void delete_mapping();
	void edit_mapping();
	void set_cell_colors(const QColor &color, QTableWidgetItem *item);

private:
	Ui::PluginWindow *ui;
	bool hidedebugitems = true;
	bool loadingdevices = false;
	bool starting = true;
	bool listening = false;
	void configure_table();
	void set_title_window();
	void connect_ui_signals();
	void setup_actions();
	//	void setCheck(bool check); // not implemented
	void load_devices();
	void show_pair(Pairs pair);
	void hide_pair(Pairs pair);
	void hide_all_pairs();
	void add_midi_device(const QString &Name);
	void set_headers();
	void set_configure_title(const QString &title);
	void connect_midi_message_handler();
	bool map_exists();
	int find_mapping_location(const MidiMessage &message);
	bool verify_mapping();

private:
	bool switching = false;

};
