/*
obs-midi
Copyright (C) 2016-2017	Stéphane Lepin <stephane.lepin@gmail.com>

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program. If not, see <https://www.gnu.org/licenses/>
*/

#include "config.h"

#define PARAM_DEVICES "MidiDevices"

#define QT_TO_UTF8(str) str.toUtf8().constData()

using namespace std;

Config::Config()
{
}

Config::~Config()
{
	
}

/* Load the configuration from the OBS Config Store
 */
void Config::Load()
{
	OBSData saveddata = GetConfigStore();
	auto deviceManager = GetDeviceManager();
	deviceManager->Load(std::move(obs_data_get_array(saveddata, PARAM_DEVICES)));
	obs_data_release(saveddata);
}

/* Save the configuration to the OBS Config Store
 */
void Config::Save()
{
	OBSData newmidi = obs_data_create();
	auto deviceManager = GetDeviceManager();
	auto data = deviceManager->GetData();
	obs_data_set_array(newmidi, PARAM_DEVICES, data);
	auto path = obs_module_config_path(get_file_name().c_str());
	obs_data_save_json_safe(newmidi, path, ".tmp", ".bkp");
	bfree(path);
	obs_data_array_release(data);
	obs_data_release(newmidi);
}
std::string Config::get_file_name()
{
	std::string file = "obs-midi_";
	file += obs_frontend_get_current_profile();
	file += "_";
	file += obs_frontend_get_current_scene_collection();
	file += ".json";
	return file;
}
OBSData Config::GetConfigStore()
{
	OBSData midiConfig;
	auto path = obs_module_config_path(NULL);
	auto filepath = obs_module_config_path(get_file_name().c_str());
	os_mkdirs(path);
	if (os_file_exists(filepath)) {
		midiConfig = obs_data_create_from_json_file(filepath);
	} else {
		midiConfig = obs_data_create();
		obs_data_save_json_safe(midiConfig, filepath, ".tmp", ".bkp");
	}
	bfree(filepath);
	bfree(path);
	return std::move(midiConfig);
}
