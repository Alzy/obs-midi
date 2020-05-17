#include <inttypes.h>


#include <obs.hpp>
#include <util/platform.h>

#include "configwindow.h"
#include "ui_configwindow.h"
#include <QtWidgets>
#include <QAbstractItemView>
#include <obs-frontend-api/obs-frontend-api.h>
#include "obs-midi.h"
#include "config.h"
#include "device-manager.h"
//#include "midi-agent.h"

ConfigWindow::ConfigWindow( std::string devicename)
	
{
	//MakeSceneCombo();
	
	//auto rob = static_cast<RouterPtr>(GetRouter());
	auto devicemanager = GetDeviceManager();
	auto config = GetConfig();
	//config->Load();
	Router *rt = midiobsrouter;	
	//connect(this, SIGNAL(&SendNewUnknownMessage), this	SIGNAL(rt.UnknownMessage));
	auto device = devicemanager->GetMidiDeviceByName(devicename.c_str());

	std::vector<MidiHook *> hooks =	devicemanager->GetMidiHooksByDeviceName(devicename.c_str());
		
	connect( rt, SIGNAL(UnknownMessage(QString, int)), this,SLOT(domessage(QString, int)));
	//Setup the UI
	ui.setupUi(this);


//connect
	//Connect back button functionality
	connect(ui.btnBack, &QPushButton::clicked, this,&ConfigWindow::on_btn_back_clicked);
	connect(ui.btnSave, &QPushButton::clicked, this,
		&ConfigWindow::save);

	ui.tableView->setSelectionMode(QAbstractItemView::SingleSelection);
	
	//hide override and bidirectional elements
	//ui.checkBox->setVisible(false);
	//ui.label_8->setVisible(false);
	//ui.slider_override->setVisible(false);
	//create lists
	
	QList<QString> messagetype;
	QList<int> messagenumber;
	QList<bool> bidirectional;
	QList<QString> actiontype;
	QList<QString> action;
	QList<QString> option1;
	QList<QString> option2;
	QList<QString> option3;
	
	for (int i = 0; i < hooks.size(); i++) {

		messagetype.append(QString::fromStdString(hooks.at(i)->type));
		messagenumber.append(hooks.at(i)->index);
		bidirectional.append(false);
		actiontype.append(QString::fromStdString(hooks.at(i)->action));
		action.append(QString::fromStdString(hooks.at(i)->command));
		option1.append(QString::fromStdString(hooks.at(i)->param1));
		option2.append(QString::fromStdString(hooks.at(i)->param2));
		option3.append(QString::fromStdString(hooks.at(i)->param3));
	}
	//setup model
	ConfigWindow::SetupModel();
	//create default actions
	ConfigWindow::chooseAtype(1);
	//create model
	//TestModel configTableModel;
	configTableModel = new TestModel(this);

	//Wrap Data into Model
		configTableModel->populateData(messagetype, messagenumber, bidirectional, actiontype, action,option1, option2, option3);
	
	
	//set model to TableView
	ui.tableView->setModel(configTableModel);
	ui.tableView->horizontalHeader()->setVisible(true);
	ui.tableView->show();
	//create Data Mapper
	//QDataWidgetMapper mapper =  QDataWidgetMapper(this);
	QDataWidgetMapper *mapper = new QDataWidgetMapper(this);
	mapper->setSubmitPolicy(QDataWidgetMapper::AutoSubmit);

	mapper->setModel(configTableModel);
	mapper->addMapping(ui.lin_mtype, 0);
	mapper->addMapping(ui.num_mchan, 1, "value");
	mapper->addMapping(ui.checkBox, 2, "checked");
	mapper->addMapping(ui.cb_atype, 3, "currentText");
	mapper->addMapping(ui.cb_action, 4, "currentText");
	mapper->addMapping(ui.cb_param1, 5, "currentText");
	mapper->addMapping(ui.cb_param2, 6, "currentText");
	mapper->addMapping(ui.cb_param3, 7, "currentText");
	
	//connect(ui.tableView, &QTableView::clicked, this,		&ConfigWindow::TselChanged);
	
	//connect UI elements
	connect(ui.tableView->selectionModel(),&QItemSelectionModel::currentRowChanged, mapper,&QDataWidgetMapper::setCurrentModelIndex);
	connect(ui.cb_atype, SIGNAL(currentIndexChanged(int)), this, SLOT(chooseAtype(int)));
	connect(ui.cb_action, SIGNAL(currentTextChanged(QString)), configTableModel, SLOT(chooseOptions1(QString)));
	//connect(ui.cb_param1, SIGNAL(currentTextChanged(QString)), this, SLOT(chooseOptions2(QString)));
	//connect(ui.cb_param2, SIGNAL(currentTextChanged(QString)), this, SLOT(chooseOptions3(QString)));
	connect(ui.cb_param1, SIGNAL(currentIndexChanged(int)), ui.tableView,
		SLOT(update()));
	
	
	//mapper->AutoSubmit = true;
	//connect(ui.tableView, &QTableView::clicked, mapper, &QDataWidgetMapper::setCurrentModelIndex);

	ui.tableView->setCurrentIndex(configTableModel->index(0, 0));
	mapper->toFirst();
	

	/*
    QStringListModel *actiontypemodel;
    QStringListModel *buttonactionsmodel;
    QStringListModel *faderactionsmodel;
    QStringListModel *options1model;
    QStringListModel *options2model;
    QStringListModel *options3model;
    */
	// TODO:: Add Data to Table here
	
}
bool TestModel::insertRow(int row, std::string  mtype, int mindex, std::string actiontype,
		std::string action, std::string option1, std::string option2,
		std::string option3, const QModelIndex &parent)
{

	
		beginInsertRows(parent, row, row);
		tm_messagetype.append(QString::fromStdString(mtype));
		tm_messagenumber.append(mindex);
		tm_bidirectional.append(true);
		tm_actiontype.append(QString::fromStdString(actiontype));
		tm_action.append(QString::fromStdString(action));
		tm_option1.append(QString::fromStdString(option1));
		tm_option2.append(QString::fromStdString(option2));
		tm_option3.append(QString::fromStdString(option3));
		endInsertRows();
		return true;
}

