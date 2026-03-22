#ifndef __AUDIO_DEVICE_H__
#define __AUDIO_DEVICE_H__

/**
 * @file audiodevice.h
 * @deprecated This header is a compatibility stub. Use DeviceHandle from devicehandle.h instead.
 */
#include "devicehandle.h"

namespace miniaudioengine::audio
{

/** @deprecated Use DeviceHandle / DeviceHandlePtr instead. */
[[deprecated("Use DeviceHandle from devicehandle.h instead of AudioDevice")]]
typedef DeviceHandle AudioDevice;

/** @deprecated Use DeviceHandlePtr instead. */
[[deprecated("Use DeviceHandlePtr from devicehandle.h instead of AudioDevicePtr")]]
typedef DeviceHandlePtr AudioDevicePtr;

} // namespace miniaudioengine::audio

#endif // __AUDIO_DEVICE_H__
