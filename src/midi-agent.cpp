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

#include <functional>
#include <string>
#include <utility>

#include <QtCore/QTime>

#include "utils.h"
#include "midi-agent.h"
#include "obs-midi.h"
#include "events.h"
#include "config.h"
#include "device-manager.h"

using namespace std;

////////////////
// MIDI AGENT //
////////////////
MidiAgent::MidiAgent(const int &in_port, const int &out_port)
{
	set_input_port(in_port);
	set_output_port(out_port);
	this->setParent(GetDeviceManager().get());
	set_callbacks();
}
MidiAgent::MidiAgent(obs_data_t *midiData)
{
	// Sets the parent of this instance of MidiAgent to Device Manager
	this->setParent(GetDeviceManager().get());
	// Sets the Midi Callback function
	this->Load(midiData);
	set_callbacks();
	if (enabled)
		open_midi_input_port();
	if (bidirectional)
		open_midi_output_port();
}
void MidiAgent::set_callbacks()
{
	connect(GetEventsSystem().get(), &Events::obsEvent, this, &MidiAgent::handle_obs_event);
	midiin.set_callback([this](const auto &message) { HandleInput(message, this); });
	midiin.set_error_callback([this](const auto &error_type, const auto &error_message) { HandleError(error_type, error_message, this); });
	midiout.set_error_callback([this](const auto &error_type, const auto &error_message) { HandleError(error_type, error_message, this); });
}
MidiAgent::~MidiAgent()
{
	
	clear_MidiHooks();
	close_both_midi_ports();
	midiin.cancel_callback();
}
/* Loads information from OBS data. (recalled from Config)
 * This will not enable the MidiAgent or open the port. (and shouldn't)
 */
void MidiAgent::Load(obs_data_t *data)
{
	obs_data_set_default_bool(data, "enabled", false);
	obs_data_set_default_bool(data, "bidirectional", false);
	midi_input_name = QString(obs_data_get_string(data, "name"));
	midi_output_name = QString(obs_data_get_string(data, "outname"));
	input_port = DeviceManager().GetPortNumberByDeviceName(midi_input_name);
	output_port = DeviceManager().GetOutPortNumberByDeviceName(midi_output_name);
	enabled = obs_data_get_bool(data, "enabled");
	bidirectional = obs_data_get_bool(data, "bidirectional");
	obs_data_array_t *hooksData = obs_data_get_array(data, "hooks");
	size_t hooksCount = obs_data_array_count(hooksData);
	for (size_t i = 0; i < hooksCount; i++) {
		obs_data_t *hookData = obs_data_array_item(hooksData, i);
		MidiHook *mh = new MidiHook();
		mh->message_type = obs_data_get_string(hookData, "message_type");
		mh->norc = obs_data_get_int(hookData, "norc");
		mh->channel = obs_data_get_int(hookData, "channel");
		mh->action = obs_data_get_string(hookData, "action");
		mh->scene = obs_data_get_string(hookData, "scene");
		mh->source = obs_data_get_string(hookData, "source");
		mh->filter = obs_data_get_string(hookData, "filter");
		mh->transition = obs_data_get_string(hookData, "transition");
		mh->item = obs_data_get_string(hookData, "item");
		mh->audio_source = obs_data_get_string(hookData, "audio_source");
		mh->media_source = obs_data_get_string(hookData, "media_source");
		mh->duration = obs_data_get_int(hookData, "duration");
		mh->scene_collection = obs_data_get_string(hookData, "scene_collection");
		mh->profile = obs_data_get_string(hookData, "profile");
		mh->string_override = obs_data_get_string(hookData, "string_override");
		mh->bool_override = obs_data_get_bool(hookData, "bool_override");
		mh->int_override = obs_data_get_int(hookData, "int_override");
		add_MidiHook(mh);
	}
}
void MidiAgent::set_input_port(const int port)
{
	input_port = port;
	midi_input_name = QString::fromStdString(midiin.get_port_name(port));
}
void MidiAgent::set_output_port(const int port)
{
	output_port = port;
	midi_output_name = QString::fromStdString(midiout.get_port_name(port));
}
/* Will open the port and enable this MidiAgent
 */