bool TestModel::insertRow(int row, QString mtype,
			   int mindex, const QModelIndex &parent)
{
		if (!tm_messagenumber.contains(mindex)) {
		beginInsertRows(parent, row, row);
		tm_messagetype.append(mtype);
		tm_messagenumber.append(mindex);
		tm_bidirectional.append(true);
		if (mtype == "control_change") {
			tm_actiontype.append("Fader");
			tm_action.append("Set Volume");
		} else {
			tm_actiontype.append("Button");
			tm_action.append("Set Mute");
		}

		tm_option1.append("Mic/Aux");
		tm_option2.append("");
		tm_option3.append("");
		endInsertRows();
		return true;
		} else {
			return false;
		}
	
	//configTableModel->populateData(messagetype, messagenumber, bidirectional, actiontype, action, option1, option2, option3);
}
void TestModel::save(QString devicename) {
	//Get Device Manager
	auto dm = GetDeviceManager();
	auto save = GetConfig();
	//to get device
	auto dev = dm->GetMidiDeviceByName(devicename.toStdString().c_str());
	dev->ClearMidiHooks();
	//get row count
	int rc = tm_messagenumber.length();
	//loop through rows
	for (int i=0; i < rc; i++)
	{
	//make default midihook
		
		MidiHook *mh = new MidiHook;
		//map values
		mh->type = tm_messagetype.at(i).toStdString();
		mh->index = tm_messagenumber.at(i);
		//mh->bidirectional = tm_bidirectional.at(i);
		mh->action = tm_actiontype.at(i).toStdString();
		mh->command = tm_action.at(i).toStdString();
		mh->param1 = tm_option1.at(i).toStdString();
		mh->param2 = tm_option2.at(i).toStdString();
		mh->param3 = tm_option3.at(i).toStdString();
		
		//uglyway//
	//MidiHook(string midiMessageType, int midiChannelIndex,string OBSCommand, string p1 = "", string p2 = "",string p3 = "", string actionType = "")
		/*MidiHook *mh = new MidiHook(
		tm_messagetype.at(i).toStdString(), tm_messagenumber.at(i),
		tm_action.at(i).toStdString(), tm_option1.at(i).toStdString(),
		tm_option2.at(i).toStdString(), tm_option3.at(i).toStdString(),
		tm_actiontype.at(i).toStdString());
		*/
		//add midihook
		dev->AddMidiHook(mh);
		
	};
	save->Save();
}
	void ConfigWindow::domessage(QString mtype, int mchan)
{
	
	blog(1, "domessage");
	int x = configTableModel->rowCount();
	switch (x) {
	case 0:
		x = 0;
		break;
	default:
		x = x - 1;
		break;
	}


	if (configTableModel->insertRow(x, mtype, mchan)) {
		ui.tableView->update();	
	}
	
//
	//make default row
	
		
	//insert default row//

	//updateview
}
void ConfigWindow::rebuildModel() {}
// Choose Action Type Handler
void ConfigWindow::chooseAtype(int index) {
	//actiontypemodel = new QStringListModel(items, this);
	QStringList items;
	switch (index) {
	case 0:
		//Button
		
		items << tr("SetCurrentScene") << tr("SetPreviewScene")
		       << tr("TransitionToProgram")
		       << tr("SetCurrentTransition")
		       << tr("SetSourceVisibility")
		       << tr("ToggleSourceVisibility") << tr("ToggleMute")
		       << tr("SetMute") << tr("StartStopStreaming")
		       << tr("StartStreaming") << tr("StopStreaming")
		       << tr("StartStopRecording") << tr("StartRecording")
		       << tr("StopRecording") << tr("StartStopReplayBuffer")
		       << tr("StartReplayBuffer") << tr("StopReplayBuffer")
		       << tr("SaveReplayBuffer") << tr("PauseRecording")
		       << tr("ResumeRecording") << tr("SetTransitionDuration")
		       << tr("SetCurrentProfile")
		       << tr("SetCurrentSceneCollection")
		       << tr("ResetSceneItem") << tr("SetTextGDIPlusText")
		       << tr("SetBrowserSourceURL") << tr("ReloadBrowserSource")
		       << tr("TakeSourceScreenshot") << tr("EnableSourceFilter")
		       << tr("DisableSourceFilter") << tr("ToggleSourceFilter");
		break;
	case 1:
		//Fader
		

		items << tr("SetVolume") << tr("SetSyncOffset")
		       << tr("SetSourcePosition") << tr("SetSourceRotation")
		       << tr("SetSourceScale") << tr("SetTransitionDuration")
		       << tr("SetGainFilter");
		break;
	case 2:
		//OSC
		break;
	case 4:
		//Soundboard
		break;
	}
	buttonactionsmodel = new QStringListModel(items, this);
	ui.cb_action->setModel(buttonactionsmodel);
	//QDataWidgetMapper *mapper = new QDataWidgetMapper(this);
	//mapper->addMapping(ui.cb_action, 4, "currentText");
}




	void ConfigWindow::SetupModel()
{
	//Make models for combo box and add to combobox
	QStringList items;
	items << tr("Button") << tr("Fader") ;
	actiontypemodel = new QStringListModel(items, this);
	actiontypemodel->setProperty("role",2);
	ui.cb_atype->setModel(actiontypemodel);
}


