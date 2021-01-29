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


	if (loadingdevices) {
		auto devicemanager = GetDeviceManager();
		auto device = devicemanager->GetMidiDeviceByName(
			ui->mapping_lbl_device_name->text());
		auto hooks = devicemanager->GetMidiHooksByDeviceName(
			ui->mapping_lbl_device_name->text());
		if (hooks.size() > 0) {

			for (int i = 0; i < hooks.size(); i++) {

				add_row_from_hook(hooks.at(i));
			}
		}
	}
	HideAllPairs();
	
	ui->cb_obs_output_audio_source->addItems(Utils::GetAudioSourceNames());
	ui->cb_obs_output_media_source->addItems(Utils::GetMediaSourceNames());
	ui->cb_obs_output_transition->addItems(Utils::GetTransitionsList());
	get_scene_names();
	ui->cb_obs_output_scene->addItems(SceneList);

	TranslateActions();

	connect(ui->cb_obs_action_filter, SIGNAL(currentIndexChanged(int)), this,
		SLOT(obs_actions_filter_select(int)));
	connect(ui->cb_obs_output_scene, SIGNAL(currentTextChanged(QString)),
		this, SLOT(get_sources(QString)));
	connect(ui->cb_obs_output_action, SIGNAL(currentTextChanged(QString)),
		this, SLOT(obs_actions_select(QString)));
	connect(ui->cb_obs_output_scene, SIGNAL(currentTextChanged(QString)),
		this, SLOT(get_sources(QString)));
	connect(ui->cb_obs_output_source, SIGNAL(currentTextChanged(QString)),
		this, SLOT(get_filters(QString)));


	connect(ui->cb_obs_output_source, SIGNAL(currentTextChanged(QString)),
		this, SLOT(on_source_change(QString)));
	connect(ui->cb_obs_output_scene, SIGNAL(currentTextChanged(QString)),
		this, SLOT(on_scene_change(QString)));

	/**************Connections to mappper****************/
	connect(ui->btn_add, SIGNAL(clicked()), this, SLOT(add_new_mapping()));
	this->ui->cb_obs_output_action->addItems(TranslateActions());
	loadingdevices = true;
}

void PluginWindow::ToggleShowHide()
{

	if (!isVisible()) {
		setVisible(true);
		ui->table_mapping->clear();
		if (loadingdevices) {
			auto devicemanager = GetDeviceManager();
			auto device = devicemanager->GetMidiDeviceByName(
				ui->mapping_lbl_device_name->text());
			auto hooks = devicemanager->GetMidiHooksByDeviceName(
				ui->mapping_lbl_device_name->text());
			if (hooks.size() > 0) {

				for (int i = 0; i < hooks.size(); i++) {

					add_row_from_hook(hooks.at(i));
				}
			}
			set_headers();

		}
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
		{"Channel", "Message Type", "Note or Control", "Action",
		 "Scene", "Source","Filter","Transition","Item","Audio Source", "Media Source"});
}


