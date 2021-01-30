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

#if __has_include(<obs-frontend-api.h>)
#include <obs-frontend-api.h>
#include "rtmidi17/rtmidi17.hpp"
#else
#include <obs-frontend-api/obs-frontend-api.h>
#include "RtMidi17/rtmidi17.hpp"
#endif
#include <QtCore/QString>
#include <QtCore/QSharedPointer>
#include <vector>
#include <QObject>
#include <functional>
#include <map>
#include <string>
#include <iostream>
#include "obs-controller.h"
#include "rpc/RpcEvent.h"
#include "utils.h"

class MidiHook {
public:
	int channel;          //midi channel
	QString message_type; // Message Type
	int norc;             // Note or Control
	QString action;
	QString scene;
	QString source;
	QString filter;
	QString transition;
	QString item;
	QString audio_source;
	QString media_source;
	int duration = -1;
	QString scene_collection;
	QString profile;
	QString string_override;
	bool bool_override;
	int int_override;

	MidiHook(){};

	MidiHook(QString jsonString)
	{
		obs_data_t *data = obs_data_create_from_json(
			jsonString.toStdString().c_str());
		channel = obs_data_get_int(data, "channel");
		message_type = obs_data_get_string(data, "message_type");
		norc = obs_data_get_int(data, "norc");
		action = obs_data_get_string(data, "action");
		scene = obs_data_get_string(data, "scene");
		source = obs_data_get_string(data, "source");
		filter = obs_data_get_string(data, "filter");
		transition = obs_data_get_string(data, "transition");
		item = obs_data_get_string(data, "item");
		audio_source = obs_data_get_string(data, "audio_source");
		media_source = obs_data_get_string(data, "media_source");
		duration = obs_data_get_int(data, "duration");
		scene_collection =
			obs_data_get_string(data, "scene_collection");
		profile = obs_data_get_string(data, "profile");
		string_override = obs_data_get_string(data, "string_override");
		bool_override = obs_data_get_bool(data, "bool_override");
		int_override = obs_data_get_int(data, "int_override");
	}

	obs_data_t *GetData()
	{
		obs_data_t *data = obs_data_create();
		obs_data_set_int(data, "channel", channel);
		obs_data_set_string(data, "message_type",
				    message_type.toStdString().c_str());
		obs_data_set_int(data, "norc", norc);
		obs_data_set_string(data, "action",
				    action.toStdString().c_str());
		obs_data_set_string(data, "scene", scene.toStdString().c_str());
		obs_data_set_string(data, "source",
				    source.toStdString().c_str());
		obs_data_set_string(data, "filter",
				    filter.toStdString().c_str());
		obs_data_set_string(data, "transition",
				    transition.toStdString().c_str());
		obs_data_set_string(data, "item", item.toStdString().c_str());
		obs_data_set_string(data, "audio_source",
				    audio_source.toStdString().c_str());
		obs_data_set_string(data, "media_source",
				    media_source.toStdString().c_str());
		obs_data_set_int(data, "duration", duration);
		obs_data_set_string(data, "scene_collection",
				    scene_collection.toStdString().c_str());
		obs_data_set_string(data, "profile",
				    profile.toStdString().c_str());
		obs_data_set_string(data, "string_override",
				    string_override.toStdString().c_str());
		obs_data_set_bool(data, "bool_override", bool_override);
		obs_data_set_int(data, "int_override", int_override);

		return data;
	}

	const char *ToJSON() { return obs_data_get_json(GetData()); }
};

class MidiAgent : public QObject {
	Q_OBJECT

public:
	MidiAgent();
	~MidiAgent();
	void Load(obs_data_t *data);

	void OpenPort(int inport);
	void OpenOutPort(int outport);
	void ClosePort();
	void executeAction(MidiHook *hook, int MidiVal, Actions action);
	QString GetName();
	QString GetOutName();
	void SetOutName(QString oname);
	int GetPort();
	bool isEnabled();
	bool isConnected();
	bool isBidirectional();
	bool setBidirectional(bool state);
	static void HandleInput(const rtmidi::message &message, void *userData);
	void TriggerInputAction(MidiHook *hook, int midiVal);
	void SendMessage(MidiMessage mess);
	QVector<MidiHook *> GetMidiHooks();
	void AddMidiHook(MidiHook *hook);
	void RemoveMidiHook(MidiHook *hook);
	void ClearMidiHooks();
	obs_data_t *GetData();

public slots:
	void NewObsEvent(QString eventType, QString eventData);
signals:
	void SendNewUnknownMessage(MidiMessage);

private:
	void send(QString type, int channel, int norc, int value = 0);
	rtmidi::midi_in *midiin;
	rtmidi::midi_out *midiout;
	QString name;
	QString outname;
	bool sending;
	int port;
	int lastscenebtn;
	bool enabled;
	bool connected;
	bool bidirectional;
	bool closing = false;
	QVector<MidiHook *> midiHooks;
};