TestModel::TestModel(QObject *parent) : QAbstractTableModel(parent) {
	MakeSceneCombo();
	MakeVolumeCombo();
}

	// Create a method to populate the model with data:
void TestModel::populateData(
	const QList<QString> &messagetype, const QList<int> &messagenumber,
	const QList<bool> &bidirectional, const QList<QString> &actiontype,
	const QList<QString> &action, const QList<QString> &option1,
	const QList<QString> &option2, const QList<QString> &option3)
{
	tm_messagetype.clear();
	tm_messagetype = messagetype;
	tm_messagenumber.clear();
	tm_messagenumber = messagenumber;
	tm_bidirectional.clear();
	tm_bidirectional = bidirectional;
	tm_actiontype.clear();
	tm_actiontype = actiontype;
	tm_action.clear();
	tm_action = action;
	tm_option1.clear();
	tm_option1 = option2;
	tm_option2.clear();
	tm_option2 = option2;
	tm_option3.clear();
	tm_option3 = option3;

	return;
}
void TestModel::PopulateOptions(const QList<QString> &option1,
				const QList<QString> &option2,
				const QList<QString> &option3)
{
	tm_option1.clear();
	tm_option1 = option2;
	tm_option2.clear();
	tm_option2 = option2;
	tm_option3.clear();
	tm_option3 = option3;
}

	void ConfigWindow::save()
{
	//do TestModel->save();

	configTableModel->save(QString::fromStdString(this->devicename));
	
}

