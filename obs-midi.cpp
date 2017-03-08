#include <RtMidi.h>
#include <obs-frontend-api.h>
#include <obs-module.h>

OBS_DECLARE_MODULE()

void rtmidicallback( double deltatime, std::vector< unsigned char > *message, void *userData )
{
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
				{
					blog(LOG_INFO, "Channel %d Note %02X on", channel, message->at(1));

					obs_source_t *source = obs_get_source_by_name("Scene 2");

					obs_frontend_set_current_scene(source);

					obs_source_release(source);

					break;
				}
				case 0xA: // Key pressure

					break;
				case 0xB: // Control Change
				{

					blog(LOG_INFO, "Channel %d Control %02X updated %02X", channel, message->at(1), message->at(2));

					float amount = message->at(2);

					amount = amount/127.0f;

					obs_source_t *source = obs_get_source_by_name("Mic/Aux");

					obs_source_set_volume(source, amount);

					obs_source_release(source);

					break;
			}
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

void test_set_colors() {
	std::vector<unsigned char> message;

	RtMidiOut *midiout = 0;
	// RtMidiOut constructor
	try {
		midiout = new RtMidiOut();
	}
	catch (RtMidiError &error) {
		// Handle the exception here
		error.printMessage();
	}

	// Check outputs.
	std::string portName;
	unsigned int nPorts = midiout->getPortCount();
	std::cout << "\nThere are " << nPorts << " MIDI output ports available.\n";
	for ( unsigned int i=0; i<nPorts; i++ ) {
		try {
			portName = midiout->getPortName(i);
		}
		catch (RtMidiError &error) {
			error.printMessage();
		}
		std::cout << "  Output Port #" << i+1 << ": " << portName << '\n';
	}
	std::cout << '\n';

	midiout->openPort( 0 );

	message.push_back( 0x90 );
	message.push_back( 0 );
	message.push_back( 1 );
	midiout->sendMessage( &message );

	message[0] = 0x90;
	message[1] = 1;
	message[2] = 2;
	midiout->sendMessage( &message );
}

static void OBSEvent(enum obs_frontend_event event, void *)
{

}

bool obs_module_load(void)
{
	test_set_colors();

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

	obs_frontend_add_event_callback(OBSEvent, nullptr);

	return true;
}