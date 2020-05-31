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



SettingsDialog::SettingsDialog(QWidget* parent) :QDialog(parent, Qt::Dialog), ui(new Ui::SettingsDialog)
{
	ui->setupUi(this);
	connect(ui->list_midi_dev, &QListWidget::currentTextChanged, this, &SettingsDialog::on_item_select);
	connect(ui->check_enabled, &QCheckBox::stateChanged, this, &SettingsDialog::on_check_enabled_stateChanged);
	connect(ui->btn_configure, &QPushButton::clicked, this, &SettingsDialog::on_btn_configure_clicked);

	SetAvailableDevices();
	
}


void SettingsDialog::ToggleShowHide() {

	if (!isVisible()) {

		setVisible(true);
		
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
	
	auto midiOutDevices = GetDeviceManager()->GetOPL();\
	loadingdevices = true;
	this->ui->outbox->clear();
	this->ui->outbox->insertItems(0, midiOutDevices);
	loadingdevices = false;
	auto midiDevices = GetDeviceManager()->GetPortsList();
	if (starting) {

		#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
			//define something for Windows (32-bit and 64-bit, this part is common)
			this->ui->outbox->setCurrentIndex(1);
		#elif __linux__
			this->ui->outbox->setCurrentIndex(0);
			// linux
		#elif __unix__ // all unices not caught above
			this->ui->outbox->setCurrentIndex(0);
			// Unix
		#endif
	}

	this->ui->list_midi_dev->clear();
	this->ui->check_enabled->setEnabled(false);
	this->ui->btn_configure->setEnabled(false);
	this->ui->outbox->setEnabled(false);
	if (midiDevices.size() == 0) {
		this->ui->list_midi_dev->addItem("No Devices Available");
	}
	else if (midiDevices.size() > 0){
		this->ui->check_enabled->setEnabled(true);
		this->ui->btn_configure->setEnabled(true);
		this->ui->outbox->setEnabled(true);
	}

	for (int i = 0; i < midiDevices.size(); i++) {
		this->ui->list_midi_dev->addItem(midiDevices.at(i).c_str());
		
	}
	if (starting) {
		
		desconnect = connect(ui->outbox, SIGNAL(currentTextChanged(QString)), this,	SLOT(selectOutput(QString)));
		starting = false;
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

void SettingsDialog::selectOutput(QString selectedDeviceName) {
	if (!loadingdevices) {
		auto selectedDevice =	ui->list_midi_dev->currentItem()->text().toStdString();
		auto device = GetDeviceManager()->GetMidiDeviceByName(selectedDevice.c_str());
		device->SetOutName(selectedDeviceName.toStdString());
		GetConfig()->Save();
	}

}

int SettingsDialog::on_check_enabled_stateChanged(bool state)
{
	
	
	if (state == true) {
		auto selectedDeviceName = ui->list_midi_dev->currentItem()->text().toStdString();
		auto selectedOutDeviceName = ui->outbox->currentText().toStdString();
		auto device = GetDeviceManager()->GetMidiDeviceByName(selectedDeviceName.c_str());
		blog(LOG_INFO, "Item enabled: %s", selectedDeviceName.c_str());
		int devicePort = GetDeviceManager()->GetPortNumberByDeviceName(selectedDeviceName.c_str());
		int deviceOutPort = GetDeviceManager()->GetOutPortNumberByDeviceName(selectedOutDeviceName.c_str());
		if (device == NULL )
		{
			GetDeviceManager()->RegisterMidiDevice(devicePort,deviceOutPort);
		}
		else
		{
			device->OpenPort(devicePort);
			if (deviceOutPort != -1) {
				device->OpenOutPort(deviceOutPort);
			}
			
		}
	}
	
		
	
	//ui->outbox->setCurrentText(QString::fromStdString(device->GetOutName()));
	ui->btn_configure->setEnabled(state);
	ui->outbox->setEnabled(state);
	GetConfig()->Save();
	return state;
}

void SettingsDialog::on_item_select(QString curitem)
{
	auto texting = curitem.toStdString();
	// Pull info on if device is enabled, if so set true if not set false
	auto device = GetDeviceManager()->GetMidiDeviceByName(curitem.toStdString().c_str());
	if (device != NULL && device->isEnabled())
	{
		ui->check_enabled->setChecked(true);
		ui->btn_configure->setEnabled(true);
		ui->outbox->setEnabled(true);
		auto on = device->GetOutName();
		ui->outbox->setCurrentText(QString::fromStdString(on));
		

	}
	else {
		ui->check_enabled->setChecked(false);
		ui->btn_configure->setEnabled(false);
		ui->outbox->setEnabled(false);


	}


	
}




SettingsDialog::~SettingsDialog() {

	loadingdevices = false;
	starting = true;
	disconnect(desconnect);
	delete ui;
}
