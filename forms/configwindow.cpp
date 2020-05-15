#include "configwindow.h"
#include "ui_configwindow.h"
#include <QtWidgets>
#include <QAbstractItemView>
ConfigWindow::ConfigWindow(QWidget *parent)
	
{
	//Setup the UI
	ui.setupUi(this);
	//Connect back button functionality
	connect(ui.btnBack, &QPushButton::clicked, this,
		&ConfigWindow::on_btn_back_clicked);
	ui.tableView->setSelectionMode(QAbstractItemView::SingleSelection);
	//QItemSelectionModel *x = ui.tableView->selectionModel();
	//x->connect(SLOT(this, ConfigWindow::TselChanged));
	
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
	bidirectional.append(false);
	actiontype.append("Button");
	action.append("Set Mute");
	option1.append("Mic/ Aux");
	option2.append("");
	option3.append("");

	
	//create model
	//TestModel configTableModel;
	TestModel *configTableModel = new TestModel(this);
	ConfigWindow::SetupModel();

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
	mapper->addMapping(ui.checkBox, 2, "enabled");
	mapper->addMapping(ui.cb_atype, 3, "currentIndex");
	mapper->addMapping(ui.cb_action, 4, "currentIndex");
	mapper->addMapping(ui.cb_param1, 5, "currentIndex");
	mapper->addMapping(ui.cb_param2, 6, "currentIndex");
	mapper->addMapping(ui.cb_param3, 7, "currentIndex");
	configTableModel->populateData(messagetype, messagenumber,
				       bidirectional, actiontype, action,
				       option1, option2, option3);
	//connect(ui.tableView, &QTableView::clicked, this,		&ConfigWindow::TselChanged);
	connect(ui.tableView->selectionModel(),&QItemSelectionModel::currentRowChanged, mapper,&QDataWidgetMapper::setCurrentModelIndex);
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
void ConfigWindow::SetupModel() {
	//Make models for combo box and add to combobox
	QStringList items;
	items << tr("Button") << tr("Fader") ;
	actiontypemodel = new QStringListModel(items, this);
	actiontypemodel->setProperty("role",2);
	ui.cb_atype->setModel(actiontypemodel);

	QStringList items2;
	items2 << tr("SetCurrentScene") << tr("SetPreviewScene")
	       << tr("TransitionToProgram") << tr("SetCurrentTransition")
	       << tr("SetSourceVisibility") << tr("ToggleSourceVisibility")
	       << tr("ToggleMute") << tr("SetMute") << tr("StartStopStreaming")
	       << tr("StartStreaming") << tr("StopStreaming")
	       << tr("StartStopRecording") << tr("StartRecording")
	       << tr("StopRecording") << tr("StartStopReplayBuffer")
	       << tr("StartReplayBuffer") << tr("StopReplayBuffer")
	       << tr("SaveReplayBuffer") << tr("PauseRecording")
	       << tr("ResumeRecording") << tr("SetTransitionDuration")
	       << tr("SetCurrentProfile") << tr("SetCurrentSceneCollection")
	       << tr("ResetSceneItem") << tr("SetTextGDIPlusText")
	       << tr("SetBrowserSourceURL") << tr("ReloadBrowserSource")
	       << tr("TakeSourceScreenshot") << tr("EnableSourceFilter")
	       << tr("DisableSourceFilter") << tr("ToggleSourceFilter");
	buttonactionsmodel = new QStringListModel(items2, this);
	ui.cb_action->setModel(buttonactionsmodel);
	/*
	// This will be dynamically chosen based on cb_atype
	// For use when we have if statement made

	QStringList items3;
	
	items3 << tr("SetVolume") << tr("SetSyncOffset") << tr("SetSourcePosition")
	<< tr("SetSourceRotation") << tr("SetSourceScale")
	<< tr("SetTransitionDuration") << tr("SetGainFilter"); 
	faderactionsmodel = new QStringListModel(items3, this);
	ui.cb_action->setModel(faderactionsmodel);
	*/
	
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

void ConfigWindow::TselChanged(QModelIndex i)
{
	//mapper->toFirst();
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
