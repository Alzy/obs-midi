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

#define PARAM_ENABLED "device.Enabled"
#define PARAM_NAME "DeviceName"
std::string section = "MIDI-OBS-Device-";

#include "utils.h"

#include "device.hpp"

#define QT_TO_UTF8(str) str.toUtf8().constData()

Device::Device() 
{
	qsrand(QTime::currentTime().msec());
	//obs_frontend_add_event_callback(OnFrontendEvent, this);
}
typedef struct t_row {
	int id;
	std::string MessageType;
	int channel;
	int InputType;
	bool bidirectional;
	int Action;
	int option1;
	int option2;
	int option3;
	const char *ROW_NAME(std::string NAME)
	{
		std::string x = section.append(NAME);
		std::string y = x+"-row-" + std::to_string(channel)+"-MessageType-"+MessageType;
		return y.c_str();
	}
}t_row;
typedef struct Device::t_device {
	std::string NAME;
	bool Enabled;
	int total_rows;
	const char *SECTION_NAME = section.append(NAME).c_str();
	

	t_row * rows;
};
Device::~Device()
{
	//obs_frontend_remove_event_callback(OnFrontendEvent, this);
}

void Device::Load(t_device device)
{
	config_t *obsDevice = GetConfigStore();
	device.Enabled = config_get_bool(obsDevice, device.SECTION_NAME, PARAM_ENABLED);
	device.NAME = config_get_string(obsDevice, device.SECTION_NAME, PARAM_NAME);
	for (int i = 0; i < device.total_rows; i++) {
		//load Each Row
		device.rows[i].MessageType   = config_get_string(obsDevice, device.rows[i].ROW_NAME(device.SECTION_NAME), "MessageType");
		device.rows[i].channel       = config_get_int   (obsDevice, device.rows[i].ROW_NAME(device.SECTION_NAME), "Channel");
		device.rows[i].InputType     = config_get_int   (obsDevice, device.rows[i].ROW_NAME(device.SECTION_NAME), "InputType");
		device.rows[i].bidirectional = config_get_bool  (obsDevice, device.rows[i].ROW_NAME(device.SECTION_NAME), "Bidirectional");
		device.rows[i].Action        = config_get_int   (obsDevice, device.rows[i].ROW_NAME(device.SECTION_NAME), "Action");
		device.rows[i].option1       = config_get_int   (obsDevice, device.rows[i].ROW_NAME(device.SECTION_NAME), "Option1");
		device.rows[i].option2       = config_get_int   (obsDevice, device.rows[i].ROW_NAME(device.SECTION_NAME), "Option2");
		device.rows[i].option3       = config_get_int   (obsDevice, device.rows[i].ROW_NAME(device.SECTION_NAME), "Option3");
	}
}

void Device::Save(t_device device)

{

	
	config_t *obsDevice = GetConfigStore();

	config_set_bool(obsDevice, device.SECTION_NAME, PARAM_ENABLED,
			device.Enabled);
	config_set_string(obsDevice, device.SECTION_NAME, PARAM_NAME,
			  device.NAME.c_str());

	config_save(obsDevice);
}

void Device::SetDefaults(t_device device)
{
	// OBS Device defaults
	config_t *obsDevice = GetConfigStore();
	if (obsDevice) {
		config_set_default_bool(obsDevice, device.SECTION_NAME,
					PARAM_ENABLED, device.Enabled);
		config_set_default_string(obsDevice, device.SECTION_NAME,
					  PARAM_NAME, device.NAME.c_str());
	}
	
}

config_t *Device::GetConfigStore()
{
	return obs_frontend_get_profile_config();
}



void Device::MigrateFromGlobalSettings(t_device device)
{
	config_t *source = obs_frontend_get_global_config();
	config_t *destination = obs_frontend_get_profile_config();

	if (config_has_user_value(source, device.SECTION_NAME, PARAM_ENABLED)) {
		bool value = config_get_bool(source, device.SECTION_NAME,
					     PARAM_ENABLED);
		config_set_bool(destination, device.SECTION_NAME, PARAM_ENABLED,
				value);

		config_remove_value(source, device.SECTION_NAME, PARAM_ENABLED);
	}

	if (config_has_user_value(source, device.SECTION_NAME, PARAM_NAME)) {
		std::string value = config_get_string(
			source, device.SECTION_NAME, PARAM_NAME);
		config_set_string(destination, device.SECTION_NAME, PARAM_NAME,
				  value.c_str());

		config_remove_value(source, device.SECTION_NAME, PARAM_NAME);
	}

	config_save(destination);
}
