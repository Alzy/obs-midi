#include <iostream>
#include <obs-module.h>
#include <util/base.h>
#include "RtMidi.h"
#include <string>
#include <map>
#include <iostream>

#include <utility>
using namespace std;
OBS_DECLARE_MODULE()
map<int, string> Employees;
std::string getMessageType(int in)
{

	std::string a = Employees[in];
	return a;
}
void init() {
	Employees.insert(std::pair<int, string>(176, "control_change"));
	Employees.insert(std::pair<int, string>(128, "note_off"));
	Employees.insert(std::pair<int, string>(144, "note_on"));
	Employees.insert(std::pair<int, string>(192, "program_change"));

	blog(LOG_INFO, "midi init");

	blog(LOG_INFO, "getMessageType==: %s ", getMessageType(128).c_str());
}

void midiin_callback(double deltatime, std::vector<unsigned char> *message, void *userData)
{
	unsigned int nBytes = message->size();
	for (unsigned int i = 0; i < nBytes; i++) {
		blog(LOG_INFO, "Byte %d = %d", i, (int)message->at(i));
	}
	if (nBytes > 0)
		std::cout << "stamp = " << deltatime << std::endl;
}



bool obs_module_load(void)
{
	blog(LOG_INFO, "MIDI LOADED ");
	init();

	RtMidiIn *midiin = 0;
	// RtMidiIn constructor
	try {
		midiin = new RtMidiIn();
	} catch (RtMidiError &error) {
		// Handle the exception here
		error.printMessage();
	}

	unsigned int nPorts = midiin->getPortCount();
	blog(LOG_INFO, "Number of MIDI ports found: %d", nPorts);
	for (unsigned int i = 0; i < nPorts; i++) {
		try {
			std::string portName = midiin->getPortName(i);
			blog(LOG_INFO, "MIDI DEVICE FOUND: %s", portName.c_str());
		} catch (RtMidiError &error) {
			error.printMessage();
		}
	}

	midiin->openPort(0);
	midiin->setCallback(&midiin_callback);

	return true;
}
