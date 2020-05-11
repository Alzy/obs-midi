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

#include "settings-midi-map.h"

#include <qdialogbuttonbox.h>
#include <QDialog>
#include <qcombobox.h>
#include <qlabel.h>
#include <qcheckbox.h>
#include <QMessageBox>

#define CHANGE_ME "changeme"

SettingsMidiMap::SettingsMidiMap(QWidget *parent)
	:
	QDialog(parent, Qt::Dialog), ui(new Ui::SettingsMidiMap)
{

	ui->setupUi(this);
	
	SettingsMidiMap::AddRow("control_change", 1);
	SettingsMidiMap::AddRow("control_change", 3);
	SettingsMidiMap::AddRow("control_change", 2);
	
}


void SettingsMidiMap::ToggleShowHide()
{
	if (!isVisible())
		setVisible(true);
	else
		setVisible(false);
}



void SettingsMidiMap::FormAccepted()
{
	return;
}

SettingsMidiMap::~SettingsMidiMap()
{
	delete ui;
}

void SettingsMidiMap::MakeInputTypeCombo(int row, int existing) {
	QComboBox* combo = new QComboBox;
	combo->insertItem(0,"button");
	combo->insertItem(1,"fader");
	if (existing != 0) {
		combo->setCurrentIndex(existing);
	}
	ui->tbl_midimap->setItem(row, 2, new QTableWidgetItem);
	ui->tbl_midimap->setCellWidget(row, 2, combo);
}
void SettingsMidiMap::MakeMtype(int row, std::string Mtype) {
	QLabel *label = new QLabel();
	label->setText(QString::fromStdString(Mtype));
	ui->tbl_midimap->setItem(row, 0, new QTableWidgetItem);
	ui->tbl_midimap->setCellWidget(row, 0, label);
}
void SettingsMidiMap::MakeChannel(int row,int channel) {
	QLabel *label = new QLabel();
	label->setText(QString::number(channel));
	ui->tbl_midimap->setItem(row, 1, new QTableWidgetItem);
	ui->tbl_midimap->setCellWidget(row, 1, label);
	
}
void SettingsMidiMap::MakeInputTypeCombo(int row)
{
	QComboBox *combo = new QComboBox;
	combo->insertItem(0, "button");
	combo->insertItem(1, "fader");

	ui->tbl_midimap->setItem(row, 2, new QTableWidgetItem);
	ui->tbl_midimap->setCellWidget(row, 2, combo);
}
void SettingsMidiMap::MakeBidirectional(int row) {
	QCheckBox *box = new QCheckBox();
	box->setStyleSheet("margin-left:50%; margin-right:50%;");
	ui->tbl_midimap->setItem(row, 3, new QTableWidgetItem);
	ui->tbl_midimap->setCellWidget(row, 3, box);
}
void SettingsMidiMap::MakeFaderActionsCombo(int row, int existing)
{
	QComboBox *combo = new QComboBox;
	combo->insertItem(0, "SetVolume");
	combo->insertItem(1, "SetSyncOffset");
	combo->insertItem(2, "SetSourcePosition");
	combo->insertItem(3, "SetSourceRotation");
	combo->insertItem(4, "SetSourceScale");
	combo->insertItem(5, "SetTransitionDuration");
	combo->insertItem(6, "SetGainFilter");
	if (existing != 0) {
		combo->setCurrentIndex(existing);
	}
	ui->tbl_midimap->setItem(row, 4, new QTableWidgetItem);
	ui->tbl_midimap->setCellWidget(row, 4, combo);
}
void SettingsMidiMap::MakeButtonActionsCombo(int row, int existing)
{
	QComboBox *combo = new QComboBox;
	combo->insertItem(0, "SetCurrentScene");
	combo->insertItem(1, "SetPreviewScene");
	combo->insertItem(2, "TransitionToProgram");
	combo->insertItem(3, "SetCurrentTransition");
	combo->insertItem(4, "SetSourceVisibility");
	combo->insertItem(5, "ToggleSourceVisibility");
	combo->insertItem(6, "ToggleMute");
	combo->insertItem(7, "SetMute");
	combo->insertItem(8, "StartStopStreaming");
	combo->insertItem(9, "StartStreaming");
	combo->insertItem(10, "StopStreaming");
	combo->insertItem(11, "StartStopRecording");
	combo->insertItem(12, "StartRecording");
	combo->insertItem(13, "StopRecording");
	combo->insertItem(14, "StartStopReplayBuffer");
	combo->insertItem(15, "StartReplayBuffer");
	combo->insertItem(16, "StopReplayBuffer");
	combo->insertItem(17, "SaveReplayBuffer");
	combo->insertItem(18, "PauseRecording");
	combo->insertItem(19, "ResumeRecording");
	combo->insertItem(20, "SetTransitionDuration");
	combo->insertItem(21, "SetCurrentProfile");
	combo->insertItem(22, "SetCurrentSceneCollection");
	combo->insertItem(23, "ResetSceneItem");
	combo->insertItem(24, "SetTextGDIPlusText");
	combo->insertItem(25, "SetBrowserSourceURL");
	combo->insertItem(26, "ReloadBrowserSource");
	combo->insertItem(27, "TakeSourceScreenshot");
	combo->insertItem(28, "EnableSourceFilter");
	combo->insertItem(29, "DisableSourceFilter");
	combo->insertItem(30, "ToggleSourceFilter");
	if (existing != 0) {
		combo->setCurrentIndex(existing);
	}
	ui->tbl_midimap->setItem(row, 4, new QTableWidgetItem);
	ui->tbl_midimap->setCellWidget(row, 4, combo);
}
void SettingsMidiMap::MakeOption1(int row) {
	QComboBox *combo = new QComboBox;
	ui->tbl_midimap->setItem(row, 5, new QTableWidgetItem);
	ui->tbl_midimap->setCellWidget(row, 5, combo);
}
void SettingsMidiMap::MakeOption2(int row) {
	QComboBox *combo = new QComboBox;
	ui->tbl_midimap->setItem(row, 6, new QTableWidgetItem);
	ui->tbl_midimap->setCellWidget(row, 6, combo);
}
void SettingsMidiMap::MakeOption3(int row) {
	QComboBox *combo = new QComboBox;
	ui->tbl_midimap->setItem(row, 7, new QTableWidgetItem);
	ui->tbl_midimap->setCellWidget(row, 7, combo);
}

