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

ConfigWindow::ConfigWindow(std::string devn) 
{
	//MakeSceneCombo();
	
	devicename = devn;
	//auto rob = static_cast<RouterPtr>(GetRouter());
	auto devicemanager = GetDeviceManager();
	auto config = GetConfig();
	//config->Load();
	Router *rt = midiobsrouter;	
	//connect(this, SIGNAL(&SendNewUnknownMessage), this	SIGNAL(rt.UnknownMessage));
	//auto device = devicemanager->GetMidiDeviceByName(devicename.c_str());

	std::vector<MidiHook *> hooks =	devicemanager->GetMidiHooksByDeviceName(devicename.c_str());
	///HOOK up the Message Handler
	connect( rt, SIGNAL(UnknownMessage(QString, int)), this,SLOT(domessage(QString, int)));
	//Setup the UI
	ui.setupUi(this);
	//void SetupModel();
	
	ui.tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	
	
		for (int i = 0; i < hooks.size(); i++) {
		int rc = ui.tableWidget->rowCount();
		AddRowFromHooks(rc,hooks.at(i)->type, hooks.at(i)->index, false,
				hooks.at(i)->action, hooks.at(i)->command,
				hooks.at(i)->param1, hooks.at(i)->param2,
				hooks.at(i)->param3);
	}
	
	
	

	MakeVolumeCombo();
	MakeSceneCombo();
	chooseAtype("Button");
	//Connect back button functionality
	connect(ui.btnBack, &QPushButton::clicked, this,
		&ConfigWindow::on_btn_back_clicked);
	connect(ui.btnSave, SIGNAL(clicked()), this, SLOT(save()));
	connect(ui.tableWidget, SIGNAL(cellClicked(int, int)), this, SLOT(select(int,int)));
	connect(ui.cb_atype, SIGNAL(currentTextChanged(QString)), this, SLOT(chooseAtype(QString)));
	connect(ui.cb_atype, SIGNAL(currentIndexChanged(int)), this,SLOT(sendToTable()));
	connect(ui.cb_action, SIGNAL(currentIndexChanged(int)), this,SLOT(sendToTable()));
	connect(ui.cb_param1, SIGNAL(currentIndexChanged(int)), this,SLOT(sendToTable()));
	//connect(ui.cb_param2, SIGNAL(currentIndexChanged(int)), this,SLOT(sendToTable()));
	//connect(ui.cb_param3, SIGNAL(currentIndexChanged(int)), this,SLOT(sendToTable()));
	//connect(ui.checkBox, SIGNAL(stateChanged(int)), this,SLOT(sendToTable()));
	//connect(ui.cb_atype, SIGNAL(currentIndexChanged(int)), this,SLOT(sendToTable()));

	connect(ui.cb_action, SIGNAL(currentTextChanged(QString)), this, SLOT(chooseOptions1(QString)));

	connect(ui.btnDel, SIGNAL(clicked()), this, SLOT(deleterow()));
	connect(ui.btnClear, SIGNAL(clicked()), ui.tableWidget,
		SLOT(clearContents()));

	
	
}
void ConfigWindow::select(int row, int col) {
	ui.lin_mtype->setText(ui.tableWidget->item(row, 0)->text());
	ui.num_mchan->display(ui.tableWidget->item(row, 1)->text().toInt());
	ui.checkBox->setChecked(QVariant(ui.tableWidget->item(row, 2)->text()).toBool());
	ui.cb_atype->setCurrentText(ui.tableWidget->item(row, 3)->text());
	//chooseAtype(ui.cb_atype->currentText());
	ui.cb_action->setCurrentText(ui.tableWidget->item(row, 4)->text());
	//chooseOptions1(ui.cb_action->currentText());
	ui.cb_param3->setCurrentText(ui.tableWidget->item(row, 5)->text());
	ui.cb_param3->setCurrentText(ui.tableWidget->item(row, 6)->text());
	ui.cb_param3->setCurrentText(ui.tableWidget->item(row, 7)->text());

}
void ConfigWindow::AddRowFromHooks(int rc, std::string type, int index, bool bid,
			     std::string action, std::string command,
			     std::string param1, std::string param2,
			     std::string param3)
{

		ui.tableWidget->insertRow(rc);
		QTableWidgetItem *newItem = new QTableWidgetItem();
		QTableWidgetItem *newItem2 = new QTableWidgetItem();
		QTableWidgetItem *newItem3 = new QTableWidgetItem();
		QTableWidgetItem *newItem4 = new QTableWidgetItem();
		QTableWidgetItem *newItem5 = new QTableWidgetItem();
		QTableWidgetItem *newItem6 = new QTableWidgetItem();
		QTableWidgetItem *newItem7 = new QTableWidgetItem();
		QTableWidgetItem *newItem8 = new QTableWidgetItem();

		
		//load rows from hooks
		//bidirectional.append(false);

		
		newItem->setText(QString::fromStdString(type)); //Message Type
		newItem2->setText(QString::number(index)); //message channel
		newItem3->setText(QVariant(false).toString()); //Bidirectional
		newItem4->setText(QString::fromStdString(action)); //Action Type
		newItem5->setText(QString::fromStdString(command)); //Action
		newItem6->setText(QString::fromStdString(param1)); //Option 1
		newItem7->setText(QString::fromStdString(param2)); //Option 2
		newItem8->setText(QString::fromStdString(param3));        //Option 3

		//if (!tm_messagenumber.contains(mindex)) {

		ui.tableWidget->setItem(rc, 0, newItem);
		ui.tableWidget->setItem(rc, 1, newItem2);
		ui.tableWidget->setItem(rc, 2, newItem3);
		ui.tableWidget->setItem(rc, 3, newItem4);
		ui.tableWidget->setItem(rc, 4, newItem5);
		ui.tableWidget->setItem(rc, 5, newItem6);
		ui.tableWidget->setItem(rc, 6, newItem7);
		ui.tableWidget->setItem(rc, 7, newItem8);	
}
void  ConfigWindow::insertRow(QString mtype,int mindex)
{

	int rc = ui.tableWidget->rowCount();
	ui.tableWidget->insertRow(rc );
	QTableWidgetItem *newItem = new QTableWidgetItem();
	QTableWidgetItem *newItem2 = new QTableWidgetItem();
	QTableWidgetItem *newItem3 = new QTableWidgetItem();
	QTableWidgetItem *newItem4 = new QTableWidgetItem();
	QTableWidgetItem *newItem5 = new QTableWidgetItem();
	QTableWidgetItem *newItem6 = new QTableWidgetItem();
	QTableWidgetItem *newItem7 = new QTableWidgetItem();
	QTableWidgetItem *newItem8 = new QTableWidgetItem();
	
	newItem->setText(mtype); //Message Type
	newItem2->setText(QString::number(mindex));   //message channel
	//newItem3->setCheckState(false); //Bidirectional
	
	if (mtype == "control_change") {
		newItem4->setText("Fader"); //Action Type
		newItem5->setText("Set Volume"); //Action	tm_actiontype.append("Fader");
		
	} else {
		newItem4->setText("Button");  //Action Type
		newItem5->setText("Set Mute"); //Action
		
	}
	newItem6->setText("Mic/Aux");   //Option 1
	newItem7->setText("");   //Option 2
	newItem8->setText("");   //Option 3

	
	
	//if (!tm_messagenumber.contains(mindex)) {
	
	
	ui.tableWidget->setItem(rc , 0, newItem);
	ui.tableWidget->setItem(rc, 1, newItem2);
	ui.tableWidget->setItem(rc, 2, newItem3);
	ui.tableWidget->setItem(rc, 3, newItem4);
	ui.tableWidget->setItem(rc, 4, newItem5);
	ui.tableWidget->setItem(rc, 5, newItem6);
	ui.tableWidget->setItem(rc, 6, newItem7);
	ui.tableWidget->setItem(rc, 7, newItem8);

	
}
void ConfigWindow::save() {
	//Get Device Manager
	auto dm = GetDeviceManager();
	auto conf = GetConfig();
	//to get device
	auto dev = dm->GetMidiDeviceByName(devicename.c_str());
	dev->ClearMidiHooks();
	//get row count
	int rc =ui.tableWidget->rowCount();
	//loop through rows
	for (int i=0; i < rc; i++)
	{
	//make default midihook
		
		MidiHook *mh = new MidiHook;
		
		//map values
		mh->type = ui.tableWidget->item(i, 0)->text().toStdString();
		mh->index =ui.tableWidget->item(i, 1)->text().toInt();
		//mh->bidirectional = ui.tableWidget->item(i, 2)->text;
		mh->action = ui.tableWidget->item(i, 3)->text().toStdString();
		mh->command = ui.tableWidget->item(i, 4)->text().toStdString();
		mh->param1 = ui.tableWidget->item(i, 5)->text().toStdString();
		mh->param2 = ui.tableWidget->item(i, 6)->text().toStdString();
		mh->param3 = ui.tableWidget->item(i, 7)->text().toStdString();
		
		dev->AddMidiHook(mh);
		
	};
	conf->Save();
}
	void ConfigWindow::domessage(QString mtype, int mchan)
{
	
	blog(1, "domessage");
	if (inrow(mchan, mtype)) {
		
			insertRow(mtype, mchan);
		
		
	}
}
bool ConfigWindow::inrow(int x) {

int rows = ui.tableWidget->rowCount();

	for (int i = 0; i < rows; ++i) {
		if (ui.tableWidget->item(i, 1)->text() ==
		    QString::number(x)) {
			return true;
		}

	}
	return false;
}
bool ConfigWindow::inrow(int x, QString mtype)
{
	auto fitems = ui.tableWidget->findItems(QString::number(x), 0);
	int rows = ui.tableWidget->rowCount();
	int itemcount = fitems.size();
		for (int i = 0; i < itemcount; ++i) {
		
		if (ui.tableWidget->item(fitems.at(i)->row(), 0)
				    ->text() == mtype) {
				return false;
			}
			
		}
	
	return true;
	
	
}


