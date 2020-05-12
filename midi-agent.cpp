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

#include <obs-frontend-api.h>
#include <QtCore/QTime>
//#include <Python.h>
#include <functional>
#include <map>
#include <string>
#include <iostream>
#include "utils.h"
#include "midi-agent.h"
#include "obs-midi.h"
#include "obs-controller.h"

using namespace std;

///////////////////////
/* MIDI HOOK ROUTES */
//////////////////////
map<string, function<void(MidiHook* hook, int midiVal)>> funcMap = {
	// BUTTON ACTIONS
	{"SetCurrentScene", [](MidiHook* hook, int midiVal) { OBSController::SetCurrentScene(hook->param1.c_str()); }},
	{"SetPreviewScene", [](MidiHook* hook, int midiVal) { OBSController::SetPreviewScene(hook->param1.c_str()); }},
	{"SetCurrentSceneCollection", [](MidiHook* hook, int midiVal) { OBSController::SetCurrentSceneCollection(QString::fromStdString(hook->param1)); }},
	{"ResetSceneItem", [](MidiHook* hook, int midiVal) { OBSController::ResetSceneItem(hook->param1.c_str(), hook->param2.c_str()); }},

	{"TransitionToProgram", [](MidiHook* hook, int midiVal) {
		if (QString::fromStdString(hook->param1).isEmpty()){
			OBSController::TransitionToProgram();
		}
		else if (Utils::is_number(hook->param2)) {
			OBSController::TransitionToProgram(QString::fromStdString(hook->param1), stoi(hook->param2));
		}
		else {
			OBSController::TransitionToProgram(QString::fromStdString(hook->param1));
		}
	}},
	{"SetCurrentTransition", [](MidiHook* hook, int midiVal) { OBSController::SetCurrentTransition(QString::fromStdString(hook->param1)); }},
	{"SetTransitionDuration", [](MidiHook* hook, int midiVal) {
		if (Utils::is_number(hook->param1)) {
			OBSController::SetTransitionDuration(stoi(hook->param1));
		}
		else {
			OBSController::SetTransitionDuration(midiVal);
		}
	}},

	{"ToggleMute", [](MidiHook* hook, int midiVal) { OBSController::ToggleMute(QString::fromStdString(hook->param1)); }},
	{"SetMute", [](MidiHook* hook, int midiVal) { OBSController::SetMute(QString::fromStdString(hook->param1), (bool) stoi(hook->param2)); }},

	{"StartStopStreaming", [](MidiHook* hook, int midiVal) { OBSController::StartStopStreaming(); }},
	{"StartStreaming", [](MidiHook* hook, int midiVal) { OBSController::StartStreaming(); }},
	{"StopStreaming", [](MidiHook* hook, int midiVal) { OBSController::StopStreaming(); }},

	{"StartStopRecording", [](MidiHook* hook, int midiVal) { OBSController::StartStopRecording(); }},
	{"StartRecording", [](MidiHook* hook, int midiVal) { OBSController::StartRecording(); }},
	{"StopRecording", [](MidiHook* hook, int midiVal) { OBSController::StopRecording(); }},
	{"PauseRecording", [](MidiHook* hook, int midiVal) { OBSController::PauseRecording(); }},
	{"ResumeRecording", [](MidiHook* hook, int midiVal) { OBSController::ResumeRecording(); }},

	{"StartStopReplayBuffer", [](MidiHook* hook, int midiVal) { OBSController::StartStopReplayBuffer(); }},
	{"StartReplayBuffer", [](MidiHook* hook, int midiVal) { OBSController::StartReplayBuffer(); }},
	{"StopReplayBuffer", [](MidiHook* hook, int midiVal) { OBSController::StopReplayBuffer(); }},
	{"SaveReplayBuffer", [](MidiHook* hook, int midiVal) { OBSController::SaveReplayBuffer(); }},

	{"SetCurrentProfile", [](MidiHook* hook, int midiVal) { OBSController::SetCurrentProfile(QString::fromStdString(hook->param1)); }},
	{"SetTextGDIPlusText", [](MidiHook* hook, int midiVal) { OBSController::SetTextGDIPlusText(); }},
	{"SetBrowserSourceURL", [](MidiHook* hook, int midiVal) { OBSController::SetBrowserSourceURL(); }},
	{"ReloadBrowserSource", [](MidiHook* hook, int midiVal) { OBSController::ReloadBrowserSource(); }},
	{"TakeSourceScreenshot", [](MidiHook* hook, int midiVal) { OBSController::TakeSourceScreenshot(QString::fromStdString(hook->param1)); }},
	{"EnableSourceFilter", [](MidiHook* hook, int midiVal) { OBSController::EnableSourceFilter(); }},
	{"DisableSourceFilter", [](MidiHook* hook, int midiVal) { OBSController::DisableSourceFilter(); }},
	{"ToggleSourceFilter", [](MidiHook* hook, int midiVal) { OBSController::ToggleSourceFilter(); }},

	// CC ACTIONS
	{"SetVolume", [](MidiHook* hook, int midiVal) {  OBSController::SetVolume(QString::fromStdString(hook->param1), Utils::mapper(midiVal)); }},
	{"SetSyncOffset", [](MidiHook* hook, int midiVal) { OBSController::SetSyncOffset(QString::fromStdString(hook->param1), (int64_t) midiVal); }},
	{"SetSourcePosition", [](MidiHook* hook, int midiVal) { OBSController::SetSourcePosition(); }},
	{"SetSourceRotation", [](MidiHook* hook, int midiVal) { OBSController::SetSourceRotation(); }},
	{"SetSourceScale", [](MidiHook* hook, int midiVal) { OBSController::SetSourceScale(); }},
	{"SetGainFilter", [](MidiHook* hook, int midiVal) { OBSController::SetGainFilter(); }},
	{"SetOpacity", [](MidiHook* hook, int midiVal) { OBSController::SetOpacity(); }},
};




