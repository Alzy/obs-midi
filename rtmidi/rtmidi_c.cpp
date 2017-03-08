#include <string.h>
#include <stdlib.h>
#include "rtmidi_c.h"
#include "RtMidi.h"

/* misc */
int rtmidi_sizeof_rtmidi_api ()
{
	return sizeof (RtMidiApi);
}

/* RtMidi API */
int rtmidi_get_compiled_api (enum RtMidiApi **apis) // return length for NULL argument.
{
	if (!apis || !(*apis)) {
		std::vector<RtMidi::Api> *v = new std::vector<RtMidi::Api> ();
		try {
			RtMidi::getCompiledApi (*v);
			int size = v->size ();
			delete v;
			return size;
		} catch (...) {
			return -1;
		}
	} else {
		try {
			std::vector<RtMidi::Api> *v = new std::vector<RtMidi::Api> ();
			RtMidi::getCompiledApi (*v);
			for (unsigned int i = 0; i < v->size (); i++)
				(*apis) [i] = (RtMidiApi) v->at (i);
			delete v;
			return 0;
		} catch (...) {
			return -1;
		}
	}
}

void rtmidi_error (MidiApi *api, enum RtMidiErrorType type, const char* errorString)
{
	std::string msg = errorString;
	api->error ((RtMidiError::Type) type, msg);
}

void rtmidi_open_port (RtMidiPtr device, unsigned int portNumber, const char *portName)
{
    std::string name = portName;
    try {
        ((RtMidi*) device->ptr)->openPort (portNumber, name);
    
    } catch (const RtMidiError & err) {
        device->ok  = false;
        device->msg = err.what ();
    }
}

void rtmidi_open_virtual_port (RtMidiPtr device, const char *portName)
{
    std::string name = portName;
    try {
        ((RtMidi*) device->ptr)->openVirtualPort (name);
    
    } catch (const RtMidiError & err) {
        device->ok  = false;
        device->msg = err.what ();
    }

}

void rtmidi_close_port (RtMidiPtr device)
{
    try { 
        ((RtMidi*) device->ptr)->closePort ();

    } catch (const RtMidiError & err) {
        device->ok  = false;
        device->msg = err.what ();
    }
}

unsigned int rtmidi_get_port_count (RtMidiPtr device)
{
    try {
        return ((RtMidi*) device->ptr)->getPortCount ();

    } catch (const RtMidiError & err) {
        device->ok  = false;
        device->msg = err.what ();
        return -1;
    }
}

const char* rtmidi_get_port_name (RtMidiPtr device, unsigned int portNumber)
{
    try {
        std::string name = ((RtMidi*) device->ptr)->getPortName (portNumber);
        return strdup (name.c_str ());
    
    } catch (const RtMidiError & err) {
        device->ok  = false;
        device->msg = err.what ();
        return "";
    }
}

/* RtMidiIn API */
RtMidiInPtr rtmidi_in_create_default ()
{
    RtMidiWrapper* wrp = new RtMidiWrapper;
    
    try {
        RtMidiIn* rIn = new RtMidiIn ();
        
        wrp->ptr = (void*) rIn;
        wrp->ok  = true;
        wrp->msg = "";
    
    } catch (const RtMidiError & err) {
        wrp->ptr = 0;
        wrp->ok  = false;
        wrp->msg = err.what ();
    }

    return wrp;
}

RtMidiInPtr rtmidi_in_create (enum RtMidiApi api, const char *clientName, unsigned int queueSizeLimit)
{
    std::string name = clientName;
    RtMidiWrapper* wrp = new RtMidiWrapper;
    
    try {
        RtMidiIn* rIn = new RtMidiIn ((RtMidi::Api) api, name, queueSizeLimit);
        
        wrp->ptr = (void*) rIn;
        wrp->ok  = true;
        wrp->msg = "";

    } catch (const RtMidiError & err) {
        wrp->ptr = 0; 
        wrp->ok  = false;
        wrp->msg = err.what ();
    }

    return wrp;
}

void rtmidi_in_free (RtMidiInPtr device)
{
    delete (RtMidiIn*) device->ptr;
    delete device;
}

enum RtMidiApi rtmidi_in_get_current_api (RtMidiPtr device)
{
    try {
        return (RtMidiApi) ((RtMidiIn*) device->ptr)->getCurrentApi ();
    
    } catch (const RtMidiError & err) {
        device->ok  = false;
        device->msg = err.what ();

        return RT_MIDI_API_UNSPECIFIED;
    }
}

