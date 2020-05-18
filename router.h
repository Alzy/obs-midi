#pragma once

#include <QObject>
using namespace std;
class Router : public QObject {
	Q_OBJECT

public:
	Router();
	~Router();
public slots:
	void gotmessage(std::string mtype, int mc);
signals:
	void UnknownMessage(QString mtype, int messageindex);
};
