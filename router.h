#pragma once
#include <stdio.h>

#include <QObject>




using namespace std;
class Router : public QObject {
	Q_OBJECT

public:
	Router();
	~Router();
public slots:
	void gotmessage(std::string name, std::string mtype, int mc);
signals:
	void UnknownMessage(QString name, QString mtype, int messageindex);
};
