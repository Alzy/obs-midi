#include "router.h"
#include <QObject>




Router::Router()
{

	// connect(this, SIGNAL(UnknownMessage),  SLOT(gotmessage));
}

Router::~Router()
{
	//this is the router
}

void Router::gotmessage(std::string name, std::string mtype, int mc)
{
	QString mtype2 = QString::fromStdString(mtype);
	QString name2 = QString::fromStdString(name);
	
	emit UnknownMessage(name2, mtype2, mc);
}