int TestModel::rowCount(const QModelIndex &parent) const
{
	Q_UNUSED(parent);
	return tm_messagetype.length();
}

int TestModel::columnCount(const QModelIndex & /*parent*/) const
{
	return 8;
}
QVariant TestModel::data(const QModelIndex &index, int role) const
{
	if (!index.isValid()){
		return QVariant();
	    }
	if (role == Qt::EditRole || role == Qt::DisplayRole) {
		switch (index.column()) {
		case 0:
			return tm_messagetype[index.row()];
		case 1:
			return tm_messagenumber[index.row()];
		case 2:
			return tm_bidirectional[index.row()];
		case 3:
			return tm_actiontype[index.row()];
		case 4:
			return tm_action[index.row()];
		case 5:
			return tm_option1[index.row()];
		case 6:
			return tm_option2[index.row()];
		case 7:
			return tm_option3[index.row()];
		}
	}
		return QVariant();
	
	}
QVariant TestModel::headerData(int section, Qt::Orientation orientation,  int role) const
{
	if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
		switch (section) {
		case 0:
			return QString("Message Type");
		case 1:
			return QString("Message Number");
		case 2:
			return QString("Bidirectional");
		case 3:
			return QString("ActionType");
		case 4:
			return QString("Action");
		case 5:
			return QString("Option 1");
		case 6:
			return QString("Option 2");
		case 7:
			return QString("Option 3");
		}
	}
	return QVariant();
}

void TestModel::MakeMapper() {}

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
void ConfigWindow::load(){};
void ConfigWindow::addrow(){};
void ConfigWindow::deleterow(){};
void ConfigWindow::updateUi(){};
void ConfigWindow::selectionChanged(){};
void ConfigWindow::loadFromHooks()
{
	


}
/*bool TestModel::insertRow(int row, std::string  mtype, int mindex, std::string actiontype,
		std::string action, std::string option1, std::string option2,
		std::string option3, const QModelIndex &parent)*/



/*                Make Combo list models
*/
void TestModel::chooseOptions1(QString Action) {
	QList<QString> option1;
	QList<QString> option2;
	QList<QString> volume;
	QStringList nada;
	if (Action == "SetVolume") {
		
		//configTableModel->PopulateOptions(volumeModel, nada, nada);
		//configTableModel->options1model->
		
	} else if (Action == "SetCurrentScene") {
		
	}
}
/*
tm_option2.clear();
tm_option2 = option2;
tm_option3.clear();
tm_option3 = option3;
*/
void TestModel::MakeSceneCombo()
{
	QStringList opitems;
	auto scenes = Utils::GetScenes();
	auto length = obs_data_array_count(scenes);
	for (size_t i = 0; i < length; i++) {
		auto d = obs_data_array_item(scenes, i);
		auto name = obs_data_get_string(d, "name");
		opitems << tr(name);
	}
	scenesModel= new QStringListModel(opitems, this);
	scenesModel->setProperty("role", 2);
	//ui.cb_param1->setModel(options1model);
}


void TestModel::MakeVolumeCombo() {
	QStringList opitems;
	QMap<const char *, int> sources;
	sources["desktop-1"] = 1;
	sources["desktop-2"] = 2;
	sources["mic-1"] = 3;
	sources["mic-2"] = 4;
	sources["mic-3"] = 5;

	QMapIterator<const char *, int> i(sources);
	while (i.hasNext()) {
		i.next();

		const char *id = i.key();
		OBSSourceAutoRelease source = obs_get_output_source(i.value());
		if (source) {
			opitems << obs_source_get_name(source);
		}
	}
	volumeModel = new QStringListModel(opitems, this);
	volumeModel->setProperty("role", 2);
	//ui.cb_param1->setModel(volumeModel);
}
