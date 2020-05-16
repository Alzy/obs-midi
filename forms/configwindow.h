#ifndef CONFIGWINDOW_H
#define CONFIGWINDOW_H

#include <QDialog>
#include <QTableView>
//#include <QMainWindow>
#include <QAbstractTableModel>
#include <QWidget>
#include "ui_configwindow.h"

class QComboBox;
class QDataWidgetMapper;
class QLabel;
class QLineEdit;
class QPushButton;
class QStandardItemModel;
class QStringListModel;
class QTextEdit;
class QAbstractItemView;
namespace Ui {
class ConfigWindow;
}
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

	int rowCount(const QModelIndex &parent = QModelIndex()) const
		Q_DECL_OVERRIDE;
	int columnCount(const QModelIndex &parent = QModelIndex()) const
		Q_DECL_OVERRIDE;

	QVariant data(const QModelIndex &index,
		      int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
	QVariant headerData(int section, Qt::Orientation orientation,
			    int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;

private:
	QList<QString> tm_messagetype;
	QList<int> tm_messagenumber;
	QList<bool> tm_bidirectional;
	QList<QString> tm_actiontype;
	QList<QString> tm_action;
	QList<QString> tm_option1;
	QList<QString> tm_option2;
	QList<QString> tm_option3;
	QStringListModel *actiontypemodel;
	QStringListModel *buttonactionsmodel;
	QStringListModel *faderactionsmodel;
	QStringListModel *options1model;
	QStringListModel *options2model;
	QStringListModel *options3model;
	TestModel *configTableModel;


};
class ConfigWindow : public QDialog{
		Q_OBJECT
	 public:
    explicit ConfigWindow(QWidget *parent = 0);
    ~ConfigWindow();
    void SetupModel();
    void on_btn_back_clicked();
    void rebuildModel();
    void ToggleShowHide();
    void TselChanged(QModelIndex i);
    //variables
    std::string devicename;
    QTableView tableEntity;

private slots:
    void chooseAtype(int index);

    private:
	QDataWidgetMapper *mapper;
	QStringListModel *actiontypemodel;
	QStringListModel *buttonactionsmodel;
	QStringListModel *faderactionsmodel;
	QStringListModel *options1model;
	QStringListModel *options2model;
	QStringListModel *options3model;
	Ui::ConfigWindow ui;

};

#endif // CONFIGWINDOW_H
