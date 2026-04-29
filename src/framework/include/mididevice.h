#ifndef __MIDI_DEVICE_H__
#define __MIDI_DEVICE_H__

/**
 * @file mididevice.h
 * @deprecated This header is a compatibility stub. Use Device from device.h instead.
 */
#include "device.h"

namespace miniaudioengine::midi
{

/** @deprecated Use Device / DeviceHandlePtr instead. */
[[deprecated("Use Device from device.h instead of MidiDevice")]]
typedef Device MidiDevice;

/** @deprecated Use DeviceHandlePtr instead. */
[[deprecated("Use DeviceHandlePtr from device.h instead of MidiDevicePtr")]]
typedef DeviceHandlePtr MidiDevicePtr;

} // namespace miniaudioengine::midi

#endif // __MIDI_DEVICE_H__
