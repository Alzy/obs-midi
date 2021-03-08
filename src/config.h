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
#include "obs-data.h"
#include <util/config-file.h>
#include <util/util.hpp>
#include <util/platform.h>
#include <qobject.h>
#include "device-manager.h"

class Config : QObject {
	Q_OBJECT
public:
	Config();
	~Config() override;
	QString get_file_name(std::optional<QString> prepend = std::nullopt);
	QString GetConfigStore(std::optional<QString> prepend = std::nullopt);
	bool DebugMode;
public slots:
	void Load();
	void Save();
};
