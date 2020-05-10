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
#include "RtMidi.h"

class MidiAgent {
	public:
		MidiAgent();
		~MidiAgent();

		void SetMidiDevice(int port);
		void UnsetMidiDevice();
		static void HandleInput(double deltatime,
				 std::vector<unsigned char> *message,
				 void *userData);

		void SetVolume(QString source, float volume);

	private:
		RtMidiIn *midiin;
		int port;


};
