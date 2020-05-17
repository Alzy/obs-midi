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

void Router::gotmessage(std::string mtype, int mc)
{
	emit UnknownMessage(mtype, mc);
}
