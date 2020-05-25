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

#include <obs-frontend-api/obs-frontend-api.h>
#include <obs-module.h>
#include <obs-data.h>
#include <string>
#include <map>
#include <iostream>
#include <utility>
#include "midi-agent.h"

#include "obs-midi.h"
#include "device-manager.h"
#include "config.h"
#include "settings-dialog.h"
#include "ui_configwindow.h"
#include "configwindow.h"
#include <qdialogbuttonbox.h>
#include <qcheckbox.h>

#define CHANGE_ME "changeme"

SettingsDialog::SettingsDialog(QWidget *parent):QDialog(parent, Qt::Dialog),ui(new Ui::SettingsDialog)
{
	ui->setupUi(this);
	connect(ui->list_midi_dev, &QListWidget::itemSelectionChanged, this, &SettingsDialog::on_item_select);
	connect(ui->check_enabled, &QCheckBox::stateChanged, this, &SettingsDialog::on_check_enabled_stateChanged);
	connect(ui->btn_configure, &QPushButton::clicked, this,&SettingsDialog::on_btn_configure_clicked);
	connect(ui->check_enabled, &QCheckBox::toggled, this, &SettingsDialog::on_check_clicked);
	connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &SettingsDialog::FormAccepted);
	//hide debug items 
	ui->lbl_debug->setHidden(hidedebugitems);
	ui->tbl_debug->setHidden(hidedebugitems);
	this->ui->list_midi_dev->setCurrentRow(0);
}


void SettingsDialog::ToggleShowHide() {
	if (!isVisible())
		setVisible(true);
	else
		setVisible(false);
}

void SettingsDialog::setCheck(bool x)
{
	this->ui->check_enabled->setChecked(x);
	ui->btn_configure->setEnabled(x);
}

void SettingsDialog::SetAvailableDevices(std::vector<std::string> &midiDevices)
{
	this->ui->list_midi_dev->clear();

	if (midiDevices.size() == 0){
		this->ui->list_midi_dev->addItem("No Devices Available");
		this->ui->check_enabled->setEnabled(false);
		return;
	}

	for (int i = 0; i < midiDevices.size(); i++) {
		this->ui->list_midi_dev->addItem(midiDevices.at(i).c_str());
		std::string name = midiDevices.at(i);
	}
	this->ui->list_midi_dev->setCurrentRow(0);
}
void SettingsDialog::on_check_clicked(bool enabled) {
	if (enabled) {
		pushDebugMidiMessage("time", "check", 0, 0);
	} else {
		pushDebugMidiMessage("time", "check", 1, 0);
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
	string devicename =ui->list_midi_dev->currentItem()->text().toStdString();

	ConfigWindow *cwin = new ConfigWindow(devicename);
	blog(LOG_INFO, "new config window created");

	cwin->devicename = devicename;
	blog(LOG_INFO, "Device Name set");

	cwin->exec();
	blog(LOG_INFO, "execute config window");

	
}


int SettingsDialog::on_check_enabled_stateChanged(bool state)
{
	auto deviceManager = GetDeviceManager();
	string selectedDeviceName = ui->list_midi_dev->currentItem()->text().toStdString();

	auto device = deviceManager->GetMidiDeviceByName(selectedDeviceName.c_str());
	if (state == true) {
		blog(LOG_INFO, "Item enabled: %s", selectedDeviceName.c_str());
		int devicePort = deviceManager->GetPortNumberByDeviceName(selectedDeviceName.c_str());
		if (device == NULL)
		{
			deviceManager->RegisterMidiDevice(devicePort);
		}
		else
		{
			device->OpenPort(devicePort);
		}
	}
	else {
		if (device != NULL)
		{
			device->ClosePort();
		}
		blog(LOG_INFO, "Item disabled: %s", selectedDeviceName.c_str());
	}

	pushDebugMidiMessage("time", "Check State", state, 0);
	ui->btn_configure->setEnabled(state);

	auto config = GetConfig();
	config->Save();

	return state;
}

void SettingsDialog::on_item_select()
{
	QString current = this->ui->list_midi_dev->currentItem()->text();
	blog(LOG_INFO, "item clicked: %s", current.toLocal8Bit().data());
	pushDebugMidiMessage("item clicked",current.toLocal8Bit().data(), 0, 0);

	// Pull info on if device is enabled, if so set true if not set false
	auto deviceManager = GetDeviceManager();
	string selectedDeviceName = current.toStdString();
	auto device = deviceManager->GetMidiDeviceByName(selectedDeviceName.c_str());
	if (device != NULL && device->isEnabled())
	{
		ui->check_enabled->setChecked(true);
		ui->btn_configure->setEnabled(true);

	}
	else {
		ui->check_enabled->setChecked(false);
		ui->btn_configure->setEnabled(false);


	}

	//If enabled, enable configuration button, if not disable it.
ui->btn_configure->setEnabled(ui->check_enabled->isChecked());
	
}


void SettingsDialog::FormAccepted()
{
	return;
}


SettingsDialog::~SettingsDialog() {
	delete ui;
}
