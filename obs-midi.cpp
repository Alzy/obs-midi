
#include <iostream>
#include <obs-module.h>
#include <obs-frontend-api.h>
#include <obs-data.h>
#include <string>
#include <map>
#include <iostream>
#include <utility>
#include "obs-midi.h"
#include "RtMidi.h"
#include "forms/settings-dialog.h"
#include <QtWidgets/QAction>
#include <QtWidgets/QMainWindow>
#include "config.h"
#include "utils.h"
#include "midi-agent.h"
#include "device.hpp"

using namespace std;

void ___source_dummy_addref(obs_source_t *) {}
void ___sceneitem_dummy_addref(obs_sceneitem_t *) {}
void ___data_dummy_addref(obs_data_t *) {}
void ___data_array_dummy_addref(obs_data_array_t *) {}
void ___output_dummy_addref(obs_output_t *) {}

void ___data_item_dummy_addref(obs_data_item_t *) {}
void ___data_item_release(obs_data_item_t *dataItem)
{
	obs_data_item_release(&dataItem);
}

OBS_DECLARE_MODULE()
OBS_MODULE_USE_DEFAULT_LOCALE("obs-midi", "en-US")

ConfigPtr _config;





void midiin_callback(double deltatime, std::vector<unsigned char> *message, void *userData)
{
	unsigned int nBytes = message->size();
	SettingsDialog *sd = static_cast<SettingsDialog *> (userData);
	std::string byte1 = Utils::getMidiMessageType((int)message->at(0));

	int x = (int)message->at(1);
	int y = (int)message->at(2);

	sd->pushDebugMidiMessage(std::to_string(deltatime), byte1,
				 x, Utils::mapper(y));
}



bool obs_module_load(void)
{
	blog(LOG_INFO, "MIDI LOADED ");

	vector<MidiAgent *> activeMidiAgents;

	MidiAgent *midiA = new MidiAgent();
	midiA->SetMidiDevice(0);

	activeMidiAgents.push_back(midiA);

	// UI SETUP
	QMainWindow *mainWindow = (QMainWindow *)obs_frontend_get_main_window();
	SettingsDialog *settingsDialog = new SettingsDialog(mainWindow, activeMidiAgents);

	const char* menuActionText = obs_module_text("OBSMIDI.Settings.DialogTitle");
	QAction* menuAction = (QAction*)obs_frontend_add_tools_menu_qaction(menuActionText);
	QObject::connect(menuAction, &QAction::triggered, [settingsDialog] {
		// The settings dialog belongs to the main window. Should be ok
		// to pass the pointer to this QAction belonging to the main window
		settingsDialog->ToggleShowHide();
		if (settingsDialog->isVisible()) {
			RtMidi *sdMidi = new RtMidiIn();

			int nDevices = sdMidi->getPortCount();
			if (nDevices == 0 ) { return; }

			vector<string> devNames;
			for (int i = 0; i < nDevices; i++) {
				std::string portn = sdMidi->getPortName(i);
				devNames.push_back(portn);
				Device::Device(portn);

				
			}
			settingsDialog->SetAvailableDevices(devNames);
		}
	});
	
	return true;
}


ConfigPtr GetConfig()
{
	return _config;
}
