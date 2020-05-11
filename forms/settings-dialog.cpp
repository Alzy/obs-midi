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

#include <qdialogbuttonbox.h>


#define CHANGE_ME "changeme"

SettingsDialog::SettingsDialog(QWidget *parent, vector<MidiAgent *> activeMidiAgents)
	:
	QDialog(parent, Qt::Dialog),
	ui(new Ui::SettingsDialog)
{
	ui->setupUi(this);

	connect(ui->list_midi_dev, &QListWidget::itemSelectionChanged, this, &SettingsDialog::on_item_select);
	connect(ui->check_enabled, &QCheckBox::stateChanged, this, &SettingsDialog::on_check_enabled_stateChanged);
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
	this->ui->tbl_debug->setItem(rowCount , 0, new QTableWidgetItem (tr(time.c_str())));
	this->ui->tbl_debug->setItem(rowCount , 1, new QTableWidgetItem(tr(message.c_str())));
	this->ui->tbl_debug->setItem(rowCount , 2, new QTableWidgetItem(tr(std::to_string(control).c_str())));
	this->ui->tbl_debug->setItem(rowCount , 3, new QTableWidgetItem(tr(std::to_string(value).c_str())));
}


void SettingsDialog::on_btn_configure_clicked()
{
	
	pushDebugMidiMessage("time", "button clicked", 0, 0);
	blog(LOG_INFO, "Configure button clicked");
	
	SettingsMidiMap mDialog;
	//mDialog.setModal(true);
	mDialog.exec();
}

int SettingsDialog::on_check_enabled_stateChanged(int state)
{
	pushDebugMidiMessage("time", "Check State", state, 0);
	return state;
}

void SettingsDialog::on_item_select()
{
	QString current = this->ui->list_midi_dev->currentItem()->text();
	blog(LOG_INFO, "item clicked: %s", current.toLocal8Bit().data());
	pushDebugMidiMessage("item clicked",current.toLocal8Bit().data(), 0, 0);

	// Pull info on if device is enabled, if so set true if not set false
	ui->check_enabled->setChecked(false);

	//If enabled, allow for configuration, if not disable it.
	if (ui->check_enabled->checkState()) {
		ui->btn_configure->setEnabled(true);
	} else {
		ui->btn_configure->setEnabled(false);
	}
}


void SettingsDialog::FormAccepted()
{
	return;
}


SettingsDialog::~SettingsDialog() {
	delete ui;
}