void ConfigWindow::sendToTable() {
	if (ui.tableWidget->rowCount() > 0) {
	
	int rc =ui.tableWidget->selectedItems()[0]->row();
	ui.tableWidget->item(rc, 0)->setText(ui.lin_mtype->text());//mtype
	ui.tableWidget->item(rc, 1)->setText(QString::number(ui.num_mchan->intValue()));               //mindex
	ui.tableWidget->item(rc, 2)->setText(QVariant(ui.checkBox->isChecked()).toString()); //bool
	ui.tableWidget->item(rc, 3)->setText(ui.cb_atype->currentText());//atype
	ui.tableWidget->item(rc, 4)->setText(ui.cb_action->currentText());   //action
	ui.tableWidget->item(rc, 5)->setText(ui.cb_param1->currentText());
	ui.tableWidget->item(rc, 6)->setText(ui.cb_param2->currentText());
	ui.tableWidget->item(rc, 7)->setText(ui.cb_param3->currentText());
	}
}






	// Create a method to populate the model with data:


void ConfigWindow::on_btn_back_clicked()
{

	setVisible(false);
	//&parentWidget->show()
	//	parent::show();
}

ConfigWindow::~ConfigWindow()
{
	
}
void ConfigWindow::ToggleShowHide()
{
	if (!isVisible())
		setVisible(true);
	else
		setVisible(false);
}

