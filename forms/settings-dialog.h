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
#include "ui_configwindow.h"
#include "configwindow.h"
#include "midi-agent.h"

class ConfigWindow;


class SettingsDialog : public QDialog
{
	Q_OBJECT

public:
	SettingsDialog(QWidget* parent);
	~SettingsDialog();
	void ToggleShowHide();
	void setCheck(bool check);
	void SetAvailableDevices(std::vector<std::string> &midiDevices);
        void pushDebugMidiMessage(std::string time, std::string message, int control, int value);
	void on_check_clicked(bool enabled);
	void on_btn_configure_clicked();
	int on_check_enabled_stateChanged(bool state);
	void on_item_select();

private Q_SLOTS:
	void FormAccepted();

private:
	Ui::SettingsDialog* ui;
	ConfigWindow *cwin;
};
