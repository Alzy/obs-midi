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
#include "forms/settings-dialog.h"
#include <QtCore/QCryptographicHash>
#include <QtCore/QTime>
#include <QtWidgets/QSystemTrayIcon>
#include "forms/settings-dialog.h"
#define PARAM_ENABLED "device.Enabled"
#define PARAM_NAME "DeviceName"
std::string section = "MIDI-OBS-Device-";

#include "utils.h"

#include "device.hpp"

#define QT_TO_UTF8(str) str.toUtf8().constData()

Device::Device(std::string name)
{
	SettingsDialog *set;

	

	config_t *obsDevice = GetConfigStore();
	t_device x;
	//if device exists, ignore.
	//if device doesnt exist set defaults
	if (!getDeviceByName(name)) {
		t_device x;
		x.NAME = name;
			
		x.Enabled = true;
		Device::SetDefaults(x);
		
		config_save(obsDevice);
		Device::Load(name);
		
	} else {
		x = Device::Load(name);
	}
	//set->setCheck(x.Enabled); errors out due to window not being created yet?
	//obs_frontend_remove_event_callback(OnFrontendEvent, this);
}
bool Device::getDeviceByName(std::string name) {
	return false;
}

Device::~Device()
{
	
	//obs_frontend_remove_event_callback(OnFrontendEvent, this);
}
bool Device::getEnabled(std::string name)
{
	t_device x= Load(name);
	return x.Enabled;
}

t_device Device::Load(std::string name)
{
	t_device device;
	config_t *obsDevice = GetConfigStore();
	device.Enabled = config_get_bool(obsDevice, device.SECTION_NAME, PARAM_ENABLED);
	device.NAME = config_get_string(obsDevice, device.SECTION_NAME, PARAM_NAME);
	std::string en;
	if (device.Enabled)
	{
		en = "enabled";
	} else {
		en = "disabled";
	};
	blog(LOG_INFO, "EnableCheck- ");
	blog(LOG_INFO, en.c_str());
	
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
	
	return device;
}

void Device::Save(t_device device)

{

	
	config_t *obsDevice = GetConfigStore();

	config_set_bool(obsDevice, device.SECTION_NAME, PARAM_ENABLED, device.Enabled);
	config_set_string(obsDevice, device.SECTION_NAME, PARAM_NAME, device.NAME.c_str());
	for (int i = 0; i < device.total_rows; i++) {
		//load Each Row
		config_set_string(obsDevice, device.rows[i].ROW_NAME(device.SECTION_NAME), "MessageType", device.rows[i].MessageType.c_str());
		config_set_int   (obsDevice, device.rows[i].ROW_NAME(device.SECTION_NAME), "Channel",device.rows[i].channel  );
		config_set_int   (obsDevice, device.rows[i].ROW_NAME(device.SECTION_NAME), "InputType", device.rows[i].InputType );
		config_set_bool  (obsDevice, device.rows[i].ROW_NAME(device.SECTION_NAME), "Bidirectional", device.rows[i].bidirectional );
		config_set_int   (obsDevice, device.rows[i].ROW_NAME(device.SECTION_NAME), "Action", device.rows[i].Action   );
		config_set_int   (obsDevice, device.rows[i].ROW_NAME(device.SECTION_NAME), "Option1",device.rows[i].option1);
		config_set_int   (obsDevice, device.rows[i].ROW_NAME(device.SECTION_NAME), "Option2",device.rows[i].option2);
		config_set_int   (obsDevice, device.rows[i].ROW_NAME(device.SECTION_NAME), "Option3", device.rows[i].option3);
	}
	config_save(obsDevice);
}
void Device::addFullRow(t_device device, std::string message, int channel) {}
void Device::addDefaultRow(t_device device, std::string message, int channel)
{
	t_row row = device.rows[device.total_rows+1];
	row.MessageType = message;
	row.channel = channel;
	if (message == "control_change") {
		row.InputType=1;
	} else if (message == "note_on" || message == "note_off") {
		row.InputType =0;
	}
	row.bidirectional = false;
	row.Action=0;
	row.option1=1; //first value
	row.option2=0; //disabled value
	row.option3=0; //disabled Value
	device.total_rows += 1;
	//add table row to struct
	Save(device);
}
void Device::editRow(t_device device, std::string Mtype, int channel) {
	//based on Message type and channel number, find and edit save info
}
void Device::delRow(t_device device, std::string Mtype, int channel) {
	//based on Message type and channel number, find and delete save info

	device.total_rows -= 1;
	Save(device);
}


void Device::SetDefaults(t_device device)
{
	// OBS Device defaults
	config_t *obsDevice = GetConfigStore();
	if (obsDevice) {
		config_set_default_bool(obsDevice, device.SECTION_NAME,	PARAM_ENABLED, device.Enabled);
		config_set_default_string(obsDevice, device.SECTION_NAME, PARAM_NAME, device.NAME.c_str());
	}
	config_save(obsDevice);
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
