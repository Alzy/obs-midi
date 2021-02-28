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
#include <map>
#include <utility>

#include <QDialogButtonBox>

#include <obs-module.h>

#include "settings-dialog.h"
#include "../device-manager.h"
#include "../config.h"

PluginWindow::PluginWindow(QWidget *parent) : QDialog(parent, Qt::Dialog), ui(new Ui::PluginWindow)
{
	ui->setupUi(this);

	//Set Window Title
	setup_actions();
	set_title_window();
	configure_table();
	hide_all_pairs();
	connect_ui_signals();
	starting = false;
}
void PluginWindow::configure_table()
{
	ui->table_mapping->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
	ui->table_mapping->setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);
}
void PluginWindow::set_title_window()
{
	QString title;
	title.append(GIT_BRANCH);
	title.append(" -- Commit: ");
	title.append(GIT_COMMIT_HASH);
	blog(LOG_DEBUG, "OBS-MIDI Version -- Branch: %s", title.toStdString().c_str());
	title.prepend("OBS MIDI Settings -- Branch: ");
	this->setWindowTitle(title);
}
void PluginWindow::connect_ui_signals()
{
	connect(ui->list_midi_dev, SIGNAL(currentTextChanged(QString)), this, SLOT(on_device_select(QString)));
	connect(ui->check_enabled, SIGNAL(stateChanged(int)), this, SLOT(on_check_enabled_state_changed(int)));
	connect(ui->bidirectional, SIGNAL(stateChanged(int)), this, SLOT(on_bid_enabled_state_changed(int)));
	//Connections for Configure Tab
	connect(ui->cb_obs_output_scene, SIGNAL(currentTextChanged(QString)), this, SLOT(get_sources(QString)));
	connect(ui->cb_obs_output_action, SIGNAL(currentTextChanged(QString)), this, SLOT(obs_actions_select(QString)));
	connect(ui->cb_obs_output_source, SIGNAL(currentTextChanged(QString)), this, SLOT(on_source_change(QString)));
	connect(ui->cb_obs_output_scene, SIGNAL(currentTextChanged(QString)), this, SLOT(on_scene_change(QString)));
	connect(ui->table_mapping, SIGNAL(cellClicked(int, int)), this, SLOT(edit_mapping()));
	/**************Connections to mappper****************/
	connect(ui->btn_add, SIGNAL(clicked()), this, SLOT(add_new_mapping()));
	connect(ui->btn_delete, SIGNAL(clicked()), this, SLOT(delete_mapping()));
	connect(ui->tabWidget, SIGNAL(currentChanged(int)), this, SLOT(tab_changed(int)));
	connect(ui->outbox, SIGNAL(currentTextChanged(QString)), this, SLOT(select_output_device(QString)));
}
void PluginWindow::setup_actions()
{
	ui->cb_obs_output_action->clear();
	ui->cb_obs_output_action->addItems(Utils::TranslateActions());
}
void PluginWindow::ToggleShowHide()
{
	if (!isVisible()) {
		load_devices();
		ui->tabWidget->setCurrentIndex(0);
		ui->list_midi_dev->setCurrentRow(0);
		setVisible(true);
	} else {
		setVisible(false);
		ui->btn_Listen_many->setChecked(false);
		ui->btn_Listen_one->setChecked(false);
		hide_all_pairs();
		reset_to_defaults();
	}
}

