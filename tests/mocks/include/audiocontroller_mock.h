#ifndef __AUDIO_CONTROLLER_MOCK_H__
#define __AUDIO_CONTROLLER_MOCK_H__

#include <optional>
#include <thread>
#include <chrono>
#include <memory>

#include "audiocontroller.h"
#include "audiocallbackhandler.h"
#include "trackmanager.h"
#include "logger.h"

using namespace MinimalAudioEngine::Control;
using namespace MinimalAudioEngine::Data;

namespace MinimalAudioEngine::Test
{

enum RtAudioErrorType
{
  RTAUDIO_NO_ERROR = 0,
  RTAUDIO_WARNING,
  RTAUDIO_ERROR,
};

class AudioControllerMock : public IAudioController
{
public:
  static AudioControllerMock& instance()
  {
    static AudioControllerMock instance;
    return instance;
  }

  // TODO
  std::vector<RtAudio::DeviceInfo> get_audio_devices() override
  {
    return {};
  }

  // TODO
  void set_output_device(const AudioDevice &device) override
  {
    // Check if device is an output device
    if (!device.is_output())
    {
      LOG_ERROR("AudioControllerMock: ", device.name, " is not an output device.");
      throw std::invalid_argument("AudioControllerMock: " + device.name + " is not an output device.");
    }

    // Close stream if already open
    if (m_is_stream_running)
    {
      LOG_INFO("AudioControllerMock: Closed existing stream.");
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
      m_is_stream_running = false;
      m_stream_state = eAudioState::Idle;
    }

    LOG_INFO("AudioControllerMock: Output device set to ", device.name);
    m_audio_output_device = device;
  }

  bool start_stream() override
  {
    // Use base class validation
    if (!validate_start_preconditions())
    {
      return false;
    }

    if (!register_active_tracks())
    {
      return false;
    }

    // Simulate successful stream start
    LOG_INFO("AudioControllerMock: RtAudio stream started successfully.");
    m_stream_state = eAudioState::Playing;
    m_is_stream_running = true;

    return true;
  }

  bool stop_stream() override
  {
    if (m_stream_state != eAudioState::Playing)
    {
      LOG_WARNING("AudioControllerMock: Stream is not running. No action taken.");
      return false;
    }

    LOG_INFO("AudioControllerMock: RtAudio stream stopped successfully.");
    m_stream_state = eAudioState::Stopped;
    m_is_stream_running = false;

    return true;
  }

private:
  AudioControllerMock() = default;
  virtual ~AudioControllerMock() = default;

  bool m_is_stream_running{false};
};

} // namespace MinimalAudioEngine::Test

#endif // __AUDIO_CONTROLLER_MOCK_H__