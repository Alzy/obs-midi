#pragma once
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

#if __has_include(<obs-frontend-api.h>)
#else
#include <obs-frontend-api/obs-frontend-api.h>
#include "rtmidi17/rtmidi17.hpp"
#endif
#include "utils.h"
#include "midi-agent.h"
#include "rpc/RpcEvent.h"
class MidiResponse : public QObject {
	Q_OBJECT
public:
	MidiResponse(const MidiAgent &midi_agent, const RpcEvent &rpc_event);
	~MidiResponse();



private:
	void source_volume_changed();

	void switch_scenes();

	void preview_scene_changed();

	void transition_begin();

	void source_mute_state_changed();

	void source_renamed();

	void exiting();

	void source_destroyed();
	MidiAgent agent;
	RpcEvent rpcevent;
	MidiHook *hook;
};
