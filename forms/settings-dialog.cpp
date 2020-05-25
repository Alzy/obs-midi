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



SettingsDialog::SettingsDialog(QWidget *parent):QDialog(parent, Qt::Dialog),ui(new Ui::SettingsDialog)
{
	ui->setupUi(this);
	connect(ui->list_midi_dev, &QListWidget::itemSelectionChanged, this, &SettingsDialog::on_item_select);
	connect(ui->check_enabled, &QCheckBox::stateChanged, this, &SettingsDialog::on_check_enabled_stateChanged);
	connect(ui->btn_configure, &QPushButton::clicked, this,&SettingsDialog::on_btn_configure_clicked);
	
	//hide debug items 
	ui->lbl_debug->setHidden(hidedebugitems);
	ui->tbl_debug->setHidden(hidedebugitems);
	this->ui->list_midi_dev->setCurrentRow(0);
}


void SettingsDialog::ToggleShowHide() {
	
	if (!isVisible()) {

		setVisible(true);
		SetAvailableDevices();
	}

	else {
		setVisible(false);
	}
		
}

void SettingsDialog::setCheck(bool x)
{
	this->ui->check_enabled->setChecked(x);
	this->ui->btn_configure->setEnabled(x);
}

void SettingsDialog::SetAvailableDevices()
{
	auto midiDevices = GetDeviceManager()->GetPortsList();
	this->ui->list_midi_dev->clear();

	if (midiDevices.size() == 0){
		this->ui->list_midi_dev->addItem("No Devices Available");
		this->ui->check_enabled->setEnabled(false);
		this->ui->btn_configure->setEnabled(false);
		return;
	}

	for (int i = 0; i < midiDevices.size(); i++) {
		this->ui->list_midi_dev->addItem(midiDevices.at(i).c_str());
	}
	this->ui->list_midi_dev->setCurrentRow(0);
}






void SettingsDialog::on_btn_configure_clicked()
{

	blog(LOG_INFO, "Configure button clicked");
	ConfigWindow *cwin = new ConfigWindow(ui->list_midi_dev->currentItem()->text());
	blog(LOG_INFO, "new config window created");
	cwin->exec();
	blog(LOG_INFO, "execute config window");

	
}


int SettingsDialog::on_check_enabled_stateChanged(bool state)
{
	auto selectedDeviceName = ui->list_midi_dev->currentItem()->text().toStdString();

	auto device = GetDeviceManager()->GetMidiDeviceByName(selectedDeviceName.c_str());
	if (state == true) {
		blog(LOG_INFO, "Item enabled: %s", selectedDeviceName.c_str());
		int devicePort = GetDeviceManager()->GetPortNumberByDeviceName(selectedDeviceName.c_str());
		if (device == NULL)
		{
			GetDeviceManager()->RegisterMidiDevice(devicePort);
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
	ui->btn_configure->setEnabled(state);
	GetConfig()->Save();
	return state;
}

void SettingsDialog::on_item_select()
{
	// Pull info on if device is enabled, if so set true if not set false
	auto device = GetDeviceManager()->GetMidiDeviceByName(this->ui->list_midi_dev->currentItem()->text().toStdString().c_str());
	if (device != NULL && device->isEnabled())
	{
		ui->check_enabled->setChecked(true);
		ui->btn_configure->setEnabled(true);

	}
	else {
		ui->check_enabled->setChecked(false);
		ui->btn_configure->setEnabled(false);


	}


	
}




SettingsDialog::~SettingsDialog() {
	delete ui;
}