void PluginWindow::ShowPair(pairs Pair)
{
	
	switch (Pair) {
	case pairs::Scene:
		ui->label_obs_output_scene->show();
		ui->cb_obs_output_scene->show();
		get_scenes();
		ui->w_scene->show();
		break;
	case pairs::Source:
		ui->label_obs_output_source->show();
		ui->cb_obs_output_source->show();
		get_sources(ui->cb_obs_output_scene->currentText());
		ui->w_source->show();
		break;
	case pairs::Filter:
		ui->label_obs_output_filter->show();
		ui->cb_obs_output_filter->show();

		ui->w_filter->show();
		break;
	case pairs::Transition:
		ui->label_obs_output_transition->show();
		ui->cb_obs_output_transition->show();
		ui->w_transition->show();
		break;
	case pairs::Item:
		ui->label_obs_output_item->show();
		ui->cb_obs_output_item->show();
		ui->w_item->show();
		break;
	case pairs::Audio:
		ui->cb_obs_output_audio_source->clear();
		ui->cb_obs_output_audio_source->addItems(
			Utils::GetAudioSourceNames());
		ui->label_obs_output_audio_source->show();
		ui->cb_obs_output_audio_source->show();
		ui->w_audio->show();
		break;
	case pairs::Media:
		ui->cb_obs_output_media_source->clear();
		ui->cb_obs_output_media_source->addItems(Utils::GetMediaSourceNames());
		ui->label_obs_output_media_source->show();
		ui->cb_obs_output_media_source->show();
		ui->w_media->show();
		break;
	}
}
void PluginWindow::HidePair(pairs Pair)
{
	switch (Pair) {
	case pairs::Scene:
		ui->label_obs_output_scene->hide();
		ui->cb_obs_output_scene->hide();
		ui->cb_obs_output_scene->clear();

		ui->w_scene->hide();
		blog(LOG_DEBUG, "Hide Scene");
		break;
	case pairs::Source:
		ui->label_obs_output_source->hide();
		ui->cb_obs_output_source->hide();
		ui->cb_obs_output_source->clear();
		ui->w_source->hide();
		blog(LOG_DEBUG, "Hide Source");
		break;
	case pairs::Filter:
		ui->label_obs_output_filter->hide();
		ui->cb_obs_output_filter->hide();
		ui->cb_obs_output_filter->clear();
		ui->w_filter->hide();
		blog(LOG_DEBUG, "Hide Filter");
		break;
	case pairs::Transition:
		ui->label_obs_output_transition->hide();
		ui->cb_obs_output_transition->hide();
		ui->cb_obs_output_transition->clear();
		ui->w_transition->hide();
		blog(LOG_DEBUG, "Hide Transition");
		break;
	case pairs::Item:
		ui->label_obs_output_item->hide();
		ui->cb_obs_output_item->hide();
		ui->cb_obs_output_item->clear();
		ui->w_item->hide();
		blog(LOG_DEBUG, "Hide Item");
		break;
	case pairs::Audio:
		ui->label_obs_output_audio_source->hide();
		ui->cb_obs_output_audio_source->hide();
		ui->cb_obs_output_audio_source->clear();
		ui->w_audio->hide();
		blog(LOG_DEBUG, "Hide Audio");
		break;
	case pairs::Media:
		ui->label_obs_output_media_source->hide();
		ui->cb_obs_output_media_source->hide();
		ui->cb_obs_output_media_source->clear();
		ui->w_media->hide();
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
	ui->cb_obs_action_filter->setCurrentIndex(0);
	ui->cb_obs_output_transition->setCurrentIndex(0);
	ui->cb_obs_output_filter->setCurrentIndex(0);
	ui->cb_obs_output_scene->setCurrentIndex(0);
	ui->cb_obs_output_source->setCurrentIndex(0);
	ui->cb_obs_output_audio_source->setCurrentIndex(0);
	ui->cb_obs_output_media_source->setCurrentIndex(0);
}
void PluginWindow::get_transitions()
{
	ui->cb_obs_output_transition->clear();
	ui->cb_obs_output_transition->addItems(Utils::GetTransitionsList());
}
QStringList PluginWindow::TranslateActions()
{
	QStringList temp;
	for (int i = 0; i < AllActions_raw.size(); i++) {
		temp.append(obs_module_text(
			Utils::action_to_string(AllActions_raw.at(i))
				.toStdString()
				.c_str()));
	}
	return temp;

}
void PluginWindow::on_source_change(QString source)
{
	
	get_filters(source);
	ui->cb_obs_output_item->clear();
	ui->cb_obs_output_item->addItems(Utils::GetSceneItemsList(source));
}
void PluginWindow::on_scene_change(QString scene)
{
	
	get_sources(scene);
	ui->cb_obs_output_item->clear();
	ui->cb_obs_output_item->addItems(Utils::GetSceneItemsList(scene));
}

void PluginWindow::ShowOnly(QList<Actions> shows)
{
	
	ui->cb_obs_output_action->clear();
	for (int i = 0; i < shows.size(); i++) {
		ui->cb_obs_output_action->addItem(obs_module_text(Utils::action_to_string(shows.at(i)).toStdString().c_str()));
	}
	
}

void PluginWindow::ShowEntry(Actions Entry)
{
	if (ui->cb_obs_output_action->findText(Utils::action_to_string(Entry)) ==-1) {
		ui->cb_obs_output_action->addItem(obs_module_text(Utils::action_to_string(Entry).toStdString().c_str()));
	}
	
}
void PluginWindow::HideEntry(Actions Entry)
{
	if (ui->cb_obs_output_action->findText(Utils::action_to_string(Entry)) > 0) {
		ui->cb_obs_output_action->removeItem(ui->cb_obs_output_action->findText(
			Utils::action_to_string(Entry)));
	}
}
void PluginWindow::ShowAllActions()
{
	int count = ui->cb_obs_output_action->count();
	for (int i = 0; i < count; i++) {
		ShowEntry(AllActions_raw.at(i));
	}
}
void PluginWindow::HideEntries(QList<Actions> entrys)
{
	int count = ui->cb_obs_output_action->count();

	for (int i = 0; i < count; i++) {
		if (entrys.contains(AllActions_raw.at(i))) {
			HideEntry(AllActions_raw.at(i));
		}
	}
	listview->adjustSize();
}
void PluginWindow::ShowEntries(QList<Actions> entrys)
{
	int count = ui->cb_obs_output_action->count();

	for (int i = 0; i < count; i++) {
		if (entrys.contains(AllActions_raw.at(i))) {
			ShowEntry(AllActions_raw.at(i));
		}
	}
	listview->adjustSize();
}



void PluginWindow::HideAdvancedActions()
{
	HideEntries(AdvancedFilterActions);
	HideEntries(AdvancedMediaActions);
	HideEntries(AdvancedSceneActions);
	HideEntries(AdvancedSourceActions);
}
void PluginWindow::get_sources(QString scene)
{

	ui->cb_obs_output_source->clear();
	auto arrayref = Utils::GetSceneArray(scene);
	int size = obs_data_array_count(arrayref);
	for (int i = 0; i < size; i++) {
		obs_data *item = obs_data_array_item(arrayref, i);

		ui->cb_obs_output_source->addItem(
			QString(obs_data_get_string(item, "name")));
		obs_data_release(item);
	}
	obs_data_array_release(arrayref);
	
}
void PluginWindow::get_scenes()
{
	ui->cb_obs_output_scene->clear();
	obs_data_array *x = Utils::GetScenes();
	int cnt = obs_data_array_count(x);
	for (int i = 0; i <= cnt; i++) {
		auto it = obs_data_array_item(x, i);		
		ui->cb_obs_output_scene->addItem(
			obs_data_get_string(it, "name"));
		obs_data_release(it);
	}
	
	obs_data_array_release(x);
}
void PluginWindow::get_filters(QString source)
{

	ui->cb_obs_output_filter->clear();

	auto x = obs_get_source_by_name(source.toStdString().c_str());
	OBSDataArrayAutoRelease y = Utils::GetSourceFiltersList(x, false);
	for (int i = 0; i < obs_data_array_count(y); i++) {
		OBSDataAutoRelease z = obs_data_array_item(y, i);
		ui->cb_obs_output_filter->addItem(QString(obs_data_get_string(z, "name")));
	}
	
	
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
	switching = false;
	ui->cb_obs_output_action->setCurrentIndex(0);
}


void PluginWindow::obs_actions_select(QString action)
{
	if (!switching) {
		HideAllPairs();

		switch (Utils::string_to_action(untranslate(action))) {
		case Actions::Set_Current_Scene:
			ShowPair(pairs::Scene);
			break;
		case Actions::Enable_Source_Filter:
			ShowPair(pairs::Scene);
			ShowPair(pairs::Source);
			ShowPair(pairs::Filter);
			break;
		case Actions::Disable_Source_Filter:
			ShowPair(pairs::Scene);
			ShowPair(pairs::Source);
			ShowPair(pairs::Filter);
			break;
		case Actions::Set_Gain_Filter:
			ShowPair(pairs::Scene);
			ShowPair(pairs::Source);
			ShowPair(pairs::Filter);
			break;
		case Actions::Toggle_Source_Filter:
			ShowPair(pairs::Scene);
			ShowPair(pairs::Source);
			ShowPair(pairs::Filter);
			break;
		case Actions::Reset_Scene_Item:
			ShowPair(pairs::Scene);
			ShowPair(pairs::Source);
			ShowPair(pairs::Item);
			break;
		case Actions::Set_Scene_Item_Render:
			ShowPair(pairs::Scene);
			ShowPair(pairs::Source);
			ShowPair(pairs::Item);
			break;
		case Actions::Set_Scene_Item_Position:
			ShowPair(pairs::Scene);
			ShowPair(pairs::Item);
			break;
		case Actions::Set_Scene_Item_Transform:
			ShowPair(pairs::Scene);
			ShowPair(pairs::Item);
			break;
		case Actions::Set_Scene_Item_Crop:
			ShowPair(pairs::Scene);
			ShowPair(pairs::Item);
			break;
		case Actions::Set_Scene_Transition_Override:
			ShowPair(pairs::Scene);
			ShowPair(pairs::Transition);
			break;
		case Actions::Set_Current_Transition:
			ShowPair(pairs::Transition);
			break;
		case Actions::Set_Volume:
			ShowPair(pairs::Audio);
			break;
		case Actions::Set_Mute:
			ShowPair(pairs::Audio);
			break;
		case Actions::Toggle_Mute:
			ShowPair(pairs::Audio);
			break;
		case Actions::Set_Source_Filter_Visibility:
			ShowPair(pairs::Source);
			ShowPair(pairs::Filter);
			break;
		case Actions::Take_Source_Screenshot:
			ShowPair(pairs::Source);
			ShowPair(pairs::Scene);
			break;
		case Actions::Play_Pause_Media:
			ShowPair(pairs::Media);
			break;
		case Actions::Restart_Media:
			ShowPair(pairs::Media);
			break;
		case Actions::Stop_Media:
			ShowPair(pairs::Media);
			break;
		case Actions::Next_Media:
			ShowPair(pairs::Media);
			break;
		case Actions::Previous_Media:
			ShowPair(pairs::Media);
			break;
		case Actions::Set_Media_Time:
			ShowPair(pairs::Media);
			break;
		case Actions::Scrub_Media:
			ShowPair(pairs::Media);
			break;
		default:
			HideAllPairs();
			break;
		}
	}
	
}
QString PluginWindow::untranslate(QString tstring)
{
	
	return Utils::action_to_string(AllActions_raw.at(TranslateActions().indexOf(tstring)));
}
bool PluginWindow::map_exists() {
	return false;
	
}

void PluginWindow::add_new_mapping()
{

		int row = ui->table_mapping->rowCount();
		ui->table_mapping->insertRow(row);

		QTableWidgetItem *channelitem = new QTableWidgetItem(
			QString::number(ui->sb_channel->value()));

		QTableWidgetItem *mtypeitem =
			new QTableWidgetItem(ui->cb_mtype->currentText());
		QTableWidgetItem *norcitem = new QTableWidgetItem(
			QString::number(ui->sb_norc->value()));
		QTableWidgetItem *actionitem = new QTableWidgetItem(
			ui->cb_obs_output_action->currentText());
		QTableWidgetItem *sceneitem = new QTableWidgetItem(
			ui->cb_obs_output_scene->currentText());
		QTableWidgetItem *sourceitem = new QTableWidgetItem(
			ui->cb_obs_output_source->currentText());
		QTableWidgetItem *filteritem = new QTableWidgetItem(
			ui->cb_obs_output_filter->currentText());
		QTableWidgetItem *transitionitem = new QTableWidgetItem(
			ui->cb_obs_output_transition->currentText());
		QTableWidgetItem *itemitem = new QTableWidgetItem(
			ui->cb_obs_output_item->currentText());
		QTableWidgetItem *audioitem = new QTableWidgetItem(
			ui->cb_obs_output_audio_source->currentText());
		QTableWidgetItem *mediaitem = new QTableWidgetItem(
			ui->cb_obs_output_media_source->currentText());

		ui->table_mapping->setItem(row, 0, channelitem);
		ui->table_mapping->setItem(row, 1, mtypeitem);
		ui->table_mapping->setItem(row, 2, norcitem);
		ui->table_mapping->setItem(row, 3, actionitem);
		ui->table_mapping->setItem(row, 4, sceneitem);
		ui->table_mapping->setItem(row, 5, sourceitem);
		ui->table_mapping->setItem(row, 6, filteritem);
		ui->table_mapping->setItem(row, 7, transitionitem);
		ui->table_mapping->setItem(row, 8, itemitem);
		ui->table_mapping->setItem(row, 9, audioitem);
		ui->table_mapping->setItem(row, 10, mediaitem);

		MidiHook *newmh = new MidiHook();
		newmh->channel = ui->sb_channel->value();
		newmh->message_type = ui->cb_mtype->currentText();
		newmh->norc = ui->sb_norc->value();
		newmh->action = ui->cb_obs_output_action->currentText();
		newmh->scene = ui->cb_obs_output_scene->currentText();
		newmh->source = ui->cb_obs_output_source->currentText();
		newmh->filter = ui->cb_obs_action_filter->currentText();
		newmh->transition = ui->cb_obs_output_transition->currentText();
		newmh->item = ui->cb_obs_output_item->currentText();
		newmh->audio_source =
			ui->cb_obs_output_audio_source->currentText();
		newmh->media_source =
			ui->cb_obs_output_media_source->currentText();
		auto dm = GetDeviceManager();
		auto dev = dm->GetMidiDeviceByName(
			ui->mapping_lbl_device_name->text());
		dev->AddMidiHook(newmh);
		auto conf = GetConfig();
		conf->Save();
	
}

void PluginWindow::add_row_from_hook(MidiHook * hook) {
	int row = ui->table_mapping->rowCount();
	ui->table_mapping->insertRow(row);

	QTableWidgetItem *channelitem =
		new QTableWidgetItem(QString::number(hook->channel));
	QTableWidgetItem *mtypeitem =
		new QTableWidgetItem(hook->message_type);
	QTableWidgetItem *norcitem =
		new QTableWidgetItem(QString::number(hook->norc));
	QTableWidgetItem *actionitem =
		new QTableWidgetItem(hook->action);
	QTableWidgetItem *sceneitem =
		new QTableWidgetItem(hook->scene);
	QTableWidgetItem *sourceitem =
		new QTableWidgetItem(hook->source);
	QTableWidgetItem *filteritem =
		new QTableWidgetItem(hook->filter);
	QTableWidgetItem *transitionitem = new QTableWidgetItem(
		hook->transition);
	QTableWidgetItem *itemitem =
		new QTableWidgetItem(hook->item);
	QTableWidgetItem *audioitem = new QTableWidgetItem(
		hook->audio_source);
	QTableWidgetItem *mediaitem = new QTableWidgetItem(
		hook->media_source);
	ui->table_mapping->setItem(row, 0, channelitem);
	ui->table_mapping->setItem(row, 1, mtypeitem);
	ui->table_mapping->setItem(row, 2, norcitem);
	ui->table_mapping->setItem(row, 3, actionitem);
	ui->table_mapping->setItem(row, 4, sceneitem);
	ui->table_mapping->setItem(row, 5, sourceitem);
	ui->table_mapping->setItem(row, 6, filteritem);
	ui->table_mapping->setItem(row, 7, transitionitem);
	ui->table_mapping->setItem(row, 8, itemitem);
	ui->table_mapping->setItem(row, 9, audioitem);
	ui->table_mapping->setItem(row, 10, mediaitem);
}
