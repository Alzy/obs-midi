#include "configwindow.h"
#include "ui_configwindow.h"
#include <QtWidgets>
#include <QAbstractItemView>
#include <obs-frontend-api/obs-frontend-api.h>
#include "obs-midi.h"
#include "config.h"
#include "router.h"
#include "device-manager.h"
#include "midi-agent.h"

ConfigWindow::ConfigWindow( std::string devicename)
	
{
	
	//auto rob = static_cast<RouterPtr>(GetRouter());
	auto devicemanager = GetDeviceManager();
	auto config = GetConfig();
	auto rt = GetRouter();	
	//connect(this, SIGNAL(&SendNewUnknownMessage), this	SIGNAL(rt.UnknownMessage));
	auto device = devicemanager->GetMidiDeviceByName(devicename.c_str());
	connect(rt, SIGNAL(&Router::UnknownMessage), this,SLOT(domessage));
	//Setup the UI
	ui.setupUi(this);


//connect
	//Connect back button functionality
	connect(ui.btnBack, &QPushButton::clicked, this,&ConfigWindow::on_btn_back_clicked);
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
	messagetype.append("control_change");
	messagenumber.append(101);
	bidirectional.append(false);
	actiontype.append("Fader");
	action.append("Set Volume");
	option1.append("Mic/ Aux");
	option2.append("");
	option3.append("");
	messagetype.append("ass");
	messagenumber.append(1);
	bidirectional.append(true);
	actiontype.append("Button");
	action.append("Set Mute");
	option1.append("Mic/ Aux");
	option2.append("");
	option3.append("");


	//setup model
	ConfigWindow::SetupModel();
	//create default actions
	ConfigWindow::chooseAtype(1);
	//create model
	//TestModel configTableModel;
	TestModel *configTableModel = new TestModel(this);
	

	//Wrap Data into Model

	//set model to TableView
	ui.tableView->setModel(configTableModel);
	ui.tableView->horizontalHeader()->setVisible(true);
	ui.tableView->show();
	//create Data Mapper
	//QDataWidgetMapper mapper =  QDataWidgetMapper(this);
	QDataWidgetMapper *mapper = new QDataWidgetMapper(this);

	mapper->setModel(configTableModel);
	mapper->setSubmitPolicy(QDataWidgetMapper::AutoSubmit);
	mapper->addMapping(ui.lin_mtype, 0);
	mapper->addMapping(ui.num_mchan, 1, "value");
	mapper->addMapping(ui.checkBox, 2, "checked");
	mapper->addMapping(ui.cb_atype, 3, "currentText");
	mapper->addMapping(ui.cb_action, 4, "currentText");
	mapper->addMapping(ui.cb_param1, 5, "currentText");
	mapper->addMapping(ui.cb_param2, 6, "currentText");
	mapper->addMapping(ui.cb_param3, 7, "currentText");
	configTableModel->populateData(messagetype, messagenumber,
				       bidirectional, actiontype, action,
				       option1, option2, option3);
	//connect(ui.tableView, &QTableView::clicked, this,		&ConfigWindow::TselChanged);
	connect(ui.tableView,
		SIGNAL(activated), ui.tableView,
		SLOT(repaint));
	
	connect(ui.tableView->selectionModel(),&QItemSelectionModel::currentRowChanged, mapper,&QDataWidgetMapper::setCurrentModelIndex);
	connect(ui.cb_atype, SIGNAL(currentIndexChanged(int)), this, SLOT(chooseAtype(int)));
	//mapper->AutoSubmit = true;
	connect(ui.cb_action, SIGNAL(currentIndexChanged(int)), this, SLOT(mapper->submit()));
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
void ConfigWindow::domessage(std::string mtype, int mchan)
{
	blog(1, "domessage");
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


	
	
	//We need to Dynamically Generate These ones based on value of cb_action and data pulled from OBS
	QStringList items4;
	items4 << tr("Button") << tr("Fader");
	options1model = new QStringListModel(items4, this);
	ui.cb_param1->setModel(options1model);

	QStringList items5;
	items5 << tr("Button") << tr("Fader");
	options2model = new QStringListModel(items5, this);
	ui.cb_param2->setModel(options2model);

	QStringList items6;
	items6 << tr("Button") << tr("Fader");
	options3model = new QStringListModel(items6, this);
	ui.cb_param3->setModel(options3model);
	//link combobox info
	
	

	
}


TestModel::TestModel(QObject *parent) : QAbstractTableModel(parent) {}

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
	tm_option1 = option1;
	tm_option2.clear();
	tm_option2 = option2;
	tm_option3.clear();
	tm_option3 = option3;

	return;
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
QVariant TestModel::headerData(int section, Qt::Orientation orientation,
			       int role) const
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
