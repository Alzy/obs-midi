/*
obs-midi

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
#include <QtCore/QString>
#include <QtCore/QSharedPointer>
#include <vector>

#include "RtMidi.h"

using namespace std;

class MidiHook {
public:
	int index;
	string command;
	string param;

	MidiHook(int i, string c, string p) : index(i), command(c), param(p) {}
};


class MidiAgent {
	public:
		MidiAgent();
		~MidiAgent();

		void SetMidiDevice(int port);
		void UnsetMidiDevice();
		static void HandleInput(double deltatime,
				 vector<unsigned char> *message,
				 void *userData);
		void TriggerInputCommand(MidiHook *hook, int midiVal);
		void AddMidiHook(string mType, MidiHook *hook);
		void RemoveMidiHook(string mType, MidiHook *hook);


	private:
		RtMidiIn *midiin;
		string name;
		int port;
		vector<MidiHook*> noteOnHooks;
		vector<MidiHook*> noteOffHooks;
		vector<MidiHook*> ccHooks;

		vector<MidiHook *> &GetMidiHooksByType(string mType);
};
