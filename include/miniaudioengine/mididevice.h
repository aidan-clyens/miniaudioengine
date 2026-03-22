#ifndef __MIDI_DEVICE_H__
#define __MIDI_DEVICE_H__

/**
 * @file mididevice.h
 * @deprecated This header is a compatibility stub. Use DeviceHandle from devicehandle.h instead.
 */
#include "devicehandle.h"

namespace miniaudioengine::midi
{

/** @deprecated Use DeviceHandle / DeviceHandlePtr instead. */
[[deprecated("Use DeviceHandle from devicehandle.h instead of MidiDevice")]]
typedef DeviceHandle MidiDevice;

/** @deprecated Use DeviceHandlePtr instead. */
[[deprecated("Use DeviceHandlePtr from devicehandle.h instead of MidiDevicePtr")]]
typedef DeviceHandlePtr MidiDevicePtr;

} // namespace miniaudioengine::midi

#endif // __MIDI_DEVICE_H__
