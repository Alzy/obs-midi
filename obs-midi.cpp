#include <RtMidi.h>
#include <obs-frontend-api.h>
#include <obs-module.h>

OBS_DECLARE_MODULE()

void rtmidicallback( double deltatime, std::vector< unsigned char > *message, void *userData )
{
	/*unsigned int nBytes = message->size();
	for ( unsigned int i=0; i<nBytes; i++ )
		std::cout << "Byte " << i << " = " << (int)message->at(i) << ", ";
	if ( nBytes > 0 )
		std::cout << "stamp = " << deltatime << std::endl;*/

	unsigned int nBytes = message->size();

	if (nBytes > 0) {
		// Voice messages
		if (message->at(0) >= 0x80 && message->at(0) <= 0xEF) {
			int statusPrefix = (message->at(0) >> 4) & 0x0f;
			int channel = message->at(0) & 0x0f;
			switch (statusPrefix) {
				case 0x8: // Note off
					blog(LOG_INFO, "Channel %d Note %02X off", channel, message->at(1));
					break;
				case 0x9: // Note on
					blog(LOG_INFO, "Channel %d Note %02X on", channel, message->at(1));
					break;
				case 0xA: // Key pressure

					break;
				case 0xB: // Control Change
					blog(LOG_INFO, "Channel %d Control %02X updated %02X", channel, message->at(1), message->at(2));
					break;
				case 0xC: // Program Change

					break;
				case 0xD: // Channel Pressure

					break;
				case 0xE: // Pitch Bend

					break;
			}
		}
	}
	else {
		return;
	}
}

bool obs_module_load(void)
{
	RtMidiIn *midiin = 0;
	// RtMidiIn constructor
	try {
		midiin = new RtMidiIn();
	}
	catch (RtMidiError &error) {
		// Handle the exception here
		error.printMessage();
	}

	blog(LOG_INFO, "Number of MIDI ports found: %d", midiin->getPortCount());

	midiin->openPort(0, "");

	midiin->setCallback(&rtmidicallback, 0);

	return true;
}