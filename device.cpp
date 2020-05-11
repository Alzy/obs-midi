/*
obs-websocket
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

#include <obs-frontend-api.h>

#include <QtCore/QCryptographicHash>
#include <QtCore/QTime>
#include <QtWidgets/QSystemTrayIcon>

#define PARAM_ENABLED "DeviceEnabled"
#define PARAM_NAME "DeviceName"
std::string section = "MIDI-OBS-Device-";

#include "utils.h"

#include "device.hpp"

#define QT_TO_UTF8(str) str.toUtf8().constData()

Device::Device()
	: DeviceEnabled(false),DeviceName(""), SettingsLoaded(false)
{

	qsrand(QTime::currentTime().msec());

	SetDefaults("");

	obs_frontend_add_event_callback(OnFrontendEvent, this);
}

Device::~Device()
{
	obs_frontend_remove_event_callback(OnFrontendEvent, this);
}

void Device::Load(std::string NAME)
{
	config_t *obsDevice = GetConfigStore();
	const char *SECTION_NAME = section.append(NAME).c_str();
	;
	
	DeviceEnabled = config_get_bool(obsDevice, SECTION_NAME, PARAM_ENABLED);
	DeviceName == config_get_string(obsDevice, SECTION_NAME, PARAM_NAME);
}

void Device::Save(std::string NAME)

{
	const char *SECTION_NAME = section.append(NAME).c_str();
	
	config_t *obsDevice = GetConfigStore();

	config_set_bool(obsDevice, SECTION_NAME, PARAM_ENABLED, DeviceEnabled);
	config_set_string(obsDevice, SECTION_NAME, PARAM_NAME, NAME.c_str());

	config_save(obsDevice);
}

void Device::SetDefaults(std::string NAME)
{
	const char *SECTION_NAME = section.append(NAME).c_str();
	// OBS Device defaults
	config_t *obsDevice = GetConfigStore();
	if (obsDevice) {
		config_set_default_bool(obsDevice, SECTION_NAME, PARAM_ENABLED, DeviceEnabled);
		config_set_default_string(obsDevice, SECTION_NAME, PARAM_NAME, NAME.c_str());
	}
}

config_t *Device::GetConfigStore()
{
	return obs_frontend_get_profile_config();
}



void Device::MigrateFromGlobalSettings(std::string NAME)
{
	const char *SECTION_NAME = section.append(NAME).c_str();
	config_t *source = obs_frontend_get_global_config();
	config_t *destination = obs_frontend_get_profile_config();

	if (config_has_user_value(source, SECTION_NAME, PARAM_ENABLED)) {
		bool value = config_get_bool(source, SECTION_NAME, PARAM_ENABLED);
		config_set_bool(destination, SECTION_NAME, PARAM_ENABLED, value);

		config_remove_value(source, SECTION_NAME, PARAM_ENABLED);
	}

	if (config_has_user_value(source, SECTION_NAME, PARAM_NAME)) {
		std::string value = config_get_string(source, SECTION_NAME, PARAM_NAME);
		config_set_string(destination, SECTION_NAME, PARAM_NAME, value.c_str());

		config_remove_value(source, SECTION_NAME, PARAM_NAME);
	}

	config_save(destination);
}
