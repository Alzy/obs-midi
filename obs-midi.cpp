#include <iostream>
#include <obs-module.h>
#include <util/base.h>
#include "RtMidi.h"

OBS_DECLARE_MODULE()

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
			blog(LOG_INFO, "MIDI DEVICE FOUND: %s", portName);
		} catch (RtMidiError &error) {
			error.printMessage();
		}
	}
	return true;
}
