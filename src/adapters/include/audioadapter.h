#ifndef __AUDIO_ADAPTER_H__
#define __AUDIO_ADAPTER_H__

#include "device.h"
#include "logger.h"
#include "ringbuffer.h"
#include "adapter.h"

#include <memory>
#include <rtaudio/RtAudio.h>

namespace miniaudioengine::adapters
{

typedef RtAudioStreamStatus AudioStreamStatus;
typedef RtAudio::StreamParameters AudioStreamParameters;
typedef RtAudio::DeviceInfo AudioDeviceInfo;

class AudioCallbackHandler : public framework::IAdapterCallback
{
public:

  struct Params
  {
    framework::eInputOutputDirection direction;
  };

  static int audio_callback(void *output_buffer, void *input_buffer, unsigned int n_frames,
                            double stream_time, AudioStreamStatus status, void *user_data) noexcept;
};

/** @class AudioAdapter
 *  @brief Adapter class that encapsulates RtAudio, separating the audio controller from direct dependency on RtAudio.
 */
class AudioAdapter : public framework::IAdapter<DeviceInfo>
{
public:
  AudioAdapter();
  AudioAdapter(const AudioAdapter&) = default;
  AudioAdapter& operator=(const AudioAdapter&) = default;
  virtual ~AudioAdapter() = default;

  unsigned int get_device_count();
  std::vector<DevicePtr> get_devices();

  bool open_stream(const DeviceInfo &info, const framework::eInputOutputDirection &direction);
  bool close_stream();
  bool stop_stream();

  bool is_stream_open();
  bool is_stream_running();

private:
  std::unique_ptr<RtAudio> p_rtaudio;
  // TODO - Device should own BufferPtr, not AudioAdapter
  framework::BufferPtr p_buffer;

  static DevicePtr make_device_handle(const AudioDeviceInfo &info, unsigned int id)
  {
    return DeviceHandleFactory::make_audio(
        id,
        info.name,
        info.isDefaultInput,
        info.isDefaultOutput,
        info.outputChannels,
        info.inputChannels,
        info.duplexChannels,
        info.preferredSampleRate,
        info.sampleRates);
  }
};

using AudioAdapterPtr = std::shared_ptr<AudioAdapter>;

} // namespace miniaudioengine::adapters

#endif // __AUDIO_ADAPTER_H__