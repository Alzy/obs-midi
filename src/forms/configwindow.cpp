#include <inttypes.h>
#include <obs.hpp>
#include <util/platform.h>
#include "configwindow.h"
#include "ui_configwindow.h"
#include <QtWidgets>
#include <QTableWidget>
#include <obs-frontend-api/obs-frontend-api.h>
#include "obs-midi.h"
#include "config.h"
#include "device-manager.h"
#include "midi-agent.h"

ConfigWindow::ConfigWindow(QString devn) : ui(new Ui::ConfigWindow)
{
	devicename = devn.toStdString();
	//Setup the UI
	ui->setupUi(this);

	auto devicemanager = GetDeviceManager();
	auto config = GetConfig();
	auto device = devicemanager->GetMidiDeviceByName(devicename.c_str());
	auto hooks =
		devicemanager->GetMidiHooksByDeviceName(devicename.c_str());
	///HOOK up the Message Handler
	connect(device,
		SIGNAL(SendNewUnknownMessage(QString, QString, int, int)), this,
		SLOT(domessage(QString, QString, int,
			       int))); /// name, mtype, norc, channel

	this->setWindowTitle(this->windowTitle() + "  " +
			     QString::fromStdString(devicename));
	ui->tableWidget->horizontalHeader()->setSectionResizeMode(
		QHeaderView::Stretch);

	//Add Existing Hooks to table.
	if (hooks.size() > 0) {

		for (int i = 0; i < hooks.size(); i++) {
			int rc = ui->tableWidget->rowCount();
			AddRowFromHooks(
				rc, hooks.at(i)->type, hooks.at(i)->mchan,
				hooks.at(i)->index, hooks.at(i)->bidirectional,
				hooks.at(i)->action, hooks.at(i)->command,
				hooks.at(i)->param1, hooks.at(i)->param2,
				hooks.at(i)->param3);
		}
	}

	//Pull Data From OBS
	MakeVolumeCombo();
	MakeSceneCombo();
	chooseAtype("Button");
	//Connect back button functionality
	connect(this->ui->btnBack, SIGNAL(clicked()), this,
		SLOT(btn_back_clicked()));
	connect(ui->btnSave, SIGNAL(clicked()), this, SLOT(save()));
	connect(ui->tableWidget, SIGNAL(cellClicked(int, int)), this,
		SLOT(select(int, int)));
	connect(ui->cb_atype, SIGNAL(currentTextChanged(QString)), this,
		SLOT(chooseAtype(QString)));
	connect(ui->cb_atype, SIGNAL(currentIndexChanged(int)), this,
		SLOT(sendToTable()));
	connect(ui->cb_action, SIGNAL(currentIndexChanged(int)), this,
		SLOT(sendToTable()));
	connect(ui->cb_param1, SIGNAL(currentIndexChanged(int)), this,
		SLOT(sendToTable()));
	//connect(ui->cb_param2, SIGNAL(currentIndexChanged(int)), this,SLOT(sendToTable()));
	//connect(ui->cb_param3, SIGNAL(currentIndexChanged(int)), this,SLOT(sendToTable()));
	connect(ui->checkBox, SIGNAL(stateChanged(int)), this,
		SLOT(sendToTable()));
	//connect(ui->cb_atype, SIGNAL(currentIndexChanged(int)), this,SLOT(sendToTable()));
	connect(ui->cb_action, SIGNAL(currentTextChanged(QString)), this,
		SLOT(chooseOptions1(QString)));
	connect(ui->btnDel, SIGNAL(clicked()), this, SLOT(deleterow()));
	connect(ui->btnClear, SIGNAL(clicked()), this, SLOT(clearTable()));
	ui->tableWidget->selectRow(0);
}
void ConfigWindow::clearTable()
{
	QMessageBox msgBox;
	msgBox.setText("Are You Sure ??");
	msgBox.setInformativeText("This will Clear all table Entries");
	msgBox.setStandardButtons(QMessageBox::Cancel | QMessageBox::Ok);
	msgBox.setDefaultButton(QMessageBox::Cancel);
	int ret = msgBox.exec();
	switch (ret) {
	case QMessageBox::Ok:
		clearpressed = true;
		ui->tableWidget->setRowCount(0);
		save();
		clearpressed = false;
		break;
	case QMessageBox::Cancel:
		return;
	}
}

