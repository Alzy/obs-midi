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



	HidePair("extra_1");
	HidePair("transition");
	HidePair("audio_source");
	HidePair("media_source");
	HidePair("filter");
	HidePair("scene");
	HidePair("source");
	HidePair("item");
	ui->cb_obs_output_audio_source->addItems(Utils::GetAudioSourceNames());
	ui->cb_obs_output_media_source->addItems(Utils::GetMediaSourceNames());
	ui->cb_obs_output_transition->addItems(Utils::GetTransitionsList());
	get_scene_names();
	ui->cb_obs_output_scene->addItems(SceneList);

	TranslateActions();

	connect(ui->cb_obs_action, SIGNAL(currentIndexChanged(int)), this,
		SLOT(obs_actions_filter_select(int)));
	connect(ui->cb_obs_output_scene, SIGNAL(currentTextChanged(QString)),
		this, SLOT(GetSources(QString)));
	connect(ui->cb_obs_output_action, SIGNAL(currentTextChanged(QString)),
		this, SLOT(obs_actions_select(QString)));
	connect(ui->cb_obs_output_scene, SIGNAL(currentTextChanged(QString)),
		this, SLOT(GetSources(QString)));
	connect(ui->cb_obs_output_source, SIGNAL(currentTextChanged(QString)),
		this, SLOT(GetFilters(QString)));

	//connect all combos to on change
	connect(ui->cb_obs_output_action, SIGNAL(currentTextChanged(QString)),
		this, SLOT(onChange()));
	connect(ui->cb_obs_output_source, SIGNAL(currentTextChanged(QString)),
		this, SLOT(onChange()));
	connect(ui->cb_obs_output_source, SIGNAL(currentTextChanged(QString)),
		this, SLOT(on_source_change(QString)));
	connect(ui->cb_obs_output_scene, SIGNAL(currentTextChanged(QString)),
		this, SLOT(on_scene_change(QString)));
	connect(ui->cb_obs_output_scene, SIGNAL(currentTextChanged(QString)),
		this, SLOT(onChange()));
	connect(ui->cb_obs_output_item, SIGNAL(currentTextChanged(QString)),
		this, SLOT(onChange()));
	connect(ui->cb_obs_output_filter, SIGNAL(currentTextChanged(QString)),
		this, SLOT(onChange()));
	connect(ui->cb_obs_output_transition,
		SIGNAL(currentTextChanged(QString)), this, SLOT(onChange()));
	connect(ui->cb_obs_output_audio_source,
		SIGNAL(currentTextChanged(QString)), this, SLOT(onChange()));
	connect(ui->cb_obs_output_media_source,
		SIGNAL(currentTextChanged(QString)), this, SLOT(onChange()));

	/**************Connections to mappper****************/

	this->listview = new QListView(this->ui->cb_obs_output_action);
	this->ui->cb_obs_output_action->setView(this->listview);
	this->ui->cb_obs_output_action->addItems(AllActions);
	this->listview->setSizeAdjustPolicy(
		QAbstractScrollArea::SizeAdjustPolicy::AdjustToContents);
	ui->cb_obs_output_action->setSizeAdjustPolicy(
		QComboBox::SizeAdjustPolicy::AdjustToContents);
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
		ui->tab_configure->setEnabled(false);
		ui->bidirectional->setEnabled(false);
		ui->check_enabled->setEnabled(false);
		this->ui->outbox->setEnabled(false);
		ui->tabWidget->setEnabled(false);
	} else if (midiDevices.size() > 0) {
		this->ui->check_enabled->setEnabled(true);
		this->ui->outbox->setEnabled(false);
		ui->tab_configure->setEnabled(true);
	}

	for (int i = 0; i < midiDevices.size(); i++) {
		this->ui->list_midi_dev->addItem(midiDevices.at(i));
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
		device->SetOutName(selectedDeviceName);

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
	ui->tabWidget->setTabText(1, QString("Configure - ").append(curitem));
	// Pull info on if device is enabled, if so set true if not set false
	DeviceFilter = curitem;
	auto device = GetDeviceManager()->GetMidiDeviceByName(
		curitem.toStdString().c_str());
	if (device != NULL && device->isEnabled()) {
		ui->check_enabled->setChecked(true);
		ui->outbox->setEnabled(true);
		ui->bidirectional->setEnabled(true);
		ui->bidirectional->setChecked(device->isBidirectional());
		ui->outbox->setCurrentText(device->GetOutName());

	} else {
		ui->check_enabled->setChecked(false);
		ui->outbox->setEnabled(false);
		ui->bidirectional->setEnabled(false);
	}
	auto devicemanager = GetDeviceManager();
	auto config = GetConfig();
	MidiAgent * MAdevice = devicemanager->GetMidiDeviceByName(DeviceFilter.toStdString().c_str());
	
	///HOOK up the Message Handler
	connect(MAdevice,
		SIGNAL(SendNewUnknownMessage(MidiMessage)), this,
		SLOT(domessage(MidiMessage))); /// name, mtype, norc, channel
	ui->mapping_lbl_device_name->setText(curitem);
}
void PluginWindow::domessage(MidiMessage mess) {
	if (ui->tabWidget->currentIndex() == 1) {
		if (ui->btn_Listen_one->isChecked() ||
		    ui->btn_Listen_many->isChecked()) {
			blog(1,
			     "got midi message via gui, \n Device = %s \nMType = %s \n NORC : %i \n Channel: %i \n Value: %i",
			     mess.device_name.toStdString().c_str(),
			     mess.message_type.toStdString().c_str(), mess.NORC,
			     mess.channel, mess.value);
			ui->mapping_lbl_device_name->setText(mess.device_name);
			ui->sb_channel->setValue(mess.channel);
			ui->sb_norc->setValue(mess.NORC);
			ui->slider_value->setValue(mess.value);
			ui->cb_mtype->setCurrentText(mess.message_type);
			ui->btn_Listen_one->setChecked(false);
		}
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

void PluginWindow::get_scene_names()
{
	obs_frontend_source_list sceneList = {};
	obs_frontend_get_scenes(&sceneList);
	SceneList.clear();
	for (size_t i = 0; i < sceneList.sources.num; i++) {

			SceneList.append(obs_source_get_name(
				sceneList.sources.array[i]));
	}
	obs_frontend_source_list_free(&sceneList);
}
void PluginWindow::add_midi_device(QString name)
{
	blog(LOG_DEBUG, "Adding Midi Device %s", name.toStdString().c_str());
	QTableWidgetItem *device_name = new QTableWidgetItem();
	QTableWidgetItem *device_enabled = new QTableWidgetItem();
	QTableWidgetItem *device_status = new QTableWidgetItem();
	QTableWidgetItem *feedback_enabled = new QTableWidgetItem();
	QTableWidgetItem *feedback_name = new QTableWidgetItem();
	QTableWidgetItem *feedback_status = new QTableWidgetItem();
	int rowcount = this->ui->table_mapping->rowCount();
	this->ui->table_mapping->insertRow(rowcount);
	device_name->setText(name);
	device_enabled->setCheckState(Qt::Unchecked);
	device_status->setText(QString("Disconnected"));
	device_status->setTextColor("grey");
	feedback_name->setText("");
	feedback_enabled->setCheckState(Qt::Unchecked);
	feedback_status->setText(QString("unset"));
	this->ui->table_mapping->setItem(rowcount, 0, device_name);
	this->ui->table_mapping->setItem(rowcount, 1, device_enabled);
	this->ui->table_mapping->setItem(rowcount, 2, device_status);
	this->ui->table_mapping->setItem(rowcount, 3, feedback_enabled);
	this->ui->table_mapping->setItem(rowcount, 4, feedback_name);
	this->ui->table_mapping->setItem(rowcount, 5, feedback_status);
}
void PluginWindow::set_headers()
{
	ui->table_mapping->setHorizontalHeaderLabels(
		{"Name", "Enabled", "Status", "Feedback Enabled",
		 "Feedback Port", "Feedback Status"});
}


void PluginWindow::ShowPair(pairs Pair)
{
	switch (Pair) {
	case pairs::Scene:
		ui->label_obs_output_scene->show();
		ui->cb_obs_output_scene->show();
		break;
	case pairs::Source:
		ui->label_obs_output_source->show();
		ui->cb_obs_output_source->show();
		break;
	case pairs::Filter:
		ui->label_obs_output_filter->show();
		ui->cb_obs_output_filter->show();
		break;
	case pairs::Transition:
		ui->label_obs_output_transition->show();
		ui->cb_obs_output_transition->show();
		break;
	case pairs::Item:
		ui->label_obs_output_item->show();
		ui->cb_obs_output_item->show();
		break;
	case pairs::Audio:
		ui->label_obs_output_audio_source->show();
		ui->cb_obs_output_audio_source->show();
		break;
	case pairs::Media:
		ui->label_obs_output_media_source->show();
		ui->cb_obs_output_media_source->show();
		break;
	}
}
void PluginWindow::HidePair(pairs Pair)
{
	switch (Pair) {
	case pairs::Scene:
		ui->label_obs_output_scene->hide();
		ui->cb_obs_output_scene->hide();
		blog(LOG_DEBUG, "Hide Scene");
		break;
	case pairs::Source:
		ui->label_obs_output_source->hide();
		ui->cb_obs_output_source->hide();
		blog(LOG_DEBUG, "Hide Source");
		break;
	case pairs::Filter:
		ui->label_obs_output_filter->hide();
		ui->cb_obs_output_filter->hide();
		blog(LOG_DEBUG, "Hide Filter");
		break;
	case pairs::Transition:
		ui->label_obs_output_transition->hide();
		ui->cb_obs_output_transition->hide();
		blog(LOG_DEBUG, "Hide Transition");
		break;
	case pairs::Item:
		ui->label_obs_output_item->hide();
		ui->cb_obs_output_item->hide();
		blog(LOG_DEBUG, "Hide Item");
		break;
	case pairs::Audio:
		ui->label_obs_output_audio_source->hide();
		ui->cb_obs_output_audio_source->hide();
		blog(LOG_DEBUG, "Hide Audio");
		break;
	case pairs::Media:
		ui->label_obs_output_media_source->hide();
		ui->cb_obs_output_media_source->hide();
		blog(LOG_DEBUG, "Hide Media");
		break;
	}
}
void PluginWindow::HideAllPairs()
{
	HidePair(pairs::Transition);
	HidePair(pairs::Audio);
	HidePair(pairs::Media);
	HidePair(pairs::Filter);
	HidePair(pairs::Scene);
	HidePair(pairs::Source);
	HidePair(pairs::Item);
}


void PluginWindow::ResetToDefaults()
{
	ui->cb_obs_output_action->setCurrentIndex(0);
	ui->cb_obs_action->setCurrentIndex(0);
	ui->cb_obs_output_transition->setCurrentIndex(0);
	ui->cb_obs_output_filter->setCurrentIndex(0);
	ui->cb_obs_output_scene->setCurrentIndex(0);
	ui->cb_obs_output_source->setCurrentIndex(0);
	ui->cb_obs_output_audio_source->setCurrentIndex(0);
	ui->cb_obs_output_media_source->setCurrentIndex(0);
}
QStringList PluginWindow::GetTransitions()
{
	return Utils::GetTransitionsList();
}
void PluginWindow::ShowPair(QString Pair)
{
	if (Pair == "scene") {
		ui->label_obs_output_scene->show();
		ui->cb_obs_output_scene->show();
		ui->cb_obs_output_scene->addItems(GetScenes());
	} else if (Pair == "source") {
		ui->label_obs_output_source->show();
		ui->cb_obs_output_source->show();
		ui->cb_obs_output_source->addItems(
			GetSources(ui->cb_obs_output_scene->currentText()));
	} else if (Pair == "filter") {
		ui->label_obs_output_filter->show();
		ui->cb_obs_output_filter->show();
		ui->cb_obs_output_filter->addItems(
			GetFilters(ui->cb_obs_output_source->currentText()));
	} else if (Pair == "transition") {
		ui->label_obs_output_transition->show();
		ui->cb_obs_output_transition->show();

	} else if (Pair == "item") {
		ui->label_obs_output_item->show();
		ui->cb_obs_output_item->show();

	} else if (Pair == "audio_source") {
		ui->label_obs_output_audio_source->show();
		ui->cb_obs_output_audio_source->show();

	} else if (Pair == "media_source") {
		ui->label_obs_output_media_source->show();
		ui->cb_obs_output_media_source->show();
	}
}
void PluginWindow::HidePair(QString Pair)
{
	if (Pair == "scene") {
		ui->label_obs_output_scene->hide();
		ui->cb_obs_output_scene->hide();
	} else if (Pair == "source") {
		ui->label_obs_output_source->hide();
		ui->cb_obs_output_source->hide();
	} else if (Pair == "filter") {
		ui->label_obs_output_filter->hide();
		ui->cb_obs_output_filter->hide();
	} else if (Pair == "transition") {
		ui->label_obs_output_transition->hide();
		ui->cb_obs_output_transition->hide();
	} else if (Pair == "item") {
		ui->label_obs_output_item->hide();
		ui->cb_obs_output_item->hide();
	} else if (Pair == "audio_source") {
		ui->label_obs_output_audio_source->hide();
		ui->cb_obs_output_audio_source->hide();
	} else if (Pair == "media_source") {
		ui->label_obs_output_media_source->hide();
		ui->cb_obs_output_media_source->hide();
	}
}
void PluginWindow::TranslateActions()
{
	for (int i = 0; i < AllActions_raw.size(); i++) {
		AllActions.append(obs_module_text(
			AllActions_raw.at(i).toStdString().c_str()));
	}
}
void PluginWindow::ShowIntActions() {}
void PluginWindow::on_source_change(QString source)
{
	ui->cb_obs_output_filter->clear();
	ui->cb_obs_output_filter->addItems(GetFilters(source));
	ui->cb_obs_output_item->clear();
	ui->cb_obs_output_item->addItems(Utils::GetSceneItemsList(source));
}
void PluginWindow::on_scene_change(QString scene)
{
	ui->cb_obs_output_source->clear();
	ui->cb_obs_output_source->addItems(GetSources(scene));
	ui->cb_obs_output_item->clear();
	ui->cb_obs_output_item->addItems(Utils::GetSceneItemsList(scene));
}
void PluginWindow::ShowStringActions() {}
void PluginWindow::ShowBoolActions() {}
void PluginWindow::ShowOnly(QStringList shows)
{
	int count = AllActions.count();
	for (int i = 0; i < count; i++) {
		if (shows.contains(AllActions.at(i))) {
			ShowEntry(AllActions.at(i));
		} else {
			HideEntry(AllActions.at(i));
		}
	}
}
QString PluginWindow::FirstVisible()
{
	int count = AllActions.count();
	for (int i = 0; i < count; i++) {
		if (!listview->isRowHidden(i)) {
			return AllActions.at(i);
		}
	}
}
void PluginWindow::ShowEntry(QString Entry)
{
	int x = AllActions.indexOf(Entry);
	if (x == -1) {
		blog(1, "no entry -- %s", Entry.toStdString().c_str());
	} else {
		listview->setRowHidden(x, false);
		listview->adjustSize();
		ui->cb_obs_output_action->adjustSize();
	}
}
void PluginWindow::HideEntry(QString Entry)
{
	int x = AllActions.indexOf(Entry);
	if (x == -1) {
		blog(1, "no entry -- %s", Entry.toStdString().c_str());
	} else {
		listview->setRowHidden(x, true);
		listview->adjustSize();
		ui->cb_obs_output_action->adjustSize();
	}
}
void PluginWindow::ShowAllActions()
{
	int count = ui->cb_obs_output_action->count();
	for (int i = 0; i < count; i++) {
		ShowEntry(AllActions.at(i));
	}
}
void PluginWindow::HideEntries(QStringList entrys)
{
	int count = ui->cb_obs_output_action->count();

	for (int i = 0; i < count; i++) {
		if (entrys.contains(AllActions.at(i))) {
			HideEntry(AllActions.at(i));
		}
	}
	listview->adjustSize();
}
void PluginWindow::ShowEntries(QStringList entrys)
{
	int count = ui->cb_obs_output_action->count();

	for (int i = 0; i < count; i++) {
		if (entrys.contains(AllActions.at(i))) {
			ShowEntry(AllActions.at(i));
		}
	}
	listview->adjustSize();
}

bool PluginWindow::MapCall(QString plugin, obs_data_t *map)
{
	if (plugin == "OBS") {
		return DoMap(map);
	} else {
		return false;
	}
}
bool PluginWindow::DoMap(obs_data_t *map)
{
	//make map into data array
	//pull action from data array
	//map action based on actionsMap

	return false;
}
void PluginWindow::HideAdvancedActions()
{
	HideEntries(AdvancedFilterActions);
	HideEntries(AdvancedMediaActions);
	HideEntries(AdvancedSceneActions);
	HideEntries(AdvancedSourceActions);
}
QStringList PluginWindow::GetSources(QString scene)
{

	SL_sources.clear();
	ui->cb_obs_output_source->clear();
	auto arrayref = Utils::GetSceneArray(scene);
	int size = obs_data_array_count(arrayref);
	for (int i = 0; i < size; i++) {
		obs_data *item = obs_data_array_item(arrayref, i);

		SL_sources.append(QString(obs_data_get_string(item, "name")));
		obs_data_release(item);
	}
	SL_sources.sort();
	if (!switching) {
		ui->cb_obs_output_source->addItems(SL_sources);
	}
	obs_data_array_release(arrayref);
	return SL_sources;
}
QStringList PluginWindow::GetScenes()
{
	obs_data_array *x = Utils::GetScenes();
	int cnt = obs_data_array_count(x);
	for (int i = 0; i <= cnt; i++) {
		auto it = obs_data_array_item(x, i);
		SL_scenes.append(obs_data_get_string(it, "name"));
		obs_data_release(it);
	}
	obs_data_array_release(x);
	return SL_scenes;
}
QStringList PluginWindow::GetFilters(QString source)
{

	ui->cb_obs_output_filter->clear();
	SL_filters.clear();

	auto x = obs_get_source_by_name(source.toStdString().c_str());
	OBSDataArrayAutoRelease y = Utils::GetSourceFiltersList(x, false);
	for (int i = 0; i < obs_data_array_count(y); i++) {
		OBSDataAutoRelease z = obs_data_array_item(y, i);
		SL_filters.append(QString(obs_data_get_string(z, "name")));
	}
	if (!switching) {
		ui->cb_obs_output_filter->addItems(SL_filters);
	}
	return SL_filters;
}
void PluginWindow::check_advanced_switch(bool state)
{
	//obs_actions_filter_select(ui->cb_obs_action->currentIndex());
}
void PluginWindow::obs_actions_filter_select(int selection)
{
	switching = true;

	switch (selection) {
	case 0:
		ShowAllActions();
		//All filters

	case 1:
		// Frontend
		ShowOnly(FrontendActions);

		break;
	case 2:
		// Scenes
		ShowOnly(sceneActions);

		break;
	case 3:
		//Sources
		ShowOnly(sourceActions);

		break;
	case 4:
		//Filters
		ShowOnly(filterActions);

		break;
	case 5:
		//Media
		ShowOnly(mediaActions);

		break;
	};
	ui->cb_obs_output_action->setCurrentText(FirstVisible());
	switching = false;
}

void PluginWindow::edit_action(obs_data_t *actions)
{
	if (QString(obs_data_get_string(actions, "Type")) == QString("OBS")) {
		ui->cb_obs_output_action->setCurrentText(
			tr(obs_data_get_string(actions, "action")));
		ui->cb_obs_output_scene->setCurrentText(
			QString(obs_data_get_string(actions, "scene")));
		ui->cb_obs_output_audio_source->setCurrentText(
			QString(obs_data_get_string(actions, "audio_source")));
		blog(1, "edit_action-- widget -- %s",
		     obs_data_get_string(actions, "action"));
	}
}
void PluginWindow::onChange()
{
	obs_data_t *data = obs_data_create();
	obs_data_set_string(
		data, "action",
		AllActions_raw
			.at(AllActions.indexOf(
				ui->cb_obs_output_action->currentText()))
			.toStdString()
			.c_str());
	obs_data_set_string(
		data, "scene",
		ui->cb_obs_output_scene->currentText().toStdString().c_str());
	obs_data_set_string(
		data, "source",
		ui->cb_obs_output_source->currentText().toStdString().c_str());
	obs_data_set_string(
		data, "filter",
		ui->cb_obs_output_filter->currentText().toStdString().c_str());
	obs_data_set_string(data, "transition",
			    ui->cb_obs_output_transition->currentText()
				    .toStdString()
				    .c_str());
	obs_data_set_string(
		data, "item",
		ui->cb_obs_output_item->currentText().toStdString().c_str());
	obs_data_set_string(data, "audio_source",
			    ui->cb_obs_output_audio_source->currentText()
				    .toStdString()
				    .c_str());
	obs_data_set_string(data, "media_source",
			    ui->cb_obs_output_media_source->currentText()
				    .toStdString()
				    .c_str());
	obs_data_set_string(data, "Type", "OBS");
	emit(changed(data));
	//obs_data_release(data);
}

void PluginWindow::obs_actions_select(QString action)
{
	HidePair("extra_1");
	HidePair("transition");
	HidePair("audio_source");
	HidePair("media_source");
	HidePair("filter");
	HidePair("scene");
	HidePair("source");
	HidePair("item");
	std::map<QString, std::function<void(PluginWindow * here)>> funcMap = {
		{"control.action.Set_Current_Scene",
		 [](PluginWindow *here) { here->ShowPair("scene"); }},
		{"control.action.Start_Streaming",
		 [](PluginWindow *here) {

		 }},
		{"control.action.Stop_Streaming",
		 [](PluginWindow *here) {}},
		{"control.action.Toggle_Start_Stop_Streaming",
		 [](PluginWindow *here) {}},
		{"control.action.Start_Recording",
		 [](PluginWindow *here) {}},
		{"control.action.Stop_Recording",
		 [](PluginWindow *here) {}},
		{"control.action.Pause_Recording",
		 [](PluginWindow *here) {}},
		{"control.action.Unpause_Recording",
		 [](PluginWindow *here) {}},
		{"control.action.Start_Replay_Buffer",
		 [](PluginWindow *here) {}},
		{"control.action.Stop_Replay_Buffer",
		 [](PluginWindow *here) {}},
		{"control.action.Enable_Preview",
		 [](PluginWindow *here) {}},
		{"control.action.Disable_Preview",
		 [](PluginWindow *here) {}},
		{"control.action.Studio_Mode", [](PluginWindow *here) {}},
		{"control.action.Transition", [](PluginWindow *here) {}},
		{"control.action.Reset_Stats", [](PluginWindow *here) {}},
		//source
		{"control.action.Enable_Source_Filter",
		 [](PluginWindow *here) {
			 here->ShowPair("scene");
			 here->ShowPair("source");
			 here->ShowPair("filter");
		 }},
		{"control.action.Disable_Source_Filter",
		 [](PluginWindow *here) {
			 here->ShowPair("scene");
			 here->ShowPair("source");
			 here->ShowPair("filter");
		 }},
		{"control.action.Set_Gain_Filter",
		 [](PluginWindow *here) {
			 here->ShowPair("scene");
			 here->ShowPair("source");
			 here->ShowPair("filter");
		 }},
		{"control.action.Toggle_Source_Filter",
		 [](PluginWindow *here) {
			 here->ShowPair("scene");
			 here->ShowPair("source");
			 here->ShowPair("filter");
		 }},
		{"control.action.Reset_Scene_Item",
		 [](PluginWindow *here) {
			 here->ShowPair("source");

			 here->ShowPair("scene");
			 here->ShowPair("item");
		 }},
		{"control.action.Set_Scene_Item_Render",
		 [](PluginWindow *here) {
			 here->ShowPair("source");
			 here->ShowPair("scene");
			 here->ShowPair("item");
		 }},
		{"control.action.Set_Scene_Item_Position",
		 [](PluginWindow *here) {
			 here->ShowPair("scene");
			 here->ShowPair("item");
		 }},
		{"control.action.Set_Scene_Item_Transform",
		 [](PluginWindow *here) {
			 here->ShowPair("scene");
			 here->ShowPair("item");
		 }},
		{"control.action.Set_Scene_Item_Crop",
		 [](PluginWindow *here) {
			 here->ShowPair("scene");
			 here->ShowPair("item");
		 }},
		{"control.action.Set_Current_Scene",
		 [](PluginWindow *here) { here->ShowPair("scene"); }},
		{"control.action.Set_Scene_Transition_Override",
		 [](PluginWindow *here) {
			 here->ShowPair("scene");
			 here->ShowPair("transition");
		 }},
		{"control.action.Set_Current_Transition",
		 [](PluginWindow *here) { here->ShowPair("transition"); }},
		{"control.action.Set_Volume",
		 [](PluginWindow *here) {
			 here->ShowPair("audio_source");
		 }},
		{"control.action.Set_Mute",
		 [](PluginWindow *here) {
			 here->ShowPair("audio_source");
		 }},
		{"control.action.Toggle_Mute",
		 [](PluginWindow *here) {
			 here->ShowPair("audio_source");
		 }},
		{"control.action.Set_Source_Name",
		 [](PluginWindow *here) {}},
		{"control.action.Set_Sync_Offset",
		 [](PluginWindow *here) {}},
		{"control.action.Set_Source_Settings",
		 [](PluginWindow *here) {}},
		{"control.action.Set_Source_Filter_Visibility",
		 [](PluginWindow *here) {
			 here->ShowPair("source");
			 here->ShowPair("filter");
		 }},
		{"control.action.Set_Audio_Monitor_Type",
		 [](PluginWindow *here) {}},
		{"control.action.Take_Source_Screenshot",
		 [](PluginWindow *here) {
			 here->ShowPair("source");
			 here->ShowPair("scene");
		 }},
		{"control.action.Play_Pause_Media",
		 [](PluginWindow *here) { here->ShowPair("media_source"); }},
		{"control.action.Restart_Media",
		 [](PluginWindow *here) { here->ShowPair("media_source"); }},
		{"control.action.Stop_Media",
		 [](PluginWindow *here) { here->ShowPair("media_source"); }},
		{"control.action.Next_Media",
		 [](PluginWindow *here) { here->ShowPair("media_source"); }},
		{"control.action.Previous_Media",
		 [](PluginWindow *here) { here->ShowPair("media_source"); }},
		{"control.action.Set_Media_Time",
		 [](PluginWindow *here) { here->ShowPair("media_source"); }},
		{"control.action.Scrub_Media",
		 [](PluginWindow *here) { here->ShowPair("media_source"); }}

	};
	try {
		funcMap[untranslate(action)](this);
	} catch (std::exception &e) {
		blog(LOG_DEBUG, "error %s", e.what());
	}
}
QString PluginWindow::untranslate(QString tstring)
{
	return AllActions_raw.at(AllActions.indexOf(tstring));
}