void MidiAgent::open_midi_input_port()
{
	if (!midiin.is_port_open()) {
		try {
			midiin.open_port(input_port);
		} catch (const rtmidi::midi_exception &error) {
			blog(LOG_DEBUG, "Midi Error %s", error.what());
		}
		blog(LOG_INFO, "MIDI device connected In: [%d] %s", input_port, midi_input_name.toStdString().c_str());
	}
}
void MidiAgent::open_midi_output_port()
{
	if (!midiout.is_port_open()) {

		try {
			midiout.open_port(output_port);
		} catch (const rtmidi::midi_exception &error) {
			blog(LOG_DEBUG, "Midi Error %s", error.what());
		}
		blog(LOG_INFO, "MIDI device connected Out: [%d] %s", output_port, midi_output_name.toStdString().c_str());
	}
}
/* Will close the port and disable this MidiAgent
 */
void MidiAgent::close_both_midi_ports()
{
	close_midi_input_port();
	close_midi_output_port();
}
void MidiAgent::close_midi_input_port()
{
	if (midiin.is_port_open()) {
		midiin.close_port();
	}
}
void MidiAgent::close_midi_output_port()
{
	if (midiout.is_port_open()) {
		midiout.close_port();
	}
}
const QString &MidiAgent::get_midi_input_name()
{
	return midi_input_name;
}
const QString &MidiAgent::get_midi_output_name()
{
	return midi_output_name;
}
void MidiAgent::set_midi_output_name(const QString &oname)
{
	midi_output_name = oname;
}
bool MidiAgent::set_bidirectional(const bool &state)
{
	this->bidirectional = state;
	if (!state) {
		if (midiout.is_port_open()) {
			midiout.close_port();
		}
	} else {
		open_midi_output_port();
	}
	GetConfig().get()->Save();
	return state;
}
int MidiAgent::GetPort() const
{
	return input_port;
}
bool MidiAgent::isEnabled() const
{
	return enabled;
}
bool MidiAgent::isBidirectional() const
{
	return bidirectional;
}
bool MidiAgent::isConnected() const
{
	return connected;
}
/* Midi input callback.
 * Extend input handling functionality here.
 * For OBS action triggers, edit the funcMap instead.
 */
void MidiAgent::HandleInput(const rtmidi::message &message, void *userData)
{
	MidiAgent *self = static_cast<MidiAgent *>(userData);
	if (!self->enabled) {
		return;
	}
	/*************Get Message parts***********/
	self->sending = true;
	MidiMessage *x = new MidiMessage();
	x->set_message(message);
	/***** Send Messages to emit function *****/
	x->device_name = self->get_midi_input_name();
	emit self->broadcast_midi_message((MidiMessage)*x);
	/** check if hook exists for this note or cc norc and launch it **/
	OBSController *obsc = new OBSController(self->get_midi_hook_if_exists(x), x->value);
	delete x;
	delete obsc;
}
void MidiAgent::HandleError(const rtmidi::midi_error &error_type, const std::string_view &error_message, void *userData)
{
	blog(LOG_ERROR, "Midi Error: %s", error_message.data());
}
/* Get the midi hooks for this device
 */
QVector<MidiHook *> MidiAgent::GetMidiHooks()
{
	return midiHooks;
}
MidiHook *MidiAgent::get_midi_hook_if_exists(MidiMessage *message)
{
	for (auto midiHook : this->midiHooks) {
		if (midiHook->message_type == message->message_type && midiHook->norc == message->NORC && midiHook->channel == message->channel) {
			return midiHook;
		}
	}
	return nullptr;
}
void MidiAgent::add_MidiHook(MidiHook *hook)
{
	// Add a new MidiHook
	midiHooks.push_back(hook);
}
void MidiAgent::set_enabled(const bool &state)
{
	this->enabled = state;
	if (state)
		open_midi_input_port();
	else
		close_midi_input_port();

	GetConfig().get()->Save();
}
void MidiAgent::set_midi_hooks(QVector<MidiHook *> mh)
{
	midiHooks = std::move(mh);
}
void MidiAgent::remove_MidiHook(MidiHook *hook)
{
	// Remove a MidiHook
	if (midiHooks.contains(hook)) {
		midiHooks.removeOne(hook);
	}
}
/* Clears all the MidiHooks for this device.
 */
void MidiAgent::clear_MidiHooks()
{
	for (int i = 0; i < midiHooks.count(); i++) {
		delete midiHooks.at(i);
	}
	midiHooks.clear();
}
/* Get this MidiAgent state as OBS Data. (includes midi hooks)
 * This is needed to Serialize the state in the config.
 * https://obsproject.com/docs/reference-settings.html
 */