////////////////
// MIDI AGENT //
////////////////
MidiAgent::MidiAgent()
{
	name = "Midi Device (uninit)";
	midiin = new RtMidiIn();
	midiin->setCallback(&MidiAgent::HandleInput, this);

	// for testing..  remove me:
	MidiHook *mh = new MidiHook(7, "SetVolume", "Desktop Audio");
	AddMidiHook("control_change", mh);
}

MidiAgent::~MidiAgent()
{
	UnsetMidiDevice();
	delete midiin;
}

void MidiAgent::SetMidiDevice(int port)
{
	midiin->openPort(port);
	name = midiin->getPortName(port);
	blog(LOG_INFO, "MIDI device connected: [%d] %s", port, name.c_str());
}

void MidiAgent::UnsetMidiDevice()
{
	midiin->closePort();
}

void MidiAgent::HandleInput(double deltatime,
			    std::vector<unsigned char> *message, void *userData)
{
	MidiAgent *self = static_cast<MidiAgent *>(userData);

	string mType = Utils::getMidiMessageType(message->at(0));
	if (mType.empty()) { return; } // unknown message type. return.
	int mIndex = message->at(1);

	try
	{
		vector<MidiHook *> *hooks = &self->GetMidiHooksByType(mType);

		// check if hook exists for this note or cc index and launch it
		for (unsigned i = 0; i < hooks->size(); i++) {
			if (hooks->at(i)->index == mIndex) {
				self->TriggerInputCommand(hooks->at(i), (int)message->at(2));
			}
		}
	} catch (const std::exception&) { return; }
}

void MidiAgent::TriggerInputCommand(MidiHook* hook, int midiVal)
{

	blog(LOG_INFO, "Triggered: %d [%d] %s %s", hook->index, midiVal, hook->command.c_str(),
	     hook->param1.c_str());
	funcMap[hook->command](hook, midiVal);
}


void MidiAgent::AddMidiHook(string mType, MidiHook* hook)
{
	GetMidiHooksByType(mType).push_back(hook);
}

void MidiAgent::RemoveMidiHook(string mType, MidiHook* hook) {
	vector<MidiHook*> *hooks = &GetMidiHooksByType(mType);
	auto it = std::find(hooks->begin(), hooks->end(), hook);
	if (it != hooks->end()) {
		hooks->erase(it);
	}
}


vector<MidiHook *>& MidiAgent::GetMidiHooksByType(string mType) 
{
	if (mType == "note_on") {
		return noteOnHooks;
	} else if (mType == "note_off") {
		return noteOffHooks;
	} else if (mType == "control_change") {
		return ccHooks;
	} else {
		throw "GetMidiHooksByType FAILED. INVALID MIDI HOOK TYPE";
	}
}
