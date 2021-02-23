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
#include "forms/settings-dialog.h"


DeviceManager::DeviceManager()
{

	this->setParent(plugin_window);
}

DeviceManager::~DeviceManager()
{
	Unload();
	this->deleteLater();
}

/* Load the Device Manager from saved Config Store data.
 * This method is called from Config. Should only be called once on runtime
 */
void DeviceManager::Load(obs_data_t *data)
{
	QStringList portsList = GetPortsList();
	QStringList outPortsList = GetOutPortsList();
	obs_data_array_t *devicesData = obs_data_get_array(data, "devices");
	obs_data_release(data);
	size_t deviceCount = obs_data_array_count(devicesData);
	for (size_t i = 0; i < deviceCount; i++) {
		obs_data_t *deviceData = obs_data_array_item(devicesData, i);
		MidiAgent *device = new MidiAgent(deviceData);
		obs_data_release(deviceData);
		midiAgents.push_back(device);

		broadcast_connection = connect(this, SIGNAL(bcast(QString, QString)), device, SLOT(handle_obs_event(QString, QString)));
	}
	obs_data_array_release(devicesData);
}

void DeviceManager::Unload()
{
	blog(LOG_INFO, "UNLOADING DEVICE MANAGER");
	disconnect(broadcast_connection);
	for (auto & midiAgent : midiAgents)
	{
		blog(LOG_DEBUG, "Unloading Midi Device %s", midiAgent->get_midi_input_name().toStdString().c_str());
		midiAgent->clear_MidiHooks();
		delete midiAgent;
	}
	midiAgents.clear();
}

/*
 * Returns QStringList of Port Names
 */
QStringList DeviceManager::GetPortsList()
{
	QStringList ports;
	unsigned int portCount = rtmidi::midi_in().get_port_count();
	for (unsigned int i = 0; i < portCount; ++i)
	{
		ports.append(QString::fromStdString(rtmidi::midi_in().get_port_name(i)));
	}
	return std::move(ports);
}

/*
 * Returns QStringList of Output  Port Names
 */
QStringList DeviceManager::GetOutPortsList()
{
	QStringList outports;
	unsigned int portCount = rtmidi::midi_out().get_port_count();
	for (unsigned int i = 0; i < portCount; ++i)
	{
		outports.append(QString::fromStdString(
			rtmidi::midi_out().get_port_name(i)));
	}
	return std::move(outports);
}

/*
 * Returns the port number of the specified device.
 * If the device isn't found (possibly due to being disconnected), returns -1
 */
int DeviceManager::GetPortNumberByDeviceName(const QString &deviceName)
{
	QStringList portsList = GetPortsList();

	if (portsList.contains(deviceName)) {
		return portsList.indexOf(deviceName);
	} else {
		return -1;
	}
}
/**
 *
 * 
 * @name GetOutPortNumberByDeviceName
 * @Param deviceName 
 * @category Device Manager
 * @description Returns the port number of the specified device. \
 *		If the device isn't found (possibly due to being disconnected), returns -1
 * @returns  Device Output Port
 * @rtype int
 */
int DeviceManager::GetOutPortNumberByDeviceName(const QString &deviceName)
{
	QStringList portsList = GetOutPortsList();

	if (portsList.contains(deviceName)) {
		return portsList.indexOf(deviceName);
	} else {
		return -1;
	}
}

QVector<MidiAgent *> DeviceManager::GetActiveMidiDevices()
{
	return midiAgents;
}

MidiAgent *DeviceManager::GetMidiDeviceByName(const QString &deviceName)
{
	MidiAgent *returndevice = nullptr;
	for (auto midiAgent : midiAgents)
	{
		if (midiAgent->get_midi_input_name() == deviceName)
		{
			returndevice = midiAgent;
			break;
		}
	}
	return returndevice;
}

QVector<MidiHook *> DeviceManager::GetMidiHooksByDeviceName(const QString &deviceName)
{
	if (deviceName != QString("No Devices Available"))
	{
		auto device = GetMidiDeviceByName(deviceName);
		if (device != nullptr)
		{
			return std::move(device->GetMidiHooks());
		}
		else
		{
			return {};
		}
	}
}

/* Registers a midi device.
 * Will create, store and enable a midi device.
 */
MidiAgent *DeviceManager::RegisterMidiDevice(const int &port, const int &outport)
{
	MidiAgent *midiA = new MidiAgent(port, outport);
	midiA->set_enabled(true);
	midiAgents.push_back(midiA);
	return midiA;
}

/* Get this Device Manager state as OBS Data. (includes devices and their midi hooks)
 * This is needed to Serialize the state in the config.
 * https://obsproject.com/docs/reference-settings.html
 */
obs_data_t *DeviceManager::GetData()
{
	obs_data_t* data = obs_data_create();

	obs_data_array_t* deviceData = obs_data_array_create();
	for (auto midiAgent : midiAgents)
	{
		obs_data_array_push_back(deviceData, midiAgent->GetData());
	}

	obs_data_set_array(data, "devices", deviceData);
	return data;
}

void DeviceManager::broadcast_obs_event(const RpcEvent &event)
{
	OBSDataAutoRelease eventData = obs_data_create();

	const QString& updateType = event.updateType();
	obs_data_set_string(eventData, "update-type", updateType.toUtf8().constData());

	OBSData additionalFields = event.additionalFields();
	if (additionalFields)
	{
		obs_data_apply(eventData, additionalFields);
	}
	if (broadcast_connection)
	{
		blog(1, "OBS EVENT %s -- %s", event.updateType().toStdString().c_str(), obs_data_get_json(eventData));
		emit bcast(event.updateType(), QString::fromStdString(obs_data_get_json(eventData)));
	}
	obs_data_release(additionalFields);
	obs_data_release(eventData);
};
