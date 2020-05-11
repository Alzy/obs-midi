/*
obs-websocket
Copyright (C) 2016-2019	Stéphane Lepin <stephane.lepin@gmail.com>

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

#pragma once

#include <obs-frontend-api.h>
#include <util/config-file.h>
#include <QtCore/QString>
#include <QtCore/QSharedPointer>

class Device {
public:
	Device();
	~Device();
	void Load(std::string NAME);
	void Save(std::string NAME);
	void SetDefaults(std::string NAME);
	config_t *GetConfigStore();

	void MigrateFromGlobalSettings(std::string NAME);

	bool DeviceEnabled;
	std::string DeviceName;

	bool SettingsLoaded;

private:
};
#pragma once