obs_data_t *MidiAgent::GetData()
{
	OBSData data = obs_data_create();
	obs_data_set_string(data, "name", midi_input_name.toStdString().c_str());
	obs_data_set_string(data, "outname", midi_output_name.toStdString().c_str());
	obs_data_set_bool(data, "enabled", enabled);
	obs_data_set_bool(data, "bidirectional", bidirectional);
	OBSDataArrayAutoRelease arrayData = obs_data_array_create();
	for (int i = 0; i < midiHooks.size(); i++) {
		OBSData hookData = midiHooks.at(i)->GetData();
		obs_data_array_push_back(arrayData, hookData);
		obs_data_release(hookData);
		
	}
	obs_data_set_array(data, "hooks", arrayData);
	return data;
}
/**
 * Get Midi Hook, For use with events
 */
MidiHook *MidiAgent::get_midi_hook_if_exists(const RpcEvent &event)
{

	for (int i = 0; i < this->midiHooks.size(); i++) {
		bool found = false;
		switch (ActionsClass::string_to_action(Utils::untranslate(midiHooks.at(i)->action))) {
		case ActionsClass::Actions::Set_Volume:
		case ActionsClass::Actions::Toggle_Mute:
			found = (midiHooks.at(i)->audio_source == QString(obs_data_get_string(event.additionalFields(), "sourceName")));
			break;
		case ActionsClass::Actions::Set_Preview_Scene:
		case ActionsClass::Actions::Set_Current_Scene:
			found = (midiHooks.at(i)->scene == QString(obs_data_get_string(event.additionalFields(), "scene-name")));
			break;
		case ActionsClass::Actions::Toggle_Start_Stop_Recording:
		case ActionsClass::Actions::Start_Recording:
		case ActionsClass::Actions::Stop_Recording:
			found = ((event.updateType() == "RecordingStarted") || (event.updateType() == "RecordingStopped") ||
				 (event.updateType() == "RecordingStopping"));
			break;
		case ActionsClass::Actions::Toggle_Start_Stop_Streaming:
		case ActionsClass::Actions::Start_Streaming:
		case ActionsClass::Actions::Stop_Streaming:
			found = ((event.updateType() == "StreamStarted") || (event.updateType() == "StreamStopped") ||
				 (event.updateType() == "StreamStopping"));
			break;
		}
		if (found)
			return midiHooks.at(i);
	}
	return NULL;
}