void ConfigWindow::loadFromHooks()
{
	


}
/*bool ConfigWindow::insertRow(int row, std::string  mtype, int mindex, std::string actiontype,
		std::string action, std::string option1, std::string option2,
		std::string option3, const QModelIndex &parent)*/



/*                Make Combo list models
*/
void ConfigWindow::chooseOptions1(QString Action) {
	if (ui.tableWidget->rowCount() > 0) {

		//ui.tableWidget->item(ui.tableWidget->selectedItems()[0]->row(), 4)->setText(Action);
		QList<QString> option1;
		QList<QString> option2;
		QList<QString> volume;
		QList<QString> scenes;
		
		QStringList nada;
		ui.cb_param1->clear();
		ui.cb_param2->clear();
		ui.cb_param3->clear();
		if (Action == "Set Volume") {
			ui.cb_param1->addItems(VolumeList);
		} else if (Action == "Set Current Scene") {
			ui.cb_param1->addItems(ScenesList);
		} else if (Action == "Set Preview Scene") {
			ui.cb_param1->addItems(ScenesList);
		} else if (Action == "Set Mute") {
			ui.cb_param1->addItems(VolumeList);
		} else if (Action == "Toggle Mute") {
			ui.cb_param1->addItems(VolumeList);
		}
	}
}
/*
tm_option2.clear();
tm_option2 = option2;
tm_option3.clear();
tm_option3 = option3;
*/
void ConfigWindow::MakeSceneCombo()
{
	auto scenes = Utils::GetScenes();
	auto length = obs_data_array_count(scenes);
	for (size_t i = 0; i < length; i++) {
		auto d = obs_data_array_item(scenes, i);
		auto name = obs_data_get_string(d, "name");
		ScenesList<< tr(name);
	}
	
	//ui.cb_param1->setModel(options1model);
}


void ConfigWindow::MakeVolumeCombo()
{

	/*
	QMap<const char *, int> sources;
	sources["desktop-1"] = 1;
	sources["desktop-2"] = 2;
	sources["mic-1"] = 3;
	sources["mic-2"] = 4;
	sources["mic-3"] = 5;

	QMapIterator<const char *, int> i(sources);
	while (i.hasNext()) {
		i.next();

		OBSSourceAutoRelease source = obs_get_output_source(i.value());
		if (source) {
			VolumeList.append(obs_source_get_name(source));
		}
	}
	*/
	//add Utils get volume sources
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
	ui.cb_action->clear();
	ui.cb_action->addItems(items);
	
}





















void ConfigWindow::load(){};
void ConfigWindow::addrow(){};
void ConfigWindow::deleterow(){
	
	
	try {
		auto items = ui.tableWidget->selectedItems();
		if (!items.isEmpty()) {
			int rc = items[0]->row();
			ui.tableWidget->removeRow(rc);
		}
		
	} catch (const std::exception &e) {
		//blog(1, e.what().c_str);
		return;
	}
};
void ConfigWindow::updateUi(){};
void ConfigWindow::selectionChanged(){};

