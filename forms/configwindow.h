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
    void AddRowFromHooks(int row, std::string mtype, int mindex, bool bid,
			 std::string atype, std::string action,
			 std::string option1, std::string option2,
			 std::string option3);
    bool inrow(int x);
    bool inrow(QString mtype);
    

private slots:

	void load();
	
	void addrow();
	void deleterow();
	void setDirty() { setWindowModified(true); }
	void updateUi();
	void selectionChanged();
	void domessage(QString mtype, int mchan);
	void insertRow(QString mtype, int mindex);
	void select(int row, int col);
	void sendToTable();
	void save();
	void chooseOptions1(QString Action);
	QStringList MakeSceneCombo();
	QStringList MakeVolumeCombo();
	//Combobox Actions
	QStringList chooseAtype(int index);

    private:
	QDataWidgetMapper *mapper;
	QStringListModel *actiontypemodel;
	QStringListModel *buttonactionsmodel;
	QStringListModel *faderactionsmodel;
	QStringListModel *options1model;
	QStringListModel *options2model;
	QStringListModel *options3model;

	


	
	Ui::ConfigWindow ui;
	//std::vector<MidiHook *> hooks;

};

#endif // CONFIGWINDOW_H