void ConfigWindow::select(int row, int col)
{
	dirty = true;
	ui->lin_mtype->setText(ui->tableWidget->item(row, 0)->text());
	ui->channel->display(ui->tableWidget->item(row, 1)->text().toInt());
	ui->num_mchan->display(ui->tableWidget->item(row, 2)->text().toInt());
	ui->checkBox->setChecked(
		QVariant(ui->tableWidget->item(row, 3)->text()).toBool());
	ui->cb_atype->setCurrentText(ui->tableWidget->item(row, 4)->text());
	ui->cb_action->setCurrentText(ui->tableWidget->item(row, 5)->text());
	ui->cb_param1->setCurrentText(ui->tableWidget->item(row, 6)->text());
	ui->cb_param2->setCurrentText(ui->tableWidget->item(row, 7)->text());
	ui->cb_param3->setCurrentText(ui->tableWidget->item(row, 8)->text());
	dirty = false;
}
void ConfigWindow::AddRowFromHooks(int rc, std::string type, int channel,
				   int index, bool bid, std::string action,
				   std::string command, std::string param1,
				   std::string param2, std::string param3)
{

	ui->tableWidget->insertRow(rc);
	QTableWidgetItem *newItem = new QTableWidgetItem();
	QTableWidgetItem *newItem1 = new QTableWidgetItem();
	QTableWidgetItem *newItem2 = new QTableWidgetItem();
	QTableWidgetItem *newItem3 = new QTableWidgetItem();
	QTableWidgetItem *newItem4 = new QTableWidgetItem();
	QTableWidgetItem *newItem5 = new QTableWidgetItem();
	QTableWidgetItem *newItem6 = new QTableWidgetItem();
	QTableWidgetItem *newItem7 = new QTableWidgetItem();
	QTableWidgetItem *newItem8 = new QTableWidgetItem();
	//load rows from hooks
	newItem->setText(QString::fromStdString(type)); //Message Type
	newItem1->setText(QString::number(channel));    //message Channel
	newItem2->setText(QString::number(index));   //message Note or Control
	newItem3->setText(QVariant(bid).toString()); //Bidirectional
	newItem4->setText(QString::fromStdString(action));  //Action Type
	newItem5->setText(QString::fromStdString(command)); //Action
	newItem6->setText(QString::fromStdString(param1));  //Option 1
	newItem7->setText(QString::fromStdString(param2));  //Option 2
	newItem8->setText(QString::fromStdString(param3));  //Option 3
	//Set items
	ui->tableWidget->setItem(rc, 0, newItem);
	ui->tableWidget->setItem(rc, 1, newItem1);
	ui->tableWidget->setItem(rc, 2, newItem2);
	ui->tableWidget->setItem(rc, 3, newItem3);
	ui->tableWidget->setItem(rc, 4, newItem4);
	ui->tableWidget->setItem(rc, 5, newItem5);
	ui->tableWidget->setItem(rc, 6, newItem6);
	ui->tableWidget->setItem(rc, 7, newItem7);
	ui->tableWidget->setItem(rc, 8, newItem8);
	//Set Default sidebar
	if (rc == 1) {
		select(0, 1);
	}
}

