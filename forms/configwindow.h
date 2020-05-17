#pragma once
#ifndef CONFIGWINDOW_H
#define CONFIGWINDOW_H


//include obs stuff
#include <obs.hpp>
#include <obs-module.h>

//qt stuff
#include <QDialog>
#include <QTableView>
//#include <QMainWindow>
#include <QAbstractTableModel>
#include <QWidget>
#include "ui_configwindow.h"
//#include "midi-agent.h"
class QComboBox;
class QDataWidgetMapper;
class QLabel;
class QLineEdit;
class QPushButton;
class QStandardItemModel;
class QStringListModel;
class QTextEdit;
class QAbstractItemView;




class TestModel : public QAbstractTableModel {
	Q_OBJECT

public:
	TestModel(QObject *parent = 0);
	void MakeMapper();
	void populateData(const QList<QString> &messagetype,
			  const QList<int> &messagenumber,
			  const QList<bool> &tm_bidirectional,
			  const QList<QString> &tm_actiontype,
			  const QList<QString> &tm_action, const QList<QString> &tm_option1,
			  const QList<QString> &tm_option2, const QList<QString> &tm_option3);

	void PopulateOptions(const QList<QString> &option1,
					const QList<QString> &option2,
					const QList<QString> &option3);
	//Insert Default Row
	bool insertRow(int row, QString mtype,
			int mindex,
			const QModelIndex &parent = QModelIndex());
	//Insert MH Row
	bool insertRow(int row, std::string mtype, int mindex,
		       std::string actiontype, std::string action,
		       std::string option1, std::string option2,
		       std::string option3,
		       const QModelIndex &parent = QModelIndex());
	void MakeVolumeCombo();
	void MakeSceneCombo();
	void chooseOptions1(QString Action);
	//bool insertRows(int row, int rows,const QModelIndex &parent = QModelIndex());
	//bool removeRows(int row, int mindex,const QModelIndex &parent = QModelIndex());)
	int rowCount(const QModelIndex &parent = QModelIndex()) const
		Q_DECL_OVERRIDE;
	int columnCount(const QModelIndex &parent = QModelIndex()) const
		Q_DECL_OVERRIDE;

	QVariant data(const QModelIndex &index,
		      int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
	QVariant headerData(int section, Qt::Orientation orientation,
			    int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
	void save(QString devname);


private:
	QList<QString> tm_messagetype;
	QList<int> tm_messagenumber;
	QList<bool> tm_bidirectional;
	QList<QString> tm_actiontype;
	QList<QString> tm_action;
	QList<QString> tm_option1;
	QList<QString> tm_option2;
	QList<QString> tm_option3;

	QList<QString> tm_scenes;
	QList<QString> tm_sources;
	QList<QString> tm_items;
	QList<QString> tm_volume;

	QStringList *scenes;
	QStringList *sources;
	QStringList *items;
	QStringList *volume;

	QStringListModel *actiontypemodel;
	QStringListModel *buttonactionsmodel;
	QStringListModel *faderactionsmodel;
	QStringListModel *options1model;
	QStringListModel *options2model;
	QStringListModel *options3model;
	QStringListModel *scenesModel;
	QStringListModel *sourcesModel;
	QStringListModel *itemsModel;
	QStringListModel *volumeModel;
	TestModel *configTableModel;


};
class ConfigWindow : public QDialog{
		Q_OBJECT
	 public:
    explicit ConfigWindow(std::string devicename);
    ~ConfigWindow();
    void SetupModel();
    void on_btn_back_clicked();
    void rebuildModel();
    void ToggleShowHide();
    void TselChanged(QModelIndex i);
    
    //variables
    std::string devicename;
    QTableView tableEntity;
    void loadFromHooks();
	//MakeCombos



private slots:

	void load();
	void save();
	void addrow();
	void deleterow();
	void setDirty() { setWindowModified(true); }
	void updateUi();
	void selectionChanged();
	void domessage(QString mtype, int mchan);

	//Combobox Actions
	void chooseAtype(int index);

    private:
	QDataWidgetMapper *mapper;
	QStringListModel *actiontypemodel;
	QStringListModel *buttonactionsmodel;
	QStringListModel *faderactionsmodel;
	QStringListModel *options1model;
	QStringListModel *options2model;
	QStringListModel *options3model;

	


	TestModel *configTableModel;
	Ui::ConfigWindow ui;
	//std::vector<MidiHook *> hooks;

};

#endif // CONFIGWINDOW_H
