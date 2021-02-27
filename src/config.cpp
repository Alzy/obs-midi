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
	connect(GetDeviceManager().get(), SIGNAL(reload_config()), this, SLOT(Load()));
}

Config::~Config()
{
	this->disconnect();
}

/* Load the configuration from the OBS Config Store
 */
void Config::Load()
{
	auto deviceManager = GetDeviceManager();
	deviceManager->Load(GetConfigStore());
}

/* Save the configuration to the OBS Config Store
 */
void Config::Save()
{
	obs_data_t * newmidi = obs_data_create();
	auto deviceManager = GetDeviceManager();
	auto data = deviceManager->GetData();
	obs_data_set_array(newmidi, PARAM_DEVICES, data);
	auto path = obs_module_config_path(get_file_name().toStdString().c_str());
	obs_data_save_json_safe(newmidi, path, ".tmp", ".bkp");
	bfree(path);
	obs_data_array_release(data);
	obs_data_release(newmidi);
}
QString Config::get_file_name()
{
	QString file("obs-midi_");
	file += obs_frontend_get_current_profile();
	file += "_";
	file += obs_frontend_get_current_scene_collection();
	file += ".json";
	return file;
}
QString Config::GetConfigStore()
{
	auto path = obs_module_config_path(NULL);
	auto filepath = obs_module_config_path(get_file_name().toStdString().c_str());
	os_mkdirs(path);
	obs_data_t *midiConfig = os_file_exists(filepath) ? obs_data_create_from_json_file(filepath) : obs_data_create();
	if (!os_file_exists(filepath)) {
		obs_data_save_json_safe(midiConfig, filepath, ".tmp", ".bkp");
	}
	bfree(filepath);
	bfree(path);
	QString conf = QString(obs_data_get_json(midiConfig));
	obs_data_release(midiConfig);

	return std::move(conf);
}
