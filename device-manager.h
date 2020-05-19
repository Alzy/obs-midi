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

#include <obs-frontend-api/obs-frontend-api.h>
#include <util/config-file.h>
#include <QtCore/QString>
#include <QtCore/QSharedPointer>
#include <vector>
#include "midi-agent.h"

using namespace std;

class DeviceManager {
	public:
		DeviceManager();
		~DeviceManager();
		void Load(obs_data_t* data);

		vector <string> GetPortsList();
		int GetPortNumberByDeviceName(const char* deviceName);

		vector<MidiAgent*> GetActiveMidiDevices();
		MidiAgent* GetMidiDeviceByName(const char* deviceName);
		vector <MidiHook *> GetMidiHooksByDeviceName(const char* deviceName);

		void RegisterMidiDevice(int port);

		obs_data_t* GetData();

	private:
		rtmidi::midi_in *rtMidi;
		vector<MidiAgent*> midiAgents;
};