void PluginWindow::load_devices()
{
	loadingdevices = true;
	auto midiOutDevices = GetDeviceManager()->GetOutPortsList();
	auto midiDevices = GetDeviceManager()->GetPortsList();
	this->ui->list_midi_dev->clear();
	if (midiDevices.size() == 0) {
		this->ui->list_midi_dev->addItem("No Devices Available");
		ui->tab_configure->setEnabled(false);
		ui->bidirectional->setEnabled(false);
		ui->check_enabled->setEnabled(false);
		this->ui->outbox->setEnabled(false);
		ui->tabWidget->setEnabled(false);
	} else if (midiDevices.size() > 0) {
		for (int i = 0; i < midiDevices.size(); i++) {
			this->ui->list_midi_dev->addItem(midiDevices.at(i));
		}
		ui->tab_configure->setEnabled(true);
		ui->bidirectional->setEnabled(true);
		ui->check_enabled->setEnabled(true);
		ui->tabWidget->setEnabled(true);
		this->ui->outbox->clear();
		this->ui->outbox->insertItems(0, midiOutDevices);
	}
	loadingdevices = false;
}
void PluginWindow::select_output_device(const QString &selectedDeviceName)
{
	if (!loadingdevices) {
		auto selectedDevice = ui->list_midi_dev->currentItem()->text().toStdString();
		auto device = GetDeviceManager()->GetMidiDeviceByName(selectedDevice.c_str());
		device->set_midi_output_name(selectedDeviceName);
		GetConfig()->Save();
	}
}
void PluginWindow::on_check_enabled_state_changed(int state)
{
	if (state == Qt::CheckState::Checked) {
		auto selectedDeviceName = ui->list_midi_dev->currentItem()->text().toStdString();
		auto selectedOutDeviceName = ui->outbox->currentText().toStdString();
		auto device = GetDeviceManager()->GetMidiDeviceByName(selectedDeviceName.c_str());
		blog(LOG_INFO, "Item enabled: %s", selectedDeviceName.c_str());
		int devicePort = GetDeviceManager()->GetPortNumberByDeviceName(selectedDeviceName.c_str());
		int deviceOutPort = GetDeviceManager()->GetOutPortNumberByDeviceName(selectedOutDeviceName.c_str());
		if (device == NULL) {
			device = GetDeviceManager()->RegisterMidiDevice(devicePort, deviceOutPort);
		}
		device->open_midi_input_port();
		device->open_midi_output_port();
		device->set_enabled(true);
		ui->bidirectional->setEnabled(true);
		ui->bidirectional->setChecked(device->isBidirectional());
		ui->outbox->setEnabled(device->isBidirectional());
		set_configure_title(QString::fromStdString(selectedDeviceName));
		connect_midi_message_handler();
	}
	GetConfig()->Save();
}
void PluginWindow::connect_midi_message_handler()
{
	auto devicemanager = GetDeviceManager();
	auto MAdevice = devicemanager->GetMidiDeviceByName(ui->list_midi_dev->currentItem()->text());
	connect(MAdevice, SIGNAL(broadcast_midi_message(MidiMessage)), this,
		SLOT(handle_midi_message(MidiMessage))); /// name, mtype, norc, channel
}
void PluginWindow::on_device_select(const QString &curitem)
{
	if (!starting) {
		blog(LOG_DEBUG, "on_device_select %s", curitem.toStdString().c_str());
		auto devicemanager = GetDeviceManager();
		auto config = GetConfig();
		MidiAgent *MAdevice = devicemanager->GetMidiDeviceByName(curitem);
		set_configure_title(curitem);
		// Pull info on if device is enabled, if so set true if not set false
		try {
			if (MAdevice != NULL && MAdevice->isEnabled()) {
				ui->check_enabled->setChecked(true);
				ui->outbox->setEnabled(true);
				ui->bidirectional->setEnabled(true);
				ui->bidirectional->setChecked(MAdevice->isBidirectional());

				if (MAdevice->isBidirectional()) {
					ui->outbox->setCurrentText(MAdevice->get_midi_output_name());
				}
				connect_midi_message_handler();
			} else {
				ui->check_enabled->setChecked(false);
				ui->outbox->setEnabled(false);
				ui->bidirectional->setEnabled(false);
			}
			///HOOK up the Message Handler
			ui->mapping_lbl_device_name->setText(curitem);
		} catch (...) {
		}
	}
}
void PluginWindow::set_configure_title(const QString &title)
{
	ui->tabWidget->setTabText(1, QString("Configure - ").append(title));
}
void PluginWindow::handle_midi_message(const MidiMessage &mess)
{
	if (ui->tabWidget->currentIndex() == 1) {
		if (ui->btn_Listen_one->isChecked() || ui->btn_Listen_many->isChecked()) {
			blog(1, "got midi message via gui, \n Device = %s \n MType = %s \n NORC : %i \n Channel: %i \n Value: %i",
			     mess.device_name.toStdString().c_str(), mess.message_type.toStdString().c_str(), mess.NORC, mess.channel, mess.value);
			ui->mapping_lbl_device_name->setText(mess.device_name);
			ui->sb_channel->setValue(mess.channel);
			ui->sb_norc->setValue(mess.NORC);
			ui->slider_value->setValue(mess.value);
			ui->cb_mtype->setCurrentText(mess.message_type);
			ui->btn_Listen_one->setChecked(false);
		} else {
			if (find_mapping_location(mess) != -1) {
				ui->table_mapping->selectRow(find_mapping_location(mess));
			}
		}
	}
}
void PluginWindow::on_bid_enabled_state_changed(int state)
{
	auto device = GetDeviceManager()->GetMidiDeviceByName(ui->list_midi_dev->currentItem()->text().toStdString().c_str());
	ui->outbox->setEnabled(state);
	if (state) {
		device->set_bidirectional(state);
	} else {
		device->set_bidirectional(state);
	}
}
PluginWindow::~PluginWindow()
{
	delete ui;
}
void PluginWindow::add_midi_device(const QString &name)
{
	blog(LOG_DEBUG, "Adding Midi Device %s", name.toStdString().c_str());

	// don't delete it, because the table takes ownership of the items
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
	//device_status->setForeground("grey");
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
		{"Channel", "Message Type", "Note or Control", "Action", "Scene", "Source", "Filter", "Transition", "Item", "Audio Source", "Media Source"});
	QColor midicolor("#00aaff");
	QColor actioncolor("#aa00ff");
	ui->table_mapping->horizontalHeaderItem(0)->setForeground(midicolor);
	ui->table_mapping->horizontalHeaderItem(1)->setForeground(midicolor);
	ui->table_mapping->horizontalHeaderItem(2)->setForeground(midicolor);
	ui->table_mapping->horizontalHeaderItem(3)->setForeground(actioncolor);
	ui->table_mapping->horizontalHeaderItem(4)->setForeground(actioncolor);
	ui->table_mapping->horizontalHeaderItem(5)->setForeground(actioncolor);
	ui->table_mapping->horizontalHeaderItem(6)->setForeground(actioncolor);
	ui->table_mapping->horizontalHeaderItem(7)->setForeground(actioncolor);
	ui->table_mapping->horizontalHeaderItem(8)->setForeground(actioncolor);
	ui->table_mapping->horizontalHeaderItem(9)->setForeground(actioncolor);
	ui->table_mapping->horizontalHeaderItem(10)->setForeground(actioncolor);
}
void PluginWindow::show_pair(Pairs Pair)
{
	switch (Pair) {
	case Pairs::Scene:
		ui->label_obs_output_scene->show();
		ui->cb_obs_output_scene->show();
		ui->cb_obs_output_scene->addItems(Utils::get_scene_names());
		ui->w_scene->show();
		break;
	case Pairs::Source:
		ui->label_obs_output_source->show();
		ui->cb_obs_output_source->show();
		Utils::get_source_names(ui->cb_obs_output_scene->currentText());
		ui->w_source->show();
		break;
	case Pairs::Filter:
		ui->label_obs_output_filter->show();
		ui->cb_obs_output_filter->show();
		ui->cb_obs_output_filter->addItems(Utils::get_filter_names(ui->cb_obs_output_source->currentText()));
		ui->w_filter->show();
		break;
	case Pairs::Transition:
		ui->label_obs_output_transition->show();
		ui->cb_obs_output_transition->show();
		ui->w_transition->show();
		ui->cb_obs_output_transition->addItems(Utils::GetTransitionsList());
		break;
	case Pairs::Item:
		ui->label_obs_output_item->show();
		ui->cb_obs_output_item->show();
		ui->cb_obs_output_item->addItems(Utils::GetSceneItemsList(ui->cb_obs_output_scene->currentText()));
		ui->w_item->show();
		break;
	case Pairs::Audio:
		ui->cb_obs_output_audio_source->clear();
		ui->cb_obs_output_audio_source->addItems(Utils::GetAudioSourceNames());
		ui->label_obs_output_audio_source->show();
		ui->cb_obs_output_audio_source->show();
		ui->w_audio->show();
		break;
	case Pairs::Media:
		ui->cb_obs_output_media_source->clear();
		ui->cb_obs_output_media_source->addItems(Utils::GetMediaSourceNames());
		ui->label_obs_output_media_source->show();
		ui->cb_obs_output_media_source->show();
		ui->w_media->show();
		break;
	case Pairs::String:
		break;
	case Pairs::Boolean:
		break;
	case Pairs::Integer:
		break;
	}
}
void PluginWindow::hide_pair(Pairs Pair)
{
	switch (Pair) {
	case Pairs::Scene:
		ui->label_obs_output_scene->hide();
		ui->cb_obs_output_scene->hide();
		ui->cb_obs_output_scene->clear();
		ui->w_scene->hide();
		blog(LOG_DEBUG, "Hide Scene");
		break;
	case Pairs::Source:
		ui->label_obs_output_source->hide();
		ui->cb_obs_output_source->hide();
		ui->cb_obs_output_source->clear();
		ui->w_source->hide();
		blog(LOG_DEBUG, "Hide Source");
		break;
	case Pairs::Filter:
		ui->label_obs_output_filter->hide();
		ui->cb_obs_output_filter->hide();
		ui->cb_obs_output_filter->clear();
		ui->w_filter->hide();
		blog(LOG_DEBUG, "Hide Filter");
		break;
	case Pairs::Transition:
		ui->label_obs_output_transition->hide();
		ui->cb_obs_output_transition->hide();
		ui->cb_obs_output_transition->clear();
		ui->w_transition->hide();
		blog(LOG_DEBUG, "Hide Transition");
		break;
	case Pairs::Item:
		ui->label_obs_output_item->hide();
		ui->cb_obs_output_item->hide();
		ui->cb_obs_output_item->clear();
		ui->w_item->hide();
		blog(LOG_DEBUG, "Hide Item");
		break;
	case Pairs::Audio:
		ui->label_obs_output_audio_source->hide();
		ui->cb_obs_output_audio_source->hide();
		ui->cb_obs_output_audio_source->clear();
		ui->w_audio->hide();
		blog(LOG_DEBUG, "Hide Audio");
		break;
	case Pairs::Media:
		ui->label_obs_output_media_source->hide();
		ui->cb_obs_output_media_source->hide();
		ui->cb_obs_output_media_source->clear();
		ui->w_media->hide();
		blog(LOG_DEBUG, "Hide Media");
		break;
	case Pairs::String:
		break;
	case Pairs::Boolean:
		break;
	case Pairs::Integer:
		break;
	}
}
void PluginWindow::hide_all_pairs()
{
	hide_pair(Pairs::Transition);
	hide_pair(Pairs::Audio);
	hide_pair(Pairs::Media);
	hide_pair(Pairs::Filter);
	hide_pair(Pairs::Scene);
	hide_pair(Pairs::Source);
	hide_pair(Pairs::Item);
	hide_pair(Pairs::String);
	hide_pair(Pairs::Integer);
	hide_pair(Pairs::Boolean);
}
void PluginWindow::reset_to_defaults()
{
	ui->cb_obs_output_action->setCurrentIndex(0);
	ui->cb_obs_output_transition->setCurrentIndex(0);
	ui->cb_obs_output_filter->setCurrentIndex(0);
	ui->cb_obs_output_scene->setCurrentIndex(0);
	ui->cb_obs_output_source->setCurrentIndex(0);
	ui->cb_obs_output_audio_source->setCurrentIndex(0);
	ui->cb_obs_output_media_source->setCurrentIndex(0);
	ui->sb_channel->setValue(0);
	ui->sb_norc->setValue(0);
	ui->cb_mtype->setCurrentIndex(0);
	ui->slider_value->setValue(0);
	ui->btn_add->setText("Add Mapping");
}
void PluginWindow::obs_actions_select(const QString &action)
{
	if (!switching) {
		hide_all_pairs();
		switch (ActionsClass::string_to_action(Utils::untranslate(action))) {
		case ActionsClass::Actions::Set_Current_Scene:
			show_pair(Pairs::Scene);
			break;
		case ActionsClass::Actions::Set_Preview_Scene:
			show_pair(Pairs::Scene);
			break;
		case ActionsClass::Actions::Enable_Source_Filter:
			show_pair(Pairs::Scene);
			show_pair(Pairs::Source);
			show_pair(Pairs::Filter);
			break;
		case ActionsClass::Actions::Disable_Source_Filter:
			show_pair(Pairs::Scene);
			show_pair(Pairs::Source);
			show_pair(Pairs::Filter);
			break;
		case ActionsClass::Actions::Set_Gain_Filter:
			show_pair(Pairs::Scene);
			show_pair(Pairs::Source);
			show_pair(Pairs::Filter);
			break;
		case ActionsClass::Actions::Toggle_Source_Filter:
			show_pair(Pairs::Scene);
			show_pair(Pairs::Source);
			show_pair(Pairs::Filter);
			break;
		case ActionsClass::Actions::Reset_Scene_Item:
			show_pair(Pairs::Scene);
			show_pair(Pairs::Source);
			break;
		case ActionsClass::Actions::Set_Scene_Item_Render:
			show_pair(Pairs::Scene);
			show_pair(Pairs::Source);
			show_pair(Pairs::Item);
			break;
		case ActionsClass::Actions::Set_Scene_Item_Position:
			show_pair(Pairs::Scene);
			show_pair(Pairs::Item);
			break;
		case ActionsClass::Actions::Set_Scene_Item_Transform:
			show_pair(Pairs::Scene);
			show_pair(Pairs::Item);
			break;
		case ActionsClass::Actions::Set_Scene_Item_Crop:
			show_pair(Pairs::Scene);
			show_pair(Pairs::Item);
			break;
		case ActionsClass::Actions::Set_Scene_Transition_Override:
			show_pair(Pairs::Scene);
			show_pair(Pairs::Transition);
			break;
		case ActionsClass::Actions::Set_Current_Transition:
			show_pair(Pairs::Transition);
			break;
		case ActionsClass::Actions::Set_Volume:
			show_pair(Pairs::Audio);
			break;
		case ActionsClass::Actions::Set_Mute:
			show_pair(Pairs::Audio);
			break;
		case ActionsClass::Actions::Toggle_Mute:
			show_pair(Pairs::Audio);
			break;
		case ActionsClass::Actions::Set_Source_Filter_Visibility:
			show_pair(Pairs::Scene);
			show_pair(Pairs::Source);
			show_pair(Pairs::Filter);
			break;
		case ActionsClass::Actions::Take_Source_Screenshot:
			show_pair(Pairs::Source);
			show_pair(Pairs::Scene);
			break;
		case ActionsClass::Actions::Play_Pause_Media:
			show_pair(Pairs::Media);
			break;
		case ActionsClass::Actions::Restart_Media:
			show_pair(Pairs::Media);
			break;
		case ActionsClass::Actions::Stop_Media:
			show_pair(Pairs::Media);
			break;
		case ActionsClass::Actions::Next_Media:
			show_pair(Pairs::Media);
			break;
		case ActionsClass::Actions::Previous_Media:
			show_pair(Pairs::Media);
			break;
		case ActionsClass::Actions::Set_Media_Time:
			show_pair(Pairs::Media);
			break;
		case ActionsClass::Actions::Scrub_Media:
			show_pair(Pairs::Media);
			break;
		case ActionsClass::Actions::Toggle_Source_Visibility: {
			show_pair(Pairs::Scene);
			show_pair(Pairs::Source);
		} break;
		default:
			hide_all_pairs();
			break;
		}
	}
}
void PluginWindow::set_edit_mode() {
}
void PluginWindow::save_edit() {}

