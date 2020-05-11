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

MidiAgent::MidiAgent()
{
	name = "Alfredo";
	midiin = new RtMidiIn();
	midiin->setCallback(&MidiAgent::HandleInput, this);

	MidiHook *mh = new MidiHook(1, "SetVolume", "Desktop Audio");
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
	int mIndex = message->at(1);

	vector<MidiHook *> *hooks = &self->GetMidiHooksByType(mType);

	// check if hook exists for this note or cc index and launch it
	for (unsigned i = 0; i < hooks->size(); i++) {
		if (hooks->at(i)->index == mIndex) {
			self->TriggerInputCommand(hooks->at(i), (int)message->at(2));
		}
	}
}

void MidiAgent::TriggerInputCommand(MidiHook* hook, int midiVal)
{

	blog(LOG_INFO, "Triggered: %d [%d] %s %s", hook->index, midiVal, hook->command.c_str(),
	     hook->param.c_str());
	MidiAgent::executor(hook, hook->command, midiVal);

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

void MidiAgent::executor(MidiHook *hook, std::string name, int midiVal)
{
	std::map < std::string,	std::function < void(std::string name, int value)>> funcMap = {
		{"SetVolume", [](std::string audio,int  y) { float x = (float) y; OBSController::SetVolume(QString::fromStdString(audio), Utils::mapper(x)); }},
		
		{"sub", [](std::string x, int y) {  }}};
	funcMap[hook->command](hook->param, midiVal);


}

void MidiAgent::executor(MidiHook *hook, std::string name, float midiVal)
{
	std::map < std::string,	std::function < void(std::string name, float y)>> funcMap = {
		{"SetVolume", [](std::string audio,float  y) {  OBSController::SetVolume(QString::fromStdString(audio), Utils::mapper(y)); }},
		{"ToggleMute", [](std::string audio,float  y) {  OBSController::ToggleMute(QString::fromStdString(audio)); }},
		{"TakeSourceScreenshot", [](std::string audio,float  y) {  OBSController::TakeSourceScreenshot(QString::fromStdString(audio)); }}};
	funcMap[hook->command](hook->param,  midiVal);
}
void MidiAgent::executor(MidiHook *hook, int y){
	std::map < std::string,	std::function < void(int y)>> funcMap = {
	{"SetTransitionDuration", [](int y) {  OBSController::SetTransitionDuration(y); }}};
	funcMap[hook->command](y);
}
void MidiAgent::executor(MidiHook *hook){
	std::map<std::string, std::function<void()>> funcMap = {
		{"StartStopReplayBuffer", []() {  OBSController::StartStopReplayBuffer(); }},
		{"StartReplayBuffer", []() { OBSController::StartReplayBuffer(); }},
		{"StopReplayBuffer", []() { OBSController::StopReplayBuffer(); }},
		{"SaveReplayBuffer", []() {  OBSController::SaveReplayBuffer(); }},
		{"StartStopStreaming", []() { OBSController::StartStopStreaming(); }},
		{"StartStreaming"  , []() {  OBSController::StartStreaming(); }},
		{"StopStreaming", []() { OBSController::StopStreaming(); }},
		{"StartStopRecording", []() { OBSController::StartStopRecording(); }},
		{"StartRecording", []() { OBSController::StartRecording; }},
		{"StopRecording", []() { OBSController::StopRecording(); }},
		{"PauseRecording", []() { OBSController::PauseRecording(); }},
		{"ResumeRecording", []() { OBSController::ResumeRecording(); }}};
	funcMap[hook->command]();
	}
/*
TODO: Add the following maps

		"SetCurrentScene":           [1,  'scene-name'                      ],	
                "SetPreviewScene":           [1,  'scene-name'                      ],
                "TransitionToProgram":       [1,  'scene-name'                      ],
                "SetCurrentTransition":      [1,  'transition'                      ],
                "SetCurrentProfile":         [1,  'profile'                         ],
                "SetCurrentSceneCollection": [1,  'sc-name'                         ],
                "ResetSceneItem":            [1,  'item'                            ],
                "SetMute":                   [2,  'source', 'bool'                  ],
                "SetSyncOffset":             [2,  'source', 'offset'                ],
                "ReloadBrowserSource":       [2,  'source', 'url'                   ],
                "EnableSourceFilter":        [2,  'source', 'filter'                ],
                "DisableSourceFilter":       [2,  'source', 'filter'                ],
                "SetTextGDIPlusText":        [2,  'source', 'text'                  ],
                "SetBrowserSourceURL":       [2,  'source', 'url'                   ],
                "SetSourceVisibility":       [2,  'item',   'bool'                  ],
                "ToggleSourceVisibility":    [2,  'item',   'bool'                  ],
                "SetSourceScale":            [3,  'source', 'item',     'scale'     ],
                "SetSourcePosition":         [3,  'source', 'item',     'position'  ],
                "SetSourceRotation":         [3,  'source', 'item',     'rotation'  ],
                "SetGainFilter":             [3,  'source', 'filter',   'db'        ],
                "ToggleSourceFilter":        [3,  'source', 'filter',   'bool'      ]
*/
