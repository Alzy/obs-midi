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
}
DeviceManager::~DeviceManager()
{
	Unload();
}
/* Load the Device Manager from saved Config Store data.
 * This method is called from Config. Should only be called once on runtime
 */
void DeviceManager::Load(QString datastring)
{
	obs_data_t *incoming_data = obs_data_create_from_json(datastring.toStdString().c_str());
	obs_data_array_t *data = obs_data_get_array(incoming_data, "MidiDevices");
	size_t deviceCount = obs_data_array_count(data);
	for (size_t i = 0; i < deviceCount; i++) {
		obs_data_t *madata = obs_data_array_item(data, i);
		MidiAgent *device = new MidiAgent(obs_data_get_json(madata));
		obs_data_release(madata);
		midiAgents.push_back(device);
	}
	obs_data_array_release(data);
	obs_data_release(incoming_data);
	blog(LOG_DEBUG, "DM::Load");
}
void DeviceManager::Unload()
{
	blog(LOG_INFO, "UNLOADING DEVICE MANAGER");
	for (auto &midiAgent : midiAgents) {
		blog(LOG_DEBUG, "Unloading Midi Device %s", midiAgent->get_midi_input_name().toStdString().c_str());
		midiAgent->clear_MidiHooks();
		delete midiAgent;
	}
	midiAgents.clear();
	blog(LOG_DEBUG, "DM::Unload");
}
/*
 * Returns QStringList of Port Names
 */
QStringList DeviceManager::get_input_ports_list()
{
	QStringList ports;
	unsigned int portCount = rtmidi::midi_in().get_port_count();
	for (unsigned int i = 0; i < portCount; ++i) {
		ports.append(QString::fromStdString(rtmidi::midi_in().get_port_name(i)));
	}
	return ports;
}
/*
 * Returns QStringList of Output  Port Names
 */
QStringList DeviceManager::get_output_ports_list()
{
	QStringList outports;
	unsigned int portCount = rtmidi::midi_out().get_port_count();
	for (unsigned int i = 0; i < portCount; ++i) {
		outports.append(QString::fromStdString(rtmidi::midi_out().get_port_name(i)));
	}
	return outports;
}
/*
 * Returns the port number of the specified device.
 * If the device isn't found (possibly due to being disconnected), returns -1
 */
int DeviceManager::get_input_port_number(const QString &deviceName)
{
	QStringList portsList = get_input_ports_list();
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
int DeviceManager::get_output_port_number(const QString &deviceName)
{
	QStringList portsList = get_output_ports_list();
	if (portsList.contains(deviceName)) {
		return portsList.indexOf(deviceName);
	} else {
		return -1;
	}
}
QVector<MidiAgent *> DeviceManager::get_active_midi_devices()
{
	return midiAgents;
}
MidiAgent *DeviceManager::get_midi_device(const QString &deviceName)
{
	MidiAgent *returndevice = nullptr;
	for (auto midiAgent : midiAgents) {
		if (midiAgent->get_midi_input_name() == deviceName) {
			returndevice = midiAgent;
			break;
		}
	}
	return returndevice;
}
QVector<MidiHook *> DeviceManager::get_midi_hooks(const QString &deviceName)
{
	if (deviceName != QString("No Devices Available")) {
		auto device = get_midi_device(deviceName);
		if (device != nullptr) {
			return device->GetMidiHooks();
		} else {
			return {};
		}
	}
	return {};
}
/* Registers a midi device.
 * Will create, store and enable a midi device.
 */
MidiAgent *DeviceManager::register_midi_device(const int &port, std::optional<int> outport)
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
QString DeviceManager::GetData()
{
	obs_data_t *return_data = obs_data_create();
	obs_data_array_t *data = obs_data_array_create();
	for (auto midiAgent : midiAgents) {
		obs_data_t *adata = obs_data_create_from_json(midiAgent->GetData().toStdString().c_str());
		obs_data_array_push_back(data, adata);
		obs_data_release(adata);
	}
	obs_data_set_array(return_data, "MidiDevices", data);
	QString rdata(obs_data_get_json(return_data));
	obs_data_array_release(data);
	obs_data_release(return_data);
	return rdata;
}
/**
 * Reload configuration -- for use with switching scene collecitons or profiles
 */
void DeviceManager::reload()
{
	this->Unload();
	emit reload_config();
}
