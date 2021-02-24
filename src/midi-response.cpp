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
#include "midi-response.h"

MidiResponse::MidiResponse(const MidiAgent &midi_agent, const RpcEvent &rpc_event)
{

	agent = midi_agent;
	rpcevent = rpc_event;
	


}
MidiResponse::~MidiResponse() {}
void MidiResponse::source_volume_changed() {
	double vol = obs_data_get_double(data, "volume");
	uint8_t newvol = Utils::mapper2(cbrt(vol));
	QString source = QString(obs_data_get_string(data, "sourceName"));
	for (int i = 0; i < this->midiHooks.size(); i++) {
		if (this->midiHooks.at(i)->action == Utils::translate_action(ActionsClass::Actions::Set_Volume) &&
		    this->midiHooks.at(i)->audio_source == source) {
			message->message_type = this->midiHooks.at(i)->message_type;
			message->channel = this->midiHooks.at(i)->channel;
			message->NORC = this->midiHooks.at(i)->norc;
			message->value = newvol;
			this->send_message_to_midi_device(message->get());
		}
	}
}
void MidiResponse::switch_scenes()
{
	QString source = QString::fromStdString(obs_data_get_string(data, "scene-name"));
	for (int i = 0; i < this->midiHooks.size(); i++) {
		if (this->midiHooks.at(i)->action == Utils::translate_action(ActionsClass::Actions::Set_Current_Scene) &&
		    this->midiHooks.at(i)->scene == source) {
			message->message_type = "Note Off";
			message->channel = this->midiHooks.at(i)->channel;
			message->NORC = lastscenebtn;
			message->value = 0;
			this->send_message_to_midi_device(message->get());
			message->NORC = this->midiHooks.at(i)->norc;
			message->message_type = "Note On";
			message->value = 1;
			this->send_message_to_midi_device(message->get());
			lastscenebtn = this->midiHooks.at(i)->norc;
		}
	}
}
void MidiResponse::preview_scene_changed()
{
	QString source = QString::fromStdString(obs_data_get_string(data, "scene-name"));
	for (int i = 0; i < this->midiHooks.size(); i++) {
		if (this->midiHooks.at(i)->action == Utils::translate_action(ActionsClass::Actions::Set_Preview_Scene) &&
		    this->midiHooks.at(i)->scene == source) {
			message->message_type = "Note Off";
			message->channel = this->midiHooks.at(i)->channel;
			message->NORC = last_preview_scene_norc;
			message->value = 0;
			this->send_message_to_midi_device(message->get());
			message->NORC = this->midiHooks.at(i)->norc;
			message->message_type = "Note On";
			message->value = 1;
			this->send_message_to_midi_device(message->get());
			last_preview_scene_norc = this->midiHooks.at(i)->norc;
		}
	}
}
void MidiResponse::transition_begin()
{
	QString from = obs_data_get_string(data, "from-scene");
	for (int i = 0; i < this->midiHooks.size(); i++) {
		if (this->midiHooks.at(i)->action == Utils::translate_action(ActionsClass::Actions::Set_Current_Scene) &&
		    this->midiHooks.at(i)->scene == from) {
			message->channel = this->midiHooks.at(i)->channel;
			message->message_type = "Note On";
			message->NORC = this->midiHooks.at(i)->norc;
			message->value = 0;
			this->send_message_to_midi_device(message->get());
			message->message_type = "Note Off";
			this->send_message_to_midi_device(message->get());
		}
	}
}
void MidiResponse::source_mute_state_changed()
{
	QString from = obs_data_get_string(data, "sourceName");
	for (int i = 0; i < this->midiHooks.size(); i++) {
		if (this->midiHooks.at(i)->action == Utils::translate_action(ActionsClass::Actions::Toggle_Mute) &&
		    this->midiHooks.at(i)->audio_source == from) {
			bool muted = obs_data_get_bool(data, "muted");
			message->message_type = "Note On";
			message->channel = this->midiHooks.at(i)->channel;
			message->NORC = this->midiHooks.at(i)->norc;
			message->value = muted;
			this->send_message_to_midi_device(message->get());
		}
	}
}
void MidiResponse::source_renamed()
{
	QString from = obs_data_get_string(data, "previousName");
	for (int i = 0; i < this->midiHooks.size(); i++) {
		if (this->midiHooks.at(i)->source == from) {
			this->midiHooks.at(i)->source = obs_data_get_string(data, "newName");
			this->GetData();
		}
	}
}
void MidiResponse::exiting()
{
	closing = true;
}
void MidiResponse::source_destroyed() {
	if (!closing) {
		QString from = obs_data_get_string(data, "sourceName");
		for (int i = 0; i < this->midiHooks.size(); i++) {
			if (this->midiHooks.at(i)->source == from) {
				this->remove_MidiHook(this->midiHooks.at(i));
				this->GetData();
				i--;
			}
		}
		GetConfig()->Save();
	}
}