//Create Default row
void ConfigWindow::insertRow(QString mtype, int mindex, int channel)
{
	int rc = ui->tableWidget->rowCount();
	ui->tableWidget->insertRow(rc);
	QTableWidgetItem *newItem = new QTableWidgetItem();
	QTableWidgetItem *newItem1 = new QTableWidgetItem();
	QTableWidgetItem *newItem2 = new QTableWidgetItem();
	QTableWidgetItem *newItem3 = new QTableWidgetItem();
	QTableWidgetItem *newItem4 = new QTableWidgetItem();
	QTableWidgetItem *newItem5 = new QTableWidgetItem();
	QTableWidgetItem *newItem6 = new QTableWidgetItem();
	QTableWidgetItem *newItem7 = new QTableWidgetItem();
	QTableWidgetItem *newItem8 = new QTableWidgetItem();

	newItem->setText(mtype);                     //Message Type
	newItem1->setText(QString::number(channel)); //message channel
	newItem2->setText(QString::number(mindex));  //message channel
	newItem3->setText("false");                  //Bidirectional

	if (mtype == "control_change") {
		newItem4->setText("Fader"); //Action Type
		newItem5->setText(
			"Set Volume"); //Action	tm_actiontype.append("Fader");

	} else {
		newItem4->setText("Button");   //Action Type
		newItem5->setText("Set Mute"); //Action
	}
	newItem6->setText("Mic/Aux"); //Option 1
	newItem7->setText("");        //Option 2
	newItem8->setText("");        //Option 3

	ui->tableWidget->setItem(rc, 0, newItem);
	ui->tableWidget->setItem(rc, 1, newItem1);
	ui->tableWidget->setItem(rc, 2, newItem2);
	ui->tableWidget->setItem(rc, 3, newItem3);
	ui->tableWidget->setItem(rc, 4, newItem4);
	ui->tableWidget->setItem(rc, 5, newItem5);
	ui->tableWidget->setItem(rc, 6, newItem6);
	ui->tableWidget->setItem(rc, 7, newItem7);
	ui->tableWidget->setItem(rc, 8, newItem8);
}
void ConfigWindow::save()
{
	//Get Device Manager
	auto dm = GetDeviceManager();
	auto conf = GetConfig();
	//to get device
	auto dev = dm->GetMidiDeviceByName(devicename.c_str());
	dev->ClearMidiHooks();
	//get row count
	int rc = ui->tableWidget->rowCount();
	//loop through rows
	for (int i = 0; i < rc; i++) {
		//make default midihook
		MidiHook *mh = new MidiHook;
		//map values
		mh->type = ui->tableWidget->item(i, 0)->text().toStdString();
		mh->mchan = ui->tableWidget->item(i, 1)->text().toInt();
		mh->index = ui->tableWidget->item(i, 2)->text().toInt();
		mh->bidirectional =
			QVariant(ui->tableWidget->item(i, 3)->text()).toBool();
		mh->action = ui->tableWidget->item(i, 4)->text().toStdString();
		mh->command = ui->tableWidget->item(i, 5)->text().toStdString();
		mh->param1 = ui->tableWidget->item(i, 6)->text().toStdString();
		mh->param2 = ui->tableWidget->item(i, 7)->text().toStdString();
		mh->param3 = ui->tableWidget->item(i, 8)->text().toStdString();
		dev->AddMidiHook(mh);
	};
	conf->Save();
	conf->Load();
	reloadEvents();
	if (!clearpressed) {

		//ui->btnBack->click();
		btn_back_clicked();
	}
}
/************************** Message Handler for UI ***************************************/
void ConfigWindow::domessage(QString namein, QString mtype, int norc,
			     int channel)
{
	if (namein == QString::fromStdString(devicename)) {
		if (inrow(norc, mtype, channel)) {
			blog(1, "domessage");
			insertRow(mtype, norc, channel);
		} else {
			ui->tableWidget->selectRow(
				getRow(norc, mtype, channel));
		}
	}
}
int ConfigWindow::getRow(int norc, QString mtype, int channel)
{

	auto fitems = ui->tableWidget->findItems(QString::number(norc), 0);
	int itemcount = fitems.size();
	for (int i = 0; i < itemcount; ++i) {
		// Only Check Collumn 2 (Note or control)

		if (fitems.at(i)->column() == 2) {

			if (ui->tableWidget->item(fitems.at(i)->row(), 0)
				    ->text() == mtype) {
				return fitems.at(i)->row();
			}
		}
	}
	return -1;
}
/************Checks if item exists in tow*************/
bool ConfigWindow::inrow(int x)
{
	int rows = ui->tableWidget->rowCount();
	for (int i = 0; i < rows; ++i) {
		if (ui->tableWidget->item(i, 2)->text() == QString::number(x)) {
			return true;
		}
	}
	return false;
}
bool ConfigWindow::inrow(int norc, QString mtype, int channel)
{
	auto fitems = ui->tableWidget->findItems(QString::number(norc), 0);
	int itemcount = fitems.size();
	for (int i = 0; i < itemcount; ++i) {
		// Only Check Collumn 2 (Note or control)

		if (fitems.at(i)->column() == 2) {

			if (ui->tableWidget->item(fitems.at(i)->row(), 0)
				    ->text() == mtype) {
				return false;
			}
		}
	}
	return true;
}

