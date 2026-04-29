#ifndef __AUDIO_ADAPTER_H__
#define __AUDIO_ADAPTER_H__

#include "devicehandle_factory.h"
#include "logger.h"

#include <rtaudio/RtAudio.h>

namespace miniaudioengine::adapters
{

typedef RtAudioStreamStatus AudioStreamStatus;
typedef RtAudio::StreamParameters AudioStreamParameters;

class AudioCallbackHandler
{
public:
  static int audio_callback(void *output_buffer, void *input_buffer, unsigned int n_frames,
                            double stream_time, AudioStreamStatus status, void *user_data) noexcept {
                              return 1;
                            }
};

/** @class AudioAdapter
 *  @brief Adapter class that encapsulates RtAudio, separating the audio controller from direct dependency on RtAudio.
 */
class AudioAdapter
{
public:
  AudioAdapter() = default;
  AudioAdapter(const AudioAdapter&) = default;
  AudioAdapter& operator=(const AudioAdapter&) = default;
  virtual ~AudioAdapter() = default;

  unsigned int get_device_count();
  std::vector<DeviceHandlePtr> get_devices();

  bool open_stream(AudioStreamParameters &params,
                  unsigned int sample_rate,
                  unsigned int buffer_frames,
                  void *callback_context);

  bool close_stream();
  bool stop_stream();

  bool is_stream_open();
  bool is_stream_running();

private:
  RtAudio m_rtaudio;

  static DeviceHandlePtr make_device_handle(const RtAudio::DeviceInfo &info, unsigned int id)
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

} // namespace miniaudioengine::adapters

#endif // __AUDIO_ADAPTER_H__