/*Handle OBS events*/
void MidiAgent::handle_obs_event(const RpcEvent &event)
{
	MidiHook *hook = get_midi_hook_if_exists(event);

	
	blog(LOG_DEBUG, "OBS Event : %s \n AD: %s", event.updateType().toStdString().c_str(), obs_data_get_json(event.additionalFields()));
	if (!this->sending) {

		// ON EVENT TYPE Find matching hook, pull data from that hook, and do thing.
		if (hook != NULL) {
			MidiMessage *message = 	hook->get_message_from_hook();
			if (event.updateType() == QString("SourceVolumeChanged")) {
				double vol = obs_data_get_double(event.additionalFields(), "volume");
				uint8_t newvol = Utils::mapper2(cbrt(vol));
				
				message->value = newvol;
				this->send_message_to_midi_device(std::move(message));
			} else if (event.updateType() == QString("SwitchScenes")) {
				message->message_type = "Note Off";
				message->channel = hook->channel;
				message->NORC = lastscenebtn;
				message->value = 0;
				this->send_message_to_midi_device(std::move(message));
				message->NORC = hook->norc;
				message->message_type = "Note On";
				message->value = 1;
				this->send_message_to_midi_device(std::move(message));
				lastscenebtn = hook->norc;

			} else if (event.updateType() == QString("PreviewSceneChanged")) {
				message->message_type = "Note Off";
				message->channel = hook->channel;
				message->NORC = last_preview_scene_norc;
				message->value = 0;
				this->send_message_to_midi_device(std::move(message));
				message->NORC = hook->norc;
				message->message_type = "Note On";
				message->value = 1;
				this->send_message_to_midi_device(std::move(message));
				last_preview_scene_norc = hook->norc;
			} else if (event.updateType() == QString("SourceMuteStateChanged")) {
				bool muted = obs_data_get_bool(event.additionalFields(), "muted");
				if (muted) {
					message->value = 2;
				} else {
					message->value = muted;
				}
				message->message_type = "Note On";
				message->channel = hook->channel;
				message->NORC = hook->norc;
				this->send_message_to_midi_device(std::move(message));
			} else if (event.updateType() == QString("StreamStarted")) {
				message->message_type = "Note On";
				message->channel = hook->channel;
				message->NORC = hook->norc;
				message->value = 2;
				this->send_message_to_midi_device(std::move(message));

			} else if (event.updateType() == QString("StreamStopped")) {
				message->message_type = "Note Off";
				message->channel = hook->channel;
				message->NORC = hook->norc;
				message->value = 0;
				this->send_message_to_midi_device(std::move(message));

			} else if (event.updateType() == QString("StreamStopping")) {
				message->message_type = "Note On";
				message->channel = hook->channel;
				message->NORC = hook->norc;
				message->value = 2;
				this->send_message_to_midi_device(std::move(message));

			} else if (event.updateType() == QString("RecordingStarted")) {
				message->message_type = "Note On";
				message->channel = hook->channel;
				message->NORC = hook->norc;
				message->value = 2;
				this->send_message_to_midi_device(std::move(message));

			} else if (event.updateType() == QString("RecordingStopped")) {
				message->message_type = "Note Off";
				message->channel = hook->channel;
				message->NORC = hook->norc;
				message->value = 0;
				this->send_message_to_midi_device(std::move(message));
			} else if (event.updateType() == QString("RecordingStopping")) {
				message->message_type = "Note On";
				message->channel = hook->channel;
				message->NORC = hook->norc;
				message->value = 2;
				this->send_message_to_midi_device(std::move(message));
			}
			delete (message);
		}
		MidiMessage *message = new MidiMessage();
		if (event.updateType() == QString("TransitionBegin")) {
			QString from = obs_data_get_string(event.additionalFields(), "from-scene");
			for (int i = 0; i < this->midiHooks.size(); i++) {
				if (this->midiHooks.at(i)->action == Utils::translate_action(ActionsClass::Actions::Set_Current_Scene) &&
				    this->midiHooks.at(i)->scene == from) {
					message->channel = this->midiHooks.at(i)->channel;
					message->message_type = "Note On";
					message->NORC = this->midiHooks.at(i)->norc;
					message->value = 0;
					this->send_message_to_midi_device(std::move(message));
					message->message_type = "Note Off";
					this->send_message_to_midi_device(std::move(message));
				}
			}
		} else if (event.updateType() == QString("SourceRenamed")) {
			QString from = obs_data_get_string(event.additionalFields(), "previousName");
			for (int i = 0; i < this->midiHooks.size(); i++) {
				if (this->midiHooks.at(i)->source == from) {
					this->midiHooks.at(i)->source = obs_data_get_string(event.additionalFields(), "newName");
					this->GetData();
				}
			}
		} else if (event.updateType() == QString("Exiting")) {
			disconnect(GetEventsSystem().get(), &Events::obsEvent, this, &MidiAgent::handle_obs_event);
			closing = true;
		} else if (event.updateType() == QString("SourceDestroyed")) {
			if (!closing) {
				QString from = obs_data_get_string(event.additionalFields(), "sourceName");
				for (int i = 0; i < this->midiHooks.size(); i++) {
					if (this->midiHooks.at(i)->source == from) {
						this->remove_MidiHook(this->midiHooks.at(i));
						this->GetData();
					}
				}
				GetConfig()->Save();
			}
		} else if (event.updateType() == QString("ProfileChanged") || event.updateType() == QString("SceneCollectionChanged")) {
			GetDeviceManager().get()->reload();
		}
		delete (message);

	} else {
		this->sending = false;
	}
	
}
void MidiAgent::send_message_to_midi_device(MidiMessage *message)
{
	if (message != NULL) {

		std::unique_ptr<rtmidi::message> hello = std::make_unique<rtmidi::message>();
		if (message->message_type == "Control Change") {
			this->midiout.send_message(hello->control_change(message->channel, message->NORC, message->value));
		} else if (message->message_type == "Note On") {
			this->midiout.send_message(hello->note_on(message->channel, message->NORC, message->value));
		} else if (message->message_type == "Note Off") {
			this->midiout.send_message(hello->note_off(message->channel, message->NORC, message->value));
		}
	}
}
