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
	Device(std::string name);
	~Device();
	struct t_device;

	t_device Load(std::string name);
	bool getEnabled(std::string name);
	void Save(t_device device);
	void SetDefaults(t_device device);
	void addFullRow(t_device device, std::string message, int channel);
	void addDefaultRow(t_device device, std::string message, int channel);
	void editRow(t_device device, std::string Mtype, int channel);
	void delRow(t_device device, std::string Mtype, int channel);
	config_t *GetConfigStore();

	void MigrateFromGlobalSettings(t_device device);



private:
};
#pragma once