void ConfigWindow::sendToTable()
{
	if (!dirty) {
		if (ui->tableWidget->rowCount() > 0) {
			int rc = ui->tableWidget->selectedItems()[0]->row();
			ui->tableWidget->item(rc, 0)->setText(
				ui->lin_mtype->text()); //mtype

			ui->tableWidget->item(rc, 1)->setText(QString::number(
				ui->channel->intValue())); //Channel
			ui->tableWidget->item(rc, 2)->setText(QString::number(
				ui->num_mchan->intValue())); //mindex
			ui->tableWidget->item(rc, 3)->setText(
				QVariant(ui->checkBox->isChecked())
					.toString()); //bool
			ui->tableWidget->item(rc, 4)->setText(
				ui->cb_atype->currentText()); //atype
			ui->tableWidget->item(rc, 5)->setText(
				ui->cb_action->currentText()); //action
			ui->tableWidget->item(rc, 6)->setText(
				ui->cb_param1->currentText());
			ui->tableWidget->item(rc, 7)->setText(
				ui->cb_param2->currentText());
			ui->tableWidget->item(rc, 8)->setText(
				ui->cb_param3->currentText());
		} // If rowcount  > 0
	}         //Dirty
} //Send to Table

//Back Button handler
void ConfigWindow::btn_back_clicked()
{

	close();
	//setVisible(false);
}

ConfigWindow::~ConfigWindow()
{
	delete ui;
}
void ConfigWindow::ToggleShowHide()
{
	if (!isVisible())
		setVisible(true);
	else
		setVisible(false);
}

/*                Make Combo list models
*/
void ConfigWindow::chooseOptions1(QString Action)
{
	if (ui->tableWidget->rowCount() > 0) {
		ui->cb_param1->clear();
		ui->cb_param2->clear();
		ui->cb_param3->clear();
		if (Action == "Set Volume") {
			ui->cb_param1->addItems(VolumeList);
		} else if (Action == "Set Current Scene") {
			ui->cb_param1->addItems(ScenesList);
		} else if (Action == "Set Preview Scene") {
			ui->cb_param1->addItems(ScenesList);
		} else if (Action == "Set Mute") {
			ui->cb_param1->addItems(VolumeList);
		} else if (Action == "Toggle Mute") {
			ui->cb_param1->addItems(VolumeList);
		}
	}
}
// Pulls Scenes from OBS, Makes Scenes list.
void ConfigWindow::MakeSceneCombo()
{
	auto scenes = Utils::GetScenes();
	auto length = obs_data_array_count(scenes);
	for (size_t i = 0; i < length; i++) {
		auto d = obs_data_array_item(scenes, i);
		auto name = obs_data_get_string(d, "name");
		ScenesList << tr(name);
	}
}

// Pulls volume sources from OBS, Makes Volume sources list.
void ConfigWindow::MakeVolumeCombo()
{
	auto utilsources = Utils::GetAudioSourceNames();
	for (int i = 0; i < utilsources.size(); i++) {
		VolumeList.append(utilsources.at(i));
	}
}
// Choose Action Type Handler
void ConfigWindow::chooseAtype(QString text)
{
	QStringList items;
	if (text == "Button") {
		items = ButtonAList;
	} else if (text == "Fader") {
		items = FaderAList;
	}
	ui->cb_action->clear();
	ui->cb_action->addItems(items);
}
// Delete row in table.
void ConfigWindow::deleterow()
{
	try {
		auto items = ui->tableWidget->selectedItems();
		if (!items.isEmpty()) {
			int rc = items[0]->row();
			ui->tableWidget->removeRow(rc);
		}

	} catch (const std::exception &e) {
		return;
	}
};