bool PluginWindow::map_exists()
{
	auto devicemanager = GetDeviceManager();
	auto hooks = devicemanager->GetMidiHooksByDeviceName(ui->mapping_lbl_device_name->text());
	for (int i = 0; i < hooks.size(); i++) {
		if ((hooks.at(i)->channel == ui->sb_channel->value()) && (hooks.at(i)->norc == ui->sb_norc->value()) &&
		    (hooks.at(i)->message_type == ui->cb_mtype->currentText())) {
			return true;
		}
	}
	return false;
}
int PluginWindow::find_mapping_location(const MidiMessage &message)
{
	auto devicemanager = GetDeviceManager();
	auto hooks = devicemanager->GetMidiHooksByDeviceName(ui->mapping_lbl_device_name->text());
	for (int i = 0; i < hooks.size(); i++) {
		if ((hooks.at(i)->channel == message.channel) && (hooks.at(i)->norc == message.NORC) && (hooks.at(i)->message_type == message.message_type)) {
			return i;
		}
	}
	return -1;
}
void PluginWindow::add_new_mapping()
{
	ui->btn_Listen_many->setChecked(false);
	ui->btn_Listen_one->setChecked(false);
	QColor midic;
	midic.setRgb(0, 170, 255);
	QColor actc;
	actc.setRgb(170, 0, 255);
	if (!map_exists() && verify_mapping() && ui->sb_channel->value() != 0) {
		int row = ui->table_mapping->rowCount();
		ui->table_mapping->insertRow(row);

		// don't delete it, because the table takes ownership of the items
		QTableWidgetItem *channelitem = new QTableWidgetItem(QString::number(ui->sb_channel->value()));
		QTableWidgetItem *mtypeitem = new QTableWidgetItem(ui->cb_mtype->currentText());
		QTableWidgetItem *norcitem = new QTableWidgetItem(QString::number(ui->sb_norc->value()));
		QTableWidgetItem *actionitem = new QTableWidgetItem(ui->cb_obs_output_action->currentText());
		QTableWidgetItem *sceneitem = new QTableWidgetItem(ui->cb_obs_output_scene->currentText());
		QTableWidgetItem *sourceitem = new QTableWidgetItem(ui->cb_obs_output_source->currentText());
		QTableWidgetItem *filteritem = new QTableWidgetItem(ui->cb_obs_output_filter->currentText());
		QTableWidgetItem *transitionitem = new QTableWidgetItem(ui->cb_obs_output_transition->currentText());
		QTableWidgetItem *itemitem = new QTableWidgetItem(ui->cb_obs_output_item->currentText());
		QTableWidgetItem *audioitem = new QTableWidgetItem(ui->cb_obs_output_audio_source->currentText());
		QTableWidgetItem *mediaitem = new QTableWidgetItem(ui->cb_obs_output_media_source->currentText());
		set_cell_colors(midic, channelitem);
		set_cell_colors(midic, mtypeitem);
		set_cell_colors(midic, norcitem);
		set_cell_colors(actc, actionitem);
		set_cell_colors(actc, sceneitem);
		set_cell_colors(actc, sourceitem);
		set_cell_colors(actc, filteritem);
		set_cell_colors(actc, transitionitem);
		set_cell_colors(actc, itemitem);
		set_cell_colors(actc, audioitem);
		set_cell_colors(actc, mediaitem);
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
		newmh->filter = ui->cb_obs_output_filter->currentText();
		newmh->transition = ui->cb_obs_output_transition->currentText();
		newmh->item = ui->cb_obs_output_item->currentText();
		newmh->audio_source = ui->cb_obs_output_audio_source->currentText();
		newmh->media_source = ui->cb_obs_output_media_source->currentText();
		auto dm = GetDeviceManager();
		auto dev = dm->GetMidiDeviceByName(ui->mapping_lbl_device_name->text());
		dev->add_MidiHook(newmh);
		auto conf = GetConfig();
		conf->Save();
		ui->table_mapping->selectRow(row);
	} else {
		if (ui->sb_channel->value()) {
			Utils::alert_popup("Can Not Map Channel 0. \nPlease Click Listen One or Listen Many to listen for MIDI Event to map");
		}
		if (!verify_mapping()) {
			Utils::alert_popup("Mapping Missing required variable");
		}
		if (map_exists()) {
			// TODO: Fix this, and create Utils::message_to_user(QString)
			QString Mess;
			Mess.append("Mapping already Exists for ");
			Mess.append(ui->mapping_lbl_device_name->text());
			Mess.append(" , with channel # ");
			Mess.append(QString::number(ui->sb_channel->value()));
			Mess.append(" norc # ");
			Mess.append(QString::number(ui->sb_norc->value()));
			Mess.append(" and Message Type ");
			Mess.append(ui->cb_mtype->currentText());
			Utils::alert_popup(Mess);
		}
	}
}
void PluginWindow::add_row_from_hook(MidiHook *hook)
{
	int row = ui->table_mapping->rowCount();
	ui->table_mapping->insertRow(row);
	QColor midic;
	midic.setRgb(0, 170, 255);
	QColor actc;
	actc.setRgb(170, 0, 255);

	// don't delete it, because the table takes ownership of the items
	QTableWidgetItem *channelitem = new QTableWidgetItem(QString::number(hook->channel));
	QTableWidgetItem *mtypeitem = new QTableWidgetItem(hook->message_type);
	QTableWidgetItem *norcitem = new QTableWidgetItem(QString::number(hook->norc));
	QTableWidgetItem *actionitem = new QTableWidgetItem(hook->action);
	QTableWidgetItem *sceneitem = new QTableWidgetItem(hook->scene);
	QTableWidgetItem *sourceitem = new QTableWidgetItem(hook->source);
	QTableWidgetItem *filteritem = new QTableWidgetItem(hook->filter);
	QTableWidgetItem *transitionitem = new QTableWidgetItem(hook->transition);
	QTableWidgetItem *itemitem = new QTableWidgetItem(hook->item);
	QTableWidgetItem *audioitem = new QTableWidgetItem(hook->audio_source);
	QTableWidgetItem *mediaitem = new QTableWidgetItem(hook->media_source);
	set_cell_colors(midic, channelitem);
	set_cell_colors(midic, mtypeitem);
	set_cell_colors(midic, norcitem);
	set_cell_colors(actc, actionitem);
	set_cell_colors(actc, sceneitem);
	set_cell_colors(actc, sourceitem);
	set_cell_colors(actc, filteritem);
	set_cell_colors(actc, transitionitem);
	set_cell_colors(actc, itemitem);
	set_cell_colors(actc, audioitem);
	set_cell_colors(actc, mediaitem);
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
void PluginWindow::set_cell_colors(const QColor &color, QTableWidgetItem *item)
{
	QColor txcolor;
	txcolor.black();
	item->setBackground(txcolor);
	item->setForeground(color);
}
void PluginWindow::tab_changed(int tab)
{
	reset_to_defaults();
	if (tab == 1)
		ui->mapping_lbl_device_name->setText(ui->list_midi_dev->currentItem()->text());
	clear_table();
	load_table();
}
void PluginWindow::clear_table()
{
	ui->table_mapping->clearContents();
	set_headers();
	ui->table_mapping->setRowCount(0);
}
void PluginWindow::load_table()
{
	auto devicemanager = GetDeviceManager();
	auto hooks = devicemanager->GetMidiHooksByDeviceName(ui->mapping_lbl_device_name->text());
	if (hooks.count() > 0) {
		for (int i = 0; i < hooks.size(); i++) {
			add_row_from_hook(hooks.at(i));
		}
	}
}

void PluginWindow::delete_mapping()
{
	if (ui->table_mapping->rowCount() > 0) {
		int row = ui->table_mapping->selectedItems().at(0)->row();
		auto devicemanager = GetDeviceManager();
		auto dev = devicemanager->GetMidiDeviceByName(ui->mapping_lbl_device_name->text());
		auto hooks = dev->GetMidiHooks();
		auto conf = GetConfig();
		for (int i = 0; i < hooks.size(); i++) {
			if ((hooks.at(i)->channel == ui->sb_channel->value()) && (hooks.at(i)->norc == ui->sb_norc->value()) &&
			    (hooks.at(i)->message_type == ui->cb_mtype->currentText())) {
				dev->remove_MidiHook(hooks.at(i));
				conf->Save();
				ui->table_mapping->removeRow(row);
				ui->table_mapping->clearSelection();
			}
		}
	}
}
void PluginWindow::edit_mapping()
{
	if (ui->table_mapping->rowCount() != 0) {
		auto sitems = ui->table_mapping->selectedItems();
		//rebuild midi
		ui->sb_channel->setValue(sitems.at(0)->text().toInt());
		ui->cb_mtype->setCurrentText(sitems.at(1)->text());
		ui->sb_norc->setValue(sitems.at(2)->text().toInt());
		//rebuild actions
		ui->cb_obs_output_action->setCurrentText(sitems.at(3)->text());
		ui->cb_obs_output_scene->setCurrentText(sitems.at(4)->text());
		ui->cb_obs_output_source->setCurrentText(sitems.at(5)->text());
		ui->cb_obs_output_filter->setCurrentText(sitems.at(6)->text());
		ui->cb_obs_output_transition->setCurrentText(sitems.at(7)->text());
		ui->cb_obs_output_item->setCurrentText(sitems.at(8)->text());
		ui->cb_obs_output_audio_source->setCurrentText(sitems.at(9)->text());
		ui->cb_obs_output_media_source->setCurrentText(sitems.at(10)->text());
	}
}
bool PluginWindow::verify_mapping()
{
	int testresults = 0;
	if (ui->cb_obs_output_audio_source->isVisible() && ui->cb_obs_output_audio_source->count() == 0) {
		testresults++;
	}
	if (ui->cb_obs_output_scene->isVisible() && ui->cb_obs_output_scene->count() == 0) {
		testresults++;
	}
	if (ui->cb_obs_output_source->isVisible() && ui->cb_obs_output_source->count() == 0) {
		testresults++;
	}
	if (ui->cb_obs_output_filter->isVisible() && ui->cb_obs_output_filter->count() == 0) {
		testresults++;
	}
	if (ui->cb_obs_output_transition->isVisible() && ui->cb_obs_output_transition->count() == 0) {
		testresults++;
	}
	if (ui->cb_obs_output_item->isVisible() && ui->cb_obs_output_item->count() == 0) {
		testresults++;
	}
	if (ui->cb_obs_output_media_source->isVisible() && ui->cb_obs_output_media_source->count() == 0) {
		testresults++;
	}
	if (testresults > 0) {
		return false;
	} else {
		return true;
	}
}
void PluginWindow::on_scene_change(const QString &newscene)
{
	if (ui->cb_obs_output_source->isVisible()) {
		ui->cb_obs_output_source->clear();
		ui->cb_obs_output_source->addItems(Utils::get_source_names(newscene));
	}
}
void PluginWindow::on_source_change(const QString &newsource)
{
	if (ui->cb_obs_output_filter->isVisible()) {
		ui->cb_obs_output_filter->clear();
		ui->cb_obs_output_filter->addItems(Utils::get_filter_names(newsource));
	}
}
