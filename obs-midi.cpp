
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
	sd->pushDebugMidiMessage(std::to_string(deltatime), byte1,
				 (int)message->at(1), (int)message->at(2));
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
	std::vector<std::string> midiDevices;
	blog(LOG_INFO, "Number of MIDI ports found: %d", nPorts);
	for (unsigned int i = 0; i < nPorts; i++) {
		try {
			std::string portName = midiin->getPortName(i).c_str();
			blog(LOG_INFO, "MIDI DEVICE FOUND: %s", portName.c_str());
			midiDevices.push_back(portName);
		} catch (RtMidiError &error) {
			error.printMessage();
		}
	}

	// UI SETUP
	QMainWindow *mainWindow = (QMainWindow *)obs_frontend_get_main_window();
	SettingsDialog *settingsDialog = new SettingsDialog(mainWindow);

	const char* menuActionText = obs_module_text("OBSMIDI.Settings.DialogTitle");
	QAction* menuAction = (QAction*)obs_frontend_add_tools_menu_qaction(menuActionText);
	QObject::connect(menuAction, &QAction::triggered, [settingsDialog] {
		// The settings dialog belongs to the main window. Should be ok
		// to pass the pointer to this QAction belonging to the main window
		settingsDialog->ToggleShowHide();
	});
	settingsDialog->SetAvailableDevices(midiDevices);
	
	midiin->openPort(0);
	midiin->setCallback(&midiin_callback, settingsDialog);
	return true;
}


ConfigPtr GetConfig()
{
	return _config;
}
