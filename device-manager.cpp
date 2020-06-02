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

#include "device-manager.h"

DeviceManager::DeviceManager()
{
	rtMidi = new rtmidi::midi_in();
	MO = new rtmidi::midi_out();
}

DeviceManager::~DeviceManager()
{
	rtMidi->~midi_in();
	MO->~midi_out();
}

/* Load the Device Manager from saved Config Store data.
 * This method is called from Config. Should only be called once on runtime
 */
void DeviceManager::Load(obs_data_t *data)
{
	vector<string> portsList = GetPortsList();
	vector<string> outPortsList = GetOutPortsList();
	obs_data_array_t *devicesData = obs_data_get_array(data, "devices");
	size_t deviceCount = obs_data_array_count(devicesData);
	for (size_t i = 0; i < deviceCount; i++) {
		obs_data_t *deviceData = obs_data_array_item(devicesData, i);
		MidiAgent *device = new MidiAgent();
		device->Load(deviceData);
		midiAgents.push_back(device);
		connect(this, SIGNAL(bcast(QString, QString)), device,
			SLOT(NewObsEvent(QString, QString)));
		if (device->isEnabled()) {
			int portNumber = GetPortNumberByDeviceName(
				device->GetName().c_str());
			int outPort = GetOutPortNumberByDeviceName(
				device->GetOutName().c_str());

			if (portNumber != -1) {
				device->OpenPort(portNumber);
			}
			if (outPort != -1) {
				device->OpenOutPort(outPort);
			}
		}
	}
}

/* Returns vector list of Port Names 
 */
vector<string> DeviceManager::GetPortsList()
{
	vector<string> ports;
	int portCount = rtMidi->get_port_count();
	for (int i = 0; i < portCount; i++) {
		ports.push_back(rtMidi->get_port_name(i));
	}
	return ports;
}
/* Returns vector list of Port Names 
 */
vector<string> DeviceManager::GetOutPortsList()
{
	opl.clear();
	vector<string> outports;
	int portCount = MO->get_port_count();
	for (int i = 0; i < portCount; i++) {
		outports.push_back(MO->get_port_name(i));
		opl.append(QString::fromStdString(MO->get_port_name(i)));
	}
	return outports;
}
QStringList DeviceManager::GetOPL()
{
	return opl;
}
/* Returns the port number of the specified device.
 * If the device isn't found (possibly due to being disconnected), returns -1
 */
int DeviceManager::GetPortNumberByDeviceName(const char *deviceName)
{
	vector<string> portsList = GetPortsList();
	auto it = find(portsList.begin(), portsList.end(), deviceName);
	if (it != portsList.end()) {
		return distance(portsList.begin(), it);
	} else {
		return -1;
	}
}

/* Returns the port number of the specified device.
 * If the device isn't found (possibly due to being disconnected), returns -1
 */
int DeviceManager::GetOutPortNumberByDeviceName(const char *deviceName)
{

	vector<string> portsList = GetOutPortsList();

	auto it = find(portsList.begin(), portsList.end(), deviceName);
	if (it != portsList.end()) {
		return distance(portsList.begin(), it);
	} else {
		return -1;
	}
}

vector<MidiAgent *> DeviceManager::GetActiveMidiDevices()
{
	return midiAgents;
}

MidiAgent *DeviceManager::GetMidiDeviceByName(const char *deviceName)
{
	for (int i = 0; i < midiAgents.size(); i++) {
		if (midiAgents.at(i)->GetName() == deviceName) {
			return midiAgents.at(i);
		}
	}
	return NULL;
}

vector<MidiHook *>
DeviceManager::GetMidiHooksByDeviceName(const char *deviceName)
{
	auto device = GetMidiDeviceByName(deviceName);
	if (device != NULL) {
		return device->GetMidiHooks();
	}
	throw("no midi hooks for this device");
}

/* Registers a midi device.
 * Will create, store and enable a midi device.
*/
void DeviceManager::RegisterMidiDevice(int port, int outport)
{

	MidiAgent *midiA = new MidiAgent();
	midiA->OpenPort(port);
	midiA->OpenOutPort(outport);

	midiAgents.push_back(midiA);
}

/* Get this Device Manager state as OBS Data. (includes devices and their midi hooks)
* This is needed to Serialize the state in the config.
* https://obsproject.com/docs/reference-settings.html
*/
obs_data_t *DeviceManager::GetData()
{
	obs_data_t *data = obs_data_create();

	obs_data_array_t *deviceData = obs_data_array_create();
	for (int i = 0; i < midiAgents.size(); i++) {
		obs_data_array_push_back(deviceData,
					 midiAgents.at(i)->GetData());
	}

	obs_data_set_array(data, "devices", deviceData);
	return data;
}

void DeviceManager::SendMidi(QString mtype, int channel, int norc, int value)

{

	//***Need to add message Deletion here***//
}
void DeviceManager::broadcast(const RpcEvent &event)
{
	OBSDataAutoRelease eventData = obs_data_create();

	QString updateType = event.updateType();
	obs_data_set_string(eventData, "update-type",
			    updateType.toUtf8().constData());

	std::optional<uint64_t> streamTime = event.streamTime();
	if (streamTime.has_value()) {
		QString streamingTimecode =
			Utils::nsToTimestamp(streamTime.value());
		obs_data_set_string(eventData, "stream-timecode",
				    streamingTimecode.toUtf8().constData());
	}

	std::optional<uint64_t> recordingTime = event.recordingTime();
	if (recordingTime.has_value()) {
		QString recordingTimecode =
			Utils::nsToTimestamp(recordingTime.value());
		obs_data_set_string(eventData, "rec-timecode",
				    recordingTimecode.toUtf8().constData());
	}

	OBSData additionalFields = event.additionalFields();
	if (additionalFields) {
		obs_data_apply(eventData, additionalFields);
	}

	//blog(1, "OBS EVENT %s -- %s", event.updateType().toStdString().c_str(),obs_data_get_json(eventData));
	emit bcast(event.updateType(),
		   QString::fromStdString(obs_data_get_json(eventData)));
};
