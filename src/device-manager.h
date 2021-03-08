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

#include <set>
#include <vector>

#include <QtCore/QString>
#include <QtCore/QObject>
#include <QtCore/QMutex>
#include <QtCore/QSharedPointer>
#include <QtCore/QVariantHash>
#include <QtCore/QThreadPool>

#include <util/config-file.h>
#if __has_include(<obs-frontend-api.h>)
#include <obs-frontend-api.h>
#else
#include <obs-frontend-api/obs-frontend-api.h>
#endif

#include "rpc/RpcEvent.h"
#include "midi-agent.h"
#include "obs-controller.h"

class DeviceManager : public QObject {
	Q_OBJECT
public:
	DeviceManager();
	~DeviceManager() override;

	void Load(QString datastring);
	void Unload();

	QStringList get_input_ports_list();
	int get_input_port_number(const QString &deviceName);
	QStringList get_output_ports_list();
	int get_output_port_number(const QString &deviceName);

	QVector<MidiAgent *> get_active_midi_devices();
	MidiAgent *get_midi_device(const QString &deviceName);
	QVector<MidiHook *> get_midi_hooks(const QString &deviceName);
	MidiAgent *register_midi_device(const int &port, std::optional<int> outport = std::nullopt);

	QString GetData();
	void reload();
signals:
	void reload_config();
	void obsEvent(const RpcEvent &event);

private:
	QVector<MidiAgent *> midiAgents;
};