class CallbackProxyUserData
{
  public:
	CallbackProxyUserData (RtMidiCCallback cCallback, void *userData)
		: c_callback (cCallback), user_data (userData)
	{
	}
	RtMidiCCallback c_callback;
	void *user_data;
};

void callback_proxy (double timeStamp, std::vector<unsigned char> *message, void *userData)
{
	CallbackProxyUserData* data = reinterpret_cast<CallbackProxyUserData*> (userData);
	data->c_callback (timeStamp, message->data (), data->user_data);
}

void rtmidi_in_set_callback (RtMidiInPtr device, RtMidiCCallback callback, void *userData)
{
    try {
        void *data = (void *) new CallbackProxyUserData (callback, userData);
        ((RtMidiIn*) device->ptr)->setCallback (callback_proxy, data);
    
    } catch (const RtMidiError & err) {
        device->ok  = false;
        device->msg = err.what ();
    }
}

void rtmidi_in_cancel_callback (RtMidiInPtr device)
{
    try {
        ((RtMidiIn*) device->ptr)->cancelCallback ();

    } catch (const RtMidiError & err) {
        device->ok  = false;
        device->msg = err.what ();
    }
}

void rtmidi_in_ignore_types (RtMidiInPtr device, bool midiSysex, bool midiTime, bool midiSense)
{
	((RtMidiIn*) device->ptr)->ignoreTypes (midiSysex, midiTime, midiSense);
}

double rtmidi_in_get_message (RtMidiInPtr device, 
                              unsigned char **message, 
                              size_t * size)
{
    try {
        // FIXME: use allocator to achieve efficient buffering
        std::vector<unsigned char> *v = new std::vector<unsigned char> ();
        double ret = ((RtMidiIn*) device->ptr)->getMessage (v);
        *size = v->size ();

        if (v->size () > 0) {
            *message = (unsigned char *) 
                        malloc (sizeof (unsigned char *) * (int) v->size ());
            memcpy (*message, v->data (), (int) v->size ());
        }
        delete v;
        return ret;
    } 
    catch (const RtMidiError & err) {
        device->ok  = false;
        device->msg = err.what ();
        return -1;
    }
    catch (...) {
        device->ok  = false;
        device->msg = "Unknown error";
        return -1;
    }
}

/* RtMidiOut API */
RtMidiOutPtr rtmidi_out_create_default ()
{
    RtMidiWrapper* wrp = new RtMidiWrapper;

    try {
        RtMidiOut* rOut = new RtMidiOut ();
        
        wrp->ptr = (void*) rOut;
        wrp->ok  = true;
        wrp->msg = "";
    
    } catch (const RtMidiError & err) {
        wrp->ptr = 0;
        wrp->ok  = false;
        wrp->msg = err.what ();
    }

    return wrp;
}

RtMidiOutPtr rtmidi_out_create (enum RtMidiApi api, const char *clientName)
{
    RtMidiWrapper* wrp = new RtMidiWrapper;
    std::string name = clientName;

    try {
        RtMidiOut* rOut = new RtMidiOut ((RtMidi::Api) api, name);
        
        wrp->ptr = (void*) rOut;
        wrp->ok  = true;
        wrp->msg = "";
    
    } catch (const RtMidiError & err) {
        wrp->ptr = 0;
        wrp->ok  = false;
        wrp->msg = err.what ();
    }


    return wrp;
}

void rtmidi_out_free (RtMidiOutPtr device)
{
    delete (RtMidiOut*) device->ptr;
    delete device;
}

enum RtMidiApi rtmidi_out_get_current_api (RtMidiPtr device)
{
    try {
        return (RtMidiApi) ((RtMidiOut*) device->ptr)->getCurrentApi ();

    } catch (const RtMidiError & err) {
        device->ok  = false;
        device->msg = err.what ();

        return RT_MIDI_API_UNSPECIFIED;
    }
}

int rtmidi_out_send_message (RtMidiOutPtr device, const unsigned char *message, int length)
{
    try {
        // FIXME: use allocator to achieve efficient buffering
        std::vector<unsigned char> *v = new std::vector<unsigned char> (length);
        memcpy (v->data (), message, length);
        ((RtMidiOut*) device->ptr)->sendMessage (v);
        delete v;
        return 0;
    }
    catch (const RtMidiError & err) {
        device->ok  = false;
        device->msg = err.what ();
        return -1;
    }
    catch (...) {
        device->ok  = false;
        device->msg = "Unknown error";
        return -1;
    }
}
