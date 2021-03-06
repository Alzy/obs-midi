#pragma once
#include "obs-midi.h"
#include "utils.h"
#include "midi-agent.h"
#include <QObject>
#include <QTimer>
/**
 *
 * Class: State
 * Stores values needed between functions
 *
 */
class state {
public:
	//** Do Transition Values **//
	inline static QString _CurrentTransition = "";
	inline static int _CurrentTransitionDuration = -1;
	inline static bool _TransitionWasCalled = false;
	//** Scene button Values **//
	inline static int previous_scene_norc;
	inline static int previous_preview_scene_norc;
	//** **/
	inline static bool swapping = false;
};
namespace Macro {
/**
 *Toggles a midi message between on and off states, should work on both notes and controls.
 */
inline void Toggle(MidiAgent* agent,MidiMessage *message)
{
	if (state::swapping)
		return;
	if (message->isNote()) {
		message->message_type = (message->message_type == "Note On") ? "Note Off" : "Note On";
	}
	message->value = (message->value > 0) ? 0 : 127;
	agent->send_message_to_midi_device((MidiMessage)*message);
}
/**
 *Toggles a midi message between on and off states, should work on both notes and controls.
 */
inline static void set_on_off(MidiAgent *agent, MidiMessage *message, bool on)
{
	if (state::swapping)
		return;
	if (message->isNote()) {
		message->message_type = (on) ? "Note On" : "Note Off";
	}
	message->value = (on) ? 127 : 0;
	agent->send_message_to_midi_device((MidiMessage)*message);
}

inline static void swap_buttons(MidiAgent *agent, MidiMessage *message, int button1, int button2)
{
	state::swapping = true;
	message->message_type = "Note Off";
	message->NORC = button1;
	message->value = 0;
	agent->send_message_to_midi_device((MidiMessage)*message);
	message->NORC = button2;
	message->message_type = "Note On";
	message->value = 1;
	agent->send_message_to_midi_device((MidiMessage)*message);
	state::swapping = false;
}
inline static void set_volume(MidiAgent* agent, MidiMessage *message, double vol) {
	uint8_t newvol = Utils::mapper2(cbrt(vol));
	message->value = newvol;
	agent->send_message_to_midi_device((MidiMessage)*message);
}
inline static void reset_midi(MidiAgent* agent) {
	MidiMessage message;
	for (int j = 0; j <= 16;j++) {
		message.channel = j;
		for (int i = 0; i <= 127; i++) {
			message.NORC = i;
			message.message_type = "Control Change";
			agent->send_message_to_midi_device(message);
			message.message_type = "Note Off";
			agent->send_message_to_midi_device(message);
		}
	}


}
}; // End Macro
