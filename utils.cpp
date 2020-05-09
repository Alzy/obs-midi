#include <iostream>
#include <obs-module.h>
#include <util/base.h>
#include "RtMidi.h"
#include <string>
#include <map>
#include <iostream>

#include <utility>
using namespace std;
map<int, string> MsgType;
MsgType.insert(std::pair<int, string>(176, "control_change"));
MsgType.insert(std::pair<int, string>(128, "note_off"));
MsgType.insert(std::pair<int, string>(144, "note_on"));
MsgType.insert(std::pair<int, string>(192, "program_change"));

std::string getMessageType(int in)
{
	std::string a = MsgType[in];
	return a;
}
void test()
{

	blog(LOG_INFO, "midi init");

	blog(LOG_INFO, "getMessageType==: %s ", getMessageType(128).c_str());
}
