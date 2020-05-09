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

#define SECTION_NAME "MidiAPI"
#define PARAM_DEBUG "DebugEnabled"
#define PARAM_ALERT "AlertsEnabled"

#include "utils.h"

#include "config.h"

#define QT_TO_UTF8(str) str.toUtf8().constData()

Config::Config() :
	DebugEnabled(false),
	AlertsEnabled(true),
	SettingsLoaded(false)
{
	qsrand(QTime::currentTime().msec());

	SetDefaults();

	obs_frontend_add_event_callback(OnFrontendEvent, this);
}

Config::~Config()
{
	obs_frontend_remove_event_callback(OnFrontendEvent, this);
}

void Config::Load()
{
	config_t* obsConfig = GetConfigStore();

	DebugEnabled = config_get_bool(obsConfig, SECTION_NAME, PARAM_DEBUG);
	AlertsEnabled = config_get_bool(obsConfig, SECTION_NAME, PARAM_ALERT);
}

void Config::Save()
{
	config_t* obsConfig = GetConfigStore();

	config_set_bool(obsConfig, SECTION_NAME, PARAM_DEBUG, DebugEnabled);
	config_set_bool(obsConfig, SECTION_NAME, PARAM_ALERT, AlertsEnabled);

	config_save(obsConfig);
}

void Config::SetDefaults()
{
	// OBS Config defaults
	config_t* obsConfig = GetConfigStore();
	if (obsConfig) {
		config_set_default_bool(obsConfig,
			SECTION_NAME, PARAM_DEBUG, DebugEnabled);
		config_set_default_bool(obsConfig,
			SECTION_NAME, PARAM_ALERT, AlertsEnabled);
	}
}

config_t* Config::GetConfigStore()
{
	return obs_frontend_get_profile_config();
}

void Config::OnFrontendEvent(enum obs_frontend_event event, void* param)
{
	auto config = reinterpret_cast<Config*>(param);

	if (event == OBS_FRONTEND_EVENT_PROFILE_CHANGED) {
		config->SetDefaults();
		config->Load();
	}
}

void Config::MigrateFromGlobalSettings()
{
	config_t* source = obs_frontend_get_global_config();
	config_t* destination = obs_frontend_get_profile_config();

	if(config_has_user_value(source, SECTION_NAME, PARAM_DEBUG)) {
		bool value = config_get_bool(source, SECTION_NAME, PARAM_DEBUG);
		config_set_bool(destination, SECTION_NAME, PARAM_DEBUG, value);

		config_remove_value(source, SECTION_NAME, PARAM_DEBUG);
	}

	if(config_has_user_value(source, SECTION_NAME, PARAM_ALERT)) {
		bool value = config_get_bool(source, SECTION_NAME, PARAM_ALERT);
		config_set_bool(destination, SECTION_NAME, PARAM_ALERT, value);

		config_remove_value(source, SECTION_NAME, PARAM_ALERT);
	}

	config_save(destination);
}
