#include <QObject>
class Router : public QObject {
	Q_OBJECT

public:
	Router();
	~Router();
public slots:
	void gotmessage(std::string mtype, int mc);
signals:
	void UnknownMessage(std::string mtype, int messageindex);
};
#pragma once
