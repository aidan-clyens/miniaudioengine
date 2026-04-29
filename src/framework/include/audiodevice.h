#ifndef __AUDIO_DEVICE_H__
#define __AUDIO_DEVICE_H__

/**
 * @file audiodevice.h
 * @deprecated This header is a compatibility stub. Use Device from device.h instead.
 */
#include "device.h"

namespace miniaudioengine::audio
{

/** @deprecated Use Device / DeviceHandlePtr instead. */
[[deprecated("Use Device from device.h instead of AudioDevice")]]
typedef Device AudioDevice;

/** @deprecated Use DeviceHandlePtr instead. */
[[deprecated("Use DeviceHandlePtr from device.h instead of AudioDevicePtr")]]
typedef DeviceHandlePtr AudioDevicePtr;

} // namespace miniaudioengine::audio

#endif // __AUDIO_DEVICE_H__
