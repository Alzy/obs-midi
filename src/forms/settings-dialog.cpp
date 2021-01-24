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

#if __has_include(<obs-frontend-api.h>)
#include <obs-frontend-api.h>
#else
#include <obs-frontend-api/obs-frontend-api.h>
#endif
#include <obs-module.h>
#include <obs-data.h>
#include <string>
#include <map>
#include <iostream>
#include <utility>
#include "../midi-agent.h"
#include "../obs-midi.h"
#include "../device-manager.h"
#include "../config.h"
#include "settings-dialog.h"
#include <qdialogbuttonbox.h>
#include <qcheckbox.h>
#include "../version.h"
	PluginWindow::PluginWindow(QWidget *parent)
	: QDialog(parent, Qt::Dialog),
	ui(new Ui::PluginWindow)
{
	ui->setupUi(this);
	connect(ui->list_midi_dev, &QListWidget::currentTextChanged, this,
		&PluginWindow::on_item_select);
	connect(ui->check_enabled, &QCheckBox::stateChanged, this,
		&PluginWindow::on_check_enabled_stateChanged);
	connect(ui->bidirectional, &QCheckBox::stateChanged, this,
		&PluginWindow::on_bid_enabled_stateChanged);

	SetAvailableDevices();
	//ui->PluginWindow->
	QString title;
	title.append("OBS MIDI Settings -- Branch: ");
	title.append(GIT_BRANCH);
	title.append(" -- Commit: ");
	title.append(GIT_COMMIT_HASH);
	this->setWindowTitle(title);
}

void PluginWindow::ToggleShowHide()
{

	if (!isVisible()) {

		setVisible(true);

	}

	else {

		setVisible(false);
	}
}

void PluginWindow::setCheck(bool x)
{
	this->ui->check_enabled->setChecked(x);
	
}

void PluginWindow::SetAvailableDevices()
{

	auto midiOutDevices = GetDeviceManager()->GetOPL();
	loadingdevices = true;
	this->ui->outbox->clear();
	this->ui->outbox->insertItems(0, midiOutDevices);
	loadingdevices = false;
	auto midiDevices = GetDeviceManager()->GetPortsList();

	this->ui->list_midi_dev->clear();
	this->ui->check_enabled->setEnabled(false);
	this->ui->outbox->setEnabled(false);

	if (midiDevices.size() == 0) {
		this->ui->list_midi_dev->addItem("No Devices Available");
	} else if (midiDevices.size() > 0) {
		this->ui->check_enabled->setEnabled(true);
		this->ui->outbox->setEnabled(false);
	}

	for (int i = 0; i < midiDevices.size(); i++) {
		this->ui->list_midi_dev->addItem(midiDevices.at(i).c_str());
	}

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
		if (midiDevices.size() != 0) {
			desconnect = connect(
				ui->outbox, SIGNAL(currentTextChanged(QString)),
				this, SLOT(selectOutput(QString)));
		}
		starting = false;
	}

	this->ui->list_midi_dev->setCurrentRow(-1);
	this->ui->list_midi_dev->setCurrentRow(0);
}



void PluginWindow::selectOutput(QString selectedDeviceName)
{
	if (!loadingdevices) {
		auto selectedDevice =
			ui->list_midi_dev->currentItem()->text().toStdString();
		auto device = GetDeviceManager()->GetMidiDeviceByName(
			selectedDevice.c_str());
		device->SetOutName(selectedDeviceName.toStdString());

		GetConfig()->Save();
	}
}

int PluginWindow::on_check_enabled_stateChanged(bool state)
{

	if (state == true) {

		auto selectedDeviceName =
			ui->list_midi_dev->currentItem()->text().toStdString();
		auto selectedOutDeviceName =
			ui->outbox->currentText().toStdString();
		auto device = GetDeviceManager()->GetMidiDeviceByName(
			selectedDeviceName.c_str());
		blog(LOG_INFO, "Item enabled: %s", selectedDeviceName.c_str());
		int devicePort = GetDeviceManager()->GetPortNumberByDeviceName(
			selectedDeviceName.c_str());
		int deviceOutPort =
			GetDeviceManager()->GetOutPortNumberByDeviceName(
				selectedOutDeviceName.c_str());
		if (device == NULL) {
			GetDeviceManager()->RegisterMidiDevice(devicePort,
							       deviceOutPort);
			device = GetDeviceManager()->GetMidiDeviceByName(
				selectedDeviceName.c_str());
			device->OpenPort(devicePort);
			device->OpenOutPort(deviceOutPort);
		} else {

			device->OpenPort(devicePort);
			device->OpenOutPort(deviceOutPort);
		}
	}

	//ui->outbox->setCurrentText(QString::fromStdString(device->GetOutName()));
	ui->bidirectional->setEnabled(state);
	ui->bidirectional->setChecked(true);
	ui->outbox->setEnabled(true);
	GetConfig()->Save();
	return state;
}

void PluginWindow::on_item_select(QString curitem)
{
	auto texting = curitem.toStdString();
	// Pull info on if device is enabled, if so set true if not set false
	auto device = GetDeviceManager()->GetMidiDeviceByName(
		curitem.toStdString().c_str());
	if (device != NULL && device->isEnabled()) {
		ui->check_enabled->setChecked(true);
		ui->outbox->setEnabled(true);
		ui->bidirectional->setEnabled(true);
		ui->bidirectional->setChecked(device->isBidirectional());
		auto on = device->GetOutName();
		ui->outbox->setCurrentText(QString::fromStdString(on));

	} else {
		ui->check_enabled->setChecked(false);
		ui->outbox->setEnabled(false);
		ui->bidirectional->setEnabled(false);
	}
}
int PluginWindow::on_bid_enabled_stateChanged(bool state)
{
	auto device = GetDeviceManager()->GetMidiDeviceByName(
		ui->list_midi_dev->currentItem()->text().toStdString().c_str());
	if (state) {
		return 1;
		device->setBidirectional(state);

	} else {
		return 0;
		device->setBidirectional(state);
	}
	GetConfig()->Save();
	GetConfig()->Load();
}

PluginWindow::~PluginWindow()
{

	loadingdevices = false;
	starting = true;
	disconnect(desconnect);
	delete ui;
}

