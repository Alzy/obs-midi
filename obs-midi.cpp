
#include <iostream>
#include <obs-module.h>
#include <obs-frontend-api/obs-frontend-api.h>
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
#include "device-manager.h"
#include "utils.h"
#include "midi-agent.h"
#include "router.h"
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
DeviceManagerPtr _deviceManager;






bool obs_module_load(void)
{
	blog(LOG_INFO, "MIDI LOADED ");

	// Device Manager Setup
	_deviceManager = DeviceManagerPtr(new DeviceManager());


	// Config Setup
	_config = ConfigPtr(new Config());
	_config->Load();

	// Signal Router Setup
	

	// UI SETUP
	QMainWindow *mainWindow = (QMainWindow *)obs_frontend_get_main_window();
	SettingsDialog *settingsDialog = new SettingsDialog(mainWindow);

	const char* menuActionText = obs_module_text("OBS MIDI Settings");

	QAction* menuAction = (QAction*)obs_frontend_add_tools_menu_qaction(menuActionText);

			 
	QObject::connect(menuAction, &QAction::triggered, [settingsDialog] {
		// The settings dialog belongs to the main window. Should be ok
		// to pass the pointer to this QAction belonging to the main window
		settingsDialog->ToggleShowHide();
		if (settingsDialog->isVisible()) {
			auto devNames = _deviceManager->GetPortsList();
			settingsDialog->SetAvailableDevices(devNames);
			

		}
	});
	
	return true;
}


void obs_module_unload()
{
	_config.reset();

	blog(LOG_INFO, "goodbye!");
}



ConfigPtr GetConfig()
{
	return _config;
}


DeviceManagerPtr GetDeviceManager()
{
	return _deviceManager;
}