void SettingsMidiMap::AddRow(std::string mtype, int channel) {
	//default Addrow
	int startrow = ui->tbl_midimap->rowCount();
	ui->tbl_midimap->insertRow(startrow);
	SettingsMidiMap::MakeMtype(startrow, mtype);
	SettingsMidiMap::MakeChannel(startrow, channel);
	SettingsMidiMap::MakeBidirectional(startrow);
	if (mtype == "control_change") {
		SettingsMidiMap::MakeInputTypeCombo(startrow, 1);
		SettingsMidiMap::MakeFaderActionsCombo(startrow, 0);	
	} else if (mtype == "note_on" || mtype == "note_off") {
		SettingsMidiMap::MakeInputTypeCombo(startrow, 0);
		SettingsMidiMap::MakeButtonActionsCombo(startrow, 0);	
	
	}
	SettingsMidiMap::MakeOption1(startrow);
	SettingsMidiMap::MakeOption2(startrow);
	SettingsMidiMap::MakeOption3(startrow);

}


void SettingsMidiMap::AddRow(std::string mtype, int channel, int input_type, bool bidirectional,
	    int action)
{
	//addrow for use from saves
	//default Addrow
	int startrow = ui->tbl_midimap->rowCount();
	ui->tbl_midimap->insertRow(startrow);
	SettingsMidiMap::MakeMtype(startrow, mtype);
	SettingsMidiMap::MakeChannel(startrow, channel);
	SettingsMidiMap::MakeBidirectional(startrow);
	if (mtype == "control_channel") {
		SettingsMidiMap::MakeInputTypeCombo(startrow, 1);
		SettingsMidiMap::MakeFaderActionsCombo(startrow, 0);
	} else if (mtype == "note_on" || mtype == "note_off") {
		SettingsMidiMap::MakeInputTypeCombo(startrow, 0);
		SettingsMidiMap::MakeButtonActionsCombo(startrow, 0);
	}
	SettingsMidiMap::MakeOption1(startrow);
	SettingsMidiMap::MakeOption2(startrow);
	SettingsMidiMap::MakeOption3(startrow);
}
