#include <iostream>
#include <obs-module.h>
#include <util/base.h>
#include "RtMidi.h"

OBS_DECLARE_MODULE()


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
			blog(LOG_INFO, "MIDI DEVICE FOUND: %s", &portName);
		} catch (RtMidiError &error) {
			error.printMessage();
		}
	}

	midiin->openPort(0);
	midiin->setCallback(&midiin_callback);

	return true;
}
