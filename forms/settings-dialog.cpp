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

#include <obs-frontend-api.h>
#include <obs-module.h>
#include <obs-data.h>
#include <string>
#include <map>
#include <iostream>
#include <utility>
#include "midi-agent.h"

#include "settings-dialog.h"
#include "settings-midi-map.h"
#include <QtWidgets\qdialogbuttonbox.h>

#define CHANGE_ME "changeme"

SettingsDialog::SettingsDialog(QWidget *parent, vector<MidiAgent *> activeMidiAgents)
	:
	QDialog(parent, Qt::Dialog),
	ui(new Ui::SettingsDialog)
{
	ui->setupUi(this);

	connect(ui->btn_configure, &QPushButton::clicked, this,&SettingsDialog::on_btn_configure_clicked);

	connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &SettingsDialog::FormAccepted);
}


void SettingsDialog::ToggleShowHide() {
	if (!isVisible())
		setVisible(true);
	else
		setVisible(false);
}


void SettingsDialog::SetAvailableDevices(std::vector<std::string> &midiDevices)
{
	this->ui->list_midi_dev->clear();

	if (midiDevices.size() == 0){
		this->ui->list_midi_dev->addItem("No Devices Available");
		return;
	}

	for (int i = 0; i < midiDevices.size(); i++) {
		this->ui->list_midi_dev->addItem(midiDevices.at(i).c_str());
	}
}


void SettingsDialog::pushDebugMidiMessage(std::string time, std::string message, int control, int value)
{
	int rowCount = this->ui->tbl_debug->rowCount();
	this->ui->tbl_debug->insertRow(rowCount);
	this->ui->tbl_debug->setItem(rowCount - 1, 0, new QTableWidgetItem (tr(time.c_str())));
	this->ui->tbl_debug->setItem(rowCount - 1, 1, new QTableWidgetItem(tr(message.c_str())));
	this->ui->tbl_debug->setItem(rowCount - 1, 2, new QTableWidgetItem(tr(std::to_string(control).c_str())));
	this->ui->tbl_debug->setItem(rowCount - 1, 3, new QTableWidgetItem(tr(std::to_string(value).c_str())));
}


void SettingsDialog::on_btn_configure_clicked()
{
	
	pushDebugMidiMessage("time", "button clicked", 0, 0);
	blog(LOG_INFO, "Configure button clicked");
	
	SettingsMidiMap mDialog;
	mDialog.setModal(true);
	mDialog.exec();
}


void SettingsDialog::FormAccepted() {
	return;
}


SettingsDialog::~SettingsDialog() {
	delete ui;
}